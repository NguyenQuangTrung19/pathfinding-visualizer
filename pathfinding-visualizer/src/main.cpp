#include "Grid.h"
#include "Visualizer.h"
#include "AStarPathfinder.h"
#include "BidirectionalPathfinder.h"
#include "Util.h"

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <conio.h>
#endif


void runPathfinder() {
    Grid myGrid(21, 31);
    myGrid.generateMaze();

    Visualizer visualizer;
    
    std::vector<std::unique_ptr<IPathfinder>> pathfinders;
    pathfinders.push_back(std::make_unique<AStarPathfinder>(false)); // A*
    pathfinders.push_back(std::make_unique<AStarPathfinder>(true));  // Dijkstra
    pathfinders.push_back(std::make_unique<BidirectionalPathfinder>());

    std::vector<Grid> grids(pathfinders.size(), myGrid);
    std::vector<AlgoState> states(pathfinders.size(), AlgoState::Running);

    for(size_t i = 0; i < pathfinders.size(); ++i) {
        pathfinders[i]->init(grids[i]);
    }

    bool running = true;
    Point cursor = {1, 1};
    int steps_per_frame = 2;

    while (running) {
        // 1. User Input
        if (_kbhit()) {
            char key = _getch();
            switch(key) {
                case 'w': if (cursor.first > 0) cursor.first--; break;
                case 's': if (cursor.first < myGrid.getRows() - 1) cursor.first++; break;
                case 'a': if (cursor.second > 0) cursor.second--; break;
                case 'd': if (cursor.second < myGrid.getCols() - 1) cursor.second++; break;
                case ' ':
                    for (size_t i = 0; i < grids.size(); ++i) {
                        grids[i].toggleWall(cursor);
                        pathfinders[i]->onWallChanged(cursor);
                        states[i] = AlgoState::Running;
                    }
                    break;
                case 'q': running = false; break;
                case '+': steps_per_frame++; break;
                case '-': if (steps_per_frame > 1) steps_per_frame--; break;
            }
        }

        // 2. Update State
        for (int step = 0; step < steps_per_frame; ++step) {
             for (size_t i = 0; i < pathfinders.size(); ++i) {
                if (states[i] == AlgoState::Running) {
                    states[i] = pathfinders[i]->step();
                }
            }
        }
       
        // 3. Render
        visualizer.drawUI(pathfinders, states, &cursor, steps_per_frame);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

int main() {
    runPathfinder();
    util::setCursorPosition(30, 0); // Move cursor to the end before exit
    return 0;
}