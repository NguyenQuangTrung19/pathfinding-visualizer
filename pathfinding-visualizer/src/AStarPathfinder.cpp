#include "AStarPathfinder.h"
#include <cmath>

AStarPathfinder::AStarPathfinder(bool isDijkstra) : m_isDijkstra(isDijkstra) {
    m_name = isDijkstra ? "Dijkstra" : "A* Search";
}

std::string AStarPathfinder::getName() const { return m_name; }

void AStarPathfinder::init(Grid& grid) {
    m_grid = &grid;
    m_grid->reset();
    m_start = m_grid->getStart();
    m_end = m_grid->getEnd();
    m_pq = {};
    m_path.clear();

    m_grid->getNode(m_start.first, m_start.second).g_score = 0;
    int h_start = m_isDijkstra ? 0 : heuristic(m_start, m_end);
    m_pq.push({h_start, m_start});
}

void AStarPathfinder::onWallChanged(const Point& pos) {
    init(*m_grid);
}

AlgoState AStarPathfinder::step() {
    if (m_pq.empty()) {
        m_grid->setCurrentlyProcessing({-1, -1});
        return AlgoState::PathNotFound;
    }

    Point current_pos = m_pq.top().second;
    m_pq.pop();

    Node& current_node = m_grid->getNode(current_pos.first, current_pos.second);
    if (current_node.visited_fwd) {
        return AlgoState::Running;
    }
    current_node.visited_fwd = true;
    m_grid->setCurrentlyProcessing(current_pos);

    if (current_pos == m_end) {
        m_path = m_grid->reconstructPath();
        m_grid->setCurrentlyProcessing({-1, -1});
        return AlgoState::PathFound;
    }

    const int dr[] = {-1, 1, 0, 0, -1, -1, 1, 1};
    const int dc[] = {0, 0, -1, 1, -1, 1, -1, 1};

    for (int i = 0; i < 8; ++i) { // 8 directions for diagonal movement
        Point neighbor_pos = {current_pos.first + dr[i], current_pos.second + dc[i]};
        if (m_grid->isValid(neighbor_pos.first, neighbor_pos.second) && !m_grid->getNode(neighbor_pos.first, neighbor_pos.second).is_wall) {
            int cost = (i < 4) ? 10 : 14; // 1 for straight, ~1.4 for diagonal (multiplied by 10)
            Node& neighbor_node = m_grid->getNode(neighbor_pos.first, neighbor_pos.second);
            int tentative_g_score = current_node.g_score + cost;

            if (tentative_g_score < neighbor_node.g_score) {
                neighbor_node.parent = current_pos;
                neighbor_node.g_score = tentative_g_score;
                int h_score = m_isDijkstra ? 0 : heuristic(neighbor_pos, m_end);
                m_pq.push({tentative_g_score + h_score, neighbor_pos});
            }
        }
    }
    return AlgoState::Running;
}

int AStarPathfinder::heuristic(const Point& a, const Point& b) {
    // Manhattan distance
    return (std::abs(a.first - b.first) + std::abs(a.second - b.second)) * 10;
}