#include "Grid.h"
#include <algorithm>
#include <stdexcept>

Grid::Grid(int r, int c) : rows(r), cols(c) {
    if (r <= 0 || c <= 0) {
        throw std::invalid_argument("Rows and cols must be positive.");
    }
    grid_data.resize(rows, std::vector<Node>(cols));
}

const Node& Grid::getNode(int r, int c) const {
    return grid_data[r][c];
}

Node& Grid::getNode(int r, int c) {
    return grid_data[r][c];
}

bool Grid::isValid(int r, int c) const {
    return r >= 0 && r < rows && c >= 0 && c < cols;
}

void Grid::generateMaze() {
    for (auto& row : grid_data) std::fill(row.begin(), row.end(), Node{true});
    std::random_device rd;
    std::mt19937 rng(rd());
    int start_r = (rng() % ((rows - 1) / 2)) * 2 + 1;
    int start_c = (rng() % ((cols - 1) / 2)) * 2 + 1;
    carvePath(start_r, start_c, rng);
    
    start_pos = {1, 1};
    grid_data[1][1].is_wall = false;
    end_pos = {rows - 2, cols - 2};
    grid_data[rows - 2][cols - 2].is_wall = false;
}

void Grid::reset() {
    for (auto& row : grid_data) {
        for (auto& node : row) {
            node.g_score = std::numeric_limits<int>::max();
            node.parent = {-1, -1};
            node.g_score_bwd = std::numeric_limits<int>::max();
            node.parent_bwd = {-1, -1};
            node.visited_fwd = false;
            node.visited_bwd = false;
        }
    }
}

void Grid::toggleWall(const Point& pos) {
    if (pos != start_pos && pos != end_pos) {
        grid_data[pos.first][pos.second].is_wall = !grid_data[pos.first][pos.second].is_wall;
    }
}

void Grid::setCurrentlyProcessing(const Point& pos) {
    currently_processing = pos;
}

std::vector<Point> Grid::reconstructPath(bool isBidirectional, Point meetingPoint) {
    std::vector<Point> path;
    if(isBidirectional) {
        Point curr = meetingPoint;
        while (curr.first != -1) {
            path.push_back(curr);
            curr = getNode(curr.first, curr.second).parent;
        }
        std::reverse(path.begin(), path.end());
        
        curr = getNode(meetingPoint.first, meetingPoint.second).parent_bwd;
        while (curr.first != -1) {
            path.push_back(curr);
            curr = getNode(curr.first, curr.second).parent_bwd;
        }
    } else {
        Point curr = end_pos;
        while (curr.first != -1) {
            path.push_back(curr);
            if (curr == start_pos) break;
            curr = getNode(curr.first, curr.second).parent;
        }
        std::reverse(path.begin(), path.end());
    }
    return path;
}

void Grid::carvePath(int r, int c, std::mt19937& rng) {
    grid_data[r][c].is_wall = false;
    int dr[] = {-2, 2, 0, 0};
    int dc[] = {0, 0, -2, 2};
    std::vector<int> directions = {0, 1, 2, 3};
    std::shuffle(directions.begin(), directions.end(), rng);
    for (int dir : directions) {
        int next_r = r + dr[dir];
        int next_c = c + dc[dir];
        if (isValid(next_r, next_c) && grid_data[next_r][next_c].is_wall) {
            grid_data[r + dr[dir] / 2][c + dc[dir] / 2].is_wall = false;
            carvePath(next_r, next_c, rng);
        }
    }
}