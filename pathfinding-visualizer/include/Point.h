#pragma once
#include <utility>
#include <limits>

using Point = std::pair<int, int>;

struct Node {
    bool is_wall = false;
    int weight = 1;

    // Dữ liệu cho các thuật toán
    int g_score = std::numeric_limits<int>::max();
    Point parent = {-1, -1};
    
    // Dữ liệu cho tìm kiếm 2 chiều
    int g_score_bwd = std::numeric_limits<int>::max();
    Point parent_bwd = {-1, -1};

    bool visited_fwd = false;
    bool visited_bwd = false;
};