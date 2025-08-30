#include "Visualizer.h"
#include "Util.h"
#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

Visualizer::Visualizer() {
    util::hideCursor();
}

void Visualizer::drawGrid(int y_offset, int x_offset, const IPathfinder& pf, AlgoState state, const Point* cursor) {
    const Grid& grid = pf.getGrid();
    Point start = grid.getStart();
    Point end = grid.getEnd();
    Point processing = grid.getCurrentlyProcessing();
    
    const std::vector<Point>& path = pf.getPath();
    std::vector<std::vector<bool>> is_on_path(grid.getRows(), std::vector<bool>(grid.getCols(), false));
    if (!path.empty()) {
        for (const auto& p : path) {
            is_on_path[p.first][p.second] = true;
        }
    }
    
    for (int r = 0; r < grid.getRows(); ++r) {
        for (int c = 0; c < grid.getCols(); ++c) {
            util::setCursorPosition(r + y_offset, c * 2 + x_offset);
            const Node& node = grid.getNode(r, c);
            
            #ifdef _WIN32
            int color = 7; // Default
            if (node.is_wall) color = 8;
            else if (node.visited_fwd && node.visited_bwd) color = 5; // Meeting nodes
            else if (node.visited_fwd) color = 13; // Visited by forward
            else if (node.visited_bwd) color = 9;  // Visited by backward
            
            if (r == start.first && c == start.second) color = 10;
            else if (r == end.first && c == end.second) color = 11;
            else if (is_on_path[r][c]) color = 14;
            else if (processing.first == r && processing.second == c) color = 12;
            
            if (cursor && cursor->first == r && cursor->second == c) color = 15;

            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            SetConsoleTextAttribute(hConsole, color);
            #endif
            
            if (node.is_wall) std::cout << "##";
            else if (r == start.first && c == start.second) std::cout << " S";
            else if (r == end.first && c == end.second) std::cout << " E";
            else if (is_on_path[r][c]) std::cout << " *";
            else std::cout << "  ";
        }
    }
}


void Visualizer::drawUI(const std::vector<std::unique_ptr<IPathfinder>>& pathfinders, const std::vector<AlgoState>& states, const Point* cursor, int speed) {
    util::clearScreen();
    int x_offset = 0;

    for (size_t i = 0; i < pathfinders.size(); ++i) {
        util::setCursorPosition(0, x_offset);
        std::cout << pathfinders[i]->getName();
        if (states[i] == AlgoState::PathFound) std::cout << " - Hoan thanh!";
        if (states[i] == AlgoState::PathNotFound) std::cout << " - That bai!";

        drawGrid(1, x_offset, *pathfinders[i], states[i], cursor);
        x_offset += pathfinders[i]->getGrid().getCols() * 2 + 5;
    }

    int bottom_line = pathfinders.empty() ? 5 : pathfinders[0]->getGrid().getRows() + 2;
    util::setCursorPosition(bottom_line, 0);
    std::cout << "Di chuyen: WASD | Them/Xoa tuong: Space | Tang/Giam toc: +/- | Thoat: Q\n";
    std::cout << "Toc do: " << speed << " buoc/khung hinh\n";
}