#pragma once
#include "Grid.h"
#include <string>
#include <vector>

enum class AlgoState { Running, PathFound, PathNotFound, Paused };

class IPathfinder {
public:
    virtual ~IPathfinder() = default;
    virtual void init(Grid& grid) = 0;
    virtual AlgoState step() = 0;
    virtual std::string getName() const = 0;
    virtual void onWallChanged(const Point& pos) = 0;
    Grid& getGrid() { return *m_grid; }
    const std::vector<Point>& getPath() const { return m_path; }

protected:
    Grid* m_grid = nullptr;
    std::vector<Point> m_path;
};