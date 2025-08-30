#pragma once
#include "IPathfinder.h"
#include <queue>

using PQElement = std::pair<int, Point>;

class BidirectionalPathfinder : public IPathfinder {
public:
    BidirectionalPathfinder();
    void init(Grid& grid) override;
    AlgoState step() override;
    std::string getName() const override;
    void onWallChanged(const Point& pos) override;

private:
    int heuristic(const Point& a, const Point& b);

    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> m_pq_fwd;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> m_pq_bwd;
    
    Point m_start;
    Point m_end;
    Point m_meetingPoint;
    int m_bestPathCost;
};