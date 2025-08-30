#pragma once
#include "IPathfinder.h"
#include <queue>

using PQElement = std::pair<int, Point>;

class AStarPathfinder : public IPathfinder {
public:
    AStarPathfinder(bool isDijkstra = false);
    void init(Grid& grid) override;
    AlgoState step() override;
    std::string getName() const override;
    void onWallChanged(const Point& pos) override;

private:
    int heuristic(const Point& a, const Point& b);

    bool m_isDijkstra;
    std::string m_name;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> m_pq;
    Point m_start;
    Point m_end;
};