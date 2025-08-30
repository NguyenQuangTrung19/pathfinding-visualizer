#pragma once
#include "IPathfinder.h"
#include <vector>
#include <memory>

class Visualizer {
public:
    Visualizer();
    void drawUI(const std::vector<std::unique_ptr<IPathfinder>>& pathfinders, const std::vector<AlgoState>& states, const Point* cursor, int speed);

private:
    void drawGrid(int y_offset, int x_offset, const IPathfinder& pf, AlgoState state, const Point* cursor);
};