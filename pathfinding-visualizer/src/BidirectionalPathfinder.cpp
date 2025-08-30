#include "BidirectionalPathfinder.h"
#include <cmath>

BidirectionalPathfinder::BidirectionalPathfinder() {
    m_name = "Bidirectional A*";
}

std::string BidirectionalPathfinder::getName() const { return m_name; }

void BidirectionalPathfinder::init(Grid& grid) {
    m_grid = &grid;
    m_grid->reset();
    m_start = m_grid->getStart();
    m_end = m_grid->getEnd();
    m_pq_fwd = {};
    m_pq_bwd = {};
    m_path.clear();
    m_meetingPoint = {-1, -1};
    m_bestPathCost = std::numeric_limits<int>::max();

    m_grid->getNode(m_start.first, m_start.second).g_score = 0;
    m_pq_fwd.push({heuristic(m_start, m_end), m_start});
    
    m_grid->getNode(m_end.first, m_end.second).g_score_bwd = 0;
    m_pq_bwd.push({heuristic(m_start, m_end), m_end});
}

void BidirectionalPathfinder::onWallChanged(const Point& pos) {
    init(*m_grid);
}

AlgoState BidirectionalPathfinder::step() {
    const int dr[] = {-1, 1, 0, 0, -1, -1, 1, 1};
    const int dc[] = {0, 0, -1, 1, -1, 1, -1, 1};

    // --- Forward Step ---
    if (!m_pq_fwd.empty()) {
        Point curr_pos = m_pq_fwd.top().second;
        m_pq_fwd.pop();
        Node& curr_node = m_grid->getNode(curr_pos.first, curr_pos.second);
        if (!curr_node.visited_fwd) {
            curr_node.visited_fwd = true;
            m_grid->setCurrentlyProcessing(curr_pos);

            if (curr_node.visited_bwd) { // Met the other search
                int pathCost = curr_node.g_score + curr_node.g_score_bwd;
                if(pathCost < m_bestPathCost) {
                    m_bestPathCost = pathCost;
                    m_meetingPoint = curr_pos;
                }
            }
            if (!m_pq_fwd.empty() && !m_pq_bwd.empty() && m_pq_fwd.top().first + m_pq_bwd.top().first >= m_bestPathCost) {
                 m_path = m_grid->reconstructPath(true, m_meetingPoint);
                 return AlgoState::PathFound;
            }

            for (int i = 0; i < 8; ++i) {
                Point neighbor_pos = {curr_pos.first + dr[i], curr_pos.second + dc[i]};
                if (m_grid->isValid(neighbor_pos.first, neighbor_pos.second) && !m_grid->getNode(neighbor_pos.first, neighbor_pos.second).is_wall) {
                    int cost = (i < 4) ? 10 : 14;
                    Node& neighbor_node = m_grid->getNode(neighbor_pos.first, neighbor_pos.second);
                    int tentative_g_score = curr_node.g_score + cost;
                    if (tentative_g_score < neighbor_node.g_score) {
                        neighbor_node.parent = curr_pos;
                        neighbor_node.g_score = tentative_g_score;
                        m_pq_fwd.push({tentative_g_score + heuristic(neighbor_pos, m_end), neighbor_pos});
                    }
                }
            }
        }
    } else { return AlgoState::PathNotFound; }

    // --- Backward Step ---
    if (!m_pq_bwd.empty()) {
        Point curr_pos = m_pq_bwd.top().second;
        m_pq_bwd.pop();
        Node& curr_node = m_grid->getNode(curr_pos.first, curr_pos.second);
        if (!curr_node.visited_bwd) {
            curr_node.visited_bwd = true;
            if (curr_node.visited_fwd) { // Met the other search
                int pathCost = curr_node.g_score + curr_node.g_score_bwd;
                if (pathCost < m_bestPathCost) {
                    m_bestPathCost = pathCost;
                    m_meetingPoint = curr_pos;
                }
            }
             if (!m_pq_fwd.empty() && !m_pq_bwd.empty() && m_pq_fwd.top().first + m_pq_bwd.top().first >= m_bestPathCost) {
                 m_path = m_grid->reconstructPath(true, m_meetingPoint);
                 return AlgoState::PathFound;
            }

            for (int i = 0; i < 8; ++i) {
                Point neighbor_pos = {curr_pos.first + dr[i], curr_pos.second + dc[i]};
                if (m_grid->isValid(neighbor_pos.first, neighbor_pos.second) && !m_grid->getNode(neighbor_pos.first, neighbor_pos.second).is_wall) {
                    int cost = (i < 4) ? 10 : 14;
                    Node& neighbor_node = m_grid->getNode(neighbor_pos.first, neighbor_pos.second);
                    int tentative_g_score = curr_node.g_score_bwd + cost;
                    if (tentative_g_score < neighbor_node.g_score_bwd) {
                        neighbor_node.parent_bwd = curr_pos;
                        neighbor_node.g_score_bwd = tentative_g_score;
                        m_pq_bwd.push({tentative_g_score + heuristic(neighbor_pos, m_start), neighbor_pos});
                    }
                }
            }
        }
    } else { return AlgoState::PathNotFound; }

    return AlgoState::Running;
}

int BidirectionalPathfinder::heuristic(const Point& a, const Point& b) {
    return (std::abs(a.first - b.first) + std::abs(a.second - b.second)) * 10;
}