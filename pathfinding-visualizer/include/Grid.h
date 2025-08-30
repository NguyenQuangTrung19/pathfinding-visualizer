#pragma once
#include "Point.h"
#include <vector>
#include <string>
#include <random>

class Grid {
public:
    explicit Grid(int r, int c);

    int getRows() const { return rows; }
    int getCols() const { return cols; }
    Point getStart() const { return start_pos; }
    Point getEnd() const { return end_pos; }
    const Node& getNode(int r, int c) const;
    Node& getNode(int r, int c);
    const Point& getCurrentlyProcessing() const { return currently_processing; }
    
    bool isValid(int r, int c) const;
    void generateMaze();
    void reset();
    void toggleWall(const Point& pos);
    void setCurrentlyProcessing(const Point& pos);
    std::vector<Point> reconstructPath(bool isBidirectional = false, Point meetingPoint = {-1,-1});

private:
    void carvePath(int r, int c, std::mt19937& rng);

    int rows;
    int cols;
    std::vector<std::vector<Node>> grid_data;
    Point start_pos;
    Point end_pos;
    Point currently_processing = {-1, -1};
};