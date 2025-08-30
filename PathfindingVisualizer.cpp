#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <limits>
#include <cmath>
#include <random>
#include <algorithm>
#include <stdexcept>

// Chỉ include thư viện dành riêng cho Windows khi cần thiết
#ifdef _WIN32
#include <windows.h>
#endif

// --- Phần Tiện ích (Utilities) ---
namespace util {
    void clearScreen() {
    #ifdef _WIN32
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD coord = {0, 0};
        DWORD count;
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hStdOut, &csbi);
        FillConsoleOutputCharacter(hStdOut, ' ', csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
        SetConsoleCursorPosition(hStdOut, coord);
    #else
        std::cout << "\033[2J\033[1;1H";
    #endif
    }

    // *** THAY ĐỔI 1: Hàm nhập liệu an toàn ***
    int getSafeIntegerInput() {
        int choice;
        while (!(std::cin >> choice)) {
            std::cout << "Nhap lieu khong hop le. Vui long nhap mot so: ";
            std::cin.clear(); // Xóa trạng thái lỗi của cin
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Loại bỏ input sai
        }
        return choice;
    }
}

// --- Phần Cấu trúc Dữ liệu (Data Structures) ---

// *** THAY ĐỔI 2: Dùng struct Point thay cho std::pair ***
struct Point {
    int r, c;

    // Toán tử so sánh bằng (đã có)
    bool operator==(const Point& other) const {
        return r == other.r && c == other.c;
    }

    // Toán tử so sánh khác (đã có)
     bool operator!=(const Point& other) const {
        return !(*this == other);
    }
    
    // *** THÊM TOÁN TỬ NÀY VÀO ***
    // Cần thiết cho priority_queue để phá vỡ thế hòa (tie-break)
    bool operator<(const Point& other) const {
        if (r != other.r) {
            return r < other.r;
        }
        return c < other.c;
    }
};

struct Node {
    bool is_wall = false;
    bool visited = false;
    Point parent = {-1, -1};
    int g_score = std::numeric_limits<int>::max();
    int weight = 1;
};

// --- Lớp Grid: Chỉ quản lý dữ liệu mê cung ---
class Grid {
private:
    int rows;
    int cols;
    std::vector<std::vector<Node>> grid_data;
    Point start_pos;
    Point end_pos;

    void carvePath(int r, int c, std::mt19937& rng) {
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

public:
    explicit Grid(int r, int c) : rows(r), cols(c) {
        if (r <= 0 || c <= 0) {
            throw std::invalid_argument("Rows and cols must be positive.");
        }
        grid_data.resize(rows, std::vector<Node>(cols));
    }
    
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    Point getStart() const { return start_pos; }
    Point getEnd() const { return end_pos; }
    const Node& getNode(int r, int c) const { return grid_data[r][c]; }
    Node& getNode(int r, int c) { return grid_data[r][c]; }
    
    bool isValid(int r, int c) const { return r >= 0 && r < rows && c >= 0 && c < cols; }

    void createMazeFromLayout(const std::vector<std::string>& layout) {
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (r >= layout.size() || c >= layout[r].size()) continue;
                switch (layout[r][c]) {
                    case '#': grid_data[r][c].is_wall = true; break;
                    case 'S': start_pos = {r, c}; break;
                    case 'E': end_pos = {r, c}; break;
                    case '~': grid_data[r][c].weight = 5; break;
                    default: break;
                }
            }
        }
    }

    void generateMaze() {
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
    
    void reset() {
        for (auto& row : grid_data) {
            for (auto& node : row) {
                node.g_score = std::numeric_limits<int>::max();
                node.visited = false;
                node.parent = {-1, -1};
            }
        }
    }
};

// --- Lớp Visualizer: Chỉ chịu trách nhiệm hiển thị ---
#ifdef _WIN32
// *** THAY ĐỔI 3: Loại bỏ Magic Numbers cho màu sắc ***
namespace ConsoleColor {
    const int DEFAULT = 7;
    const int WALL = 8;
    const int START = 10;       // Green
    const int END = 11;         // Cyan
    const int PATH = 12;        // Red
    const int VISITED = 13;     // Magenta
    const int PROCESSING = 14;  // Yellow
    const int WEIGHTED = 6;     // Brown/Dark Yellow
}
#endif

class Visualizer {
private:
#ifdef _WIN32
    HANDLE hConsole;
#endif
public:
    Visualizer() {
    #ifdef _WIN32
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    #endif
    }
    
    void printState(const Grid& grid, const Point* current_processing = nullptr, const std::vector<Point>* path = nullptr) {
        if (current_processing != nullptr) util::clearScreen();

        std::vector<std::vector<bool>> is_on_path(grid.getRows(), std::vector<bool>(grid.getCols(), false));
        if (path) {
            for (const auto& p : *path) {
                is_on_path[p.r][p.c] = true;
            }
        }

        Point start = grid.getStart();
        Point end = grid.getEnd();

        for (int r = 0; r < grid.getRows(); ++r) {
            for (int c = 0; c < grid.getCols(); ++c) {
                const Node& node = grid.getNode(r, c);
                
                #ifdef _WIN32
                // Sử dụng hằng số màu đã định danh
                int color = ConsoleColor::DEFAULT;
                if (r == start.r && c == start.c) color = ConsoleColor::START;
                else if (r == end.r && c == end.c) color = ConsoleColor::END;
                else if (is_on_path[r][c]) color = ConsoleColor::PATH;
                else if (current_processing && r == current_processing->r && c == current_processing->c) color = ConsoleColor::PROCESSING;
                else if (node.is_wall) color = ConsoleColor::WALL;
                else if (node.visited) color = ConsoleColor::VISITED;
                else if (node.weight > 1) color = ConsoleColor::WEIGHTED;
                SetConsoleTextAttribute(hConsole, color);
                #endif

                if (r == start.r && c == start.c) std::cout << "S ";
                else if (r == end.r && c == end.c) std::cout << "E ";
                else if (is_on_path[r][c]) std::cout << "* ";
                else if (current_processing && r == current_processing->r && c == current_processing->c) std::cout << "o ";
                else if (node.is_wall) std::cout << "# ";
                else if (node.visited) std::cout << "+ ";
                else if (node.weight > 1) std::cout << "~ ";
                else std::cout << ". ";
            }
            std::cout << '\n';
        }
        
        #ifdef _WIN32
        SetConsoleTextAttribute(hConsole, ConsoleColor::DEFAULT);
        #endif

        if (current_processing) {
            std::cout << "Dang xet o: (" << current_processing->r << ", " << current_processing->c << ")\n";
        }
    }

    void printFinalPath(const Grid& grid) {
        std::vector<Point> path;
        Point current = grid.getEnd();
        Point start = grid.getStart();
        Point invalid_parent = {-1, -1};
        
        while (current != invalid_parent) {
            path.push_back(current);
            if (current == start) break;
            current = grid.getNode(current.r, current.c).parent;
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        util::clearScreen();
        std::cout << "\n--- DUONG DI TIM THAY ---\n";
        printState(grid, nullptr, &path);
        std::cout << "Tong chi phi (cost): " << grid.getNode(grid.getEnd().r, grid.getEnd().c).g_score << '\n';
    }
};

// --- Phần Thuật toán: Pathfinder ---
namespace Pathfinder {
    using PQElement = std::pair<int, Point>;
    
    int heuristic(const Point& a, const Point& b) {
        return std::abs(a.r - b.r) + std::abs(a.c - b.c);
    }

    bool findPathAStar(Grid& grid, Visualizer& visualizer, bool isDijkstra) {
        grid.reset();
        Point start = grid.getStart();
        Point end = grid.getEnd();
        
        grid.getNode(start.r, start.c).g_score = 0;
        
        std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> pq;
        int h_start = isDijkstra ? 0 : heuristic(start, end);
        pq.push({h_start, start});
        
        const int dr[] = {-1, 1, 0, 0};
        const int dc[] = {0, 0, -1, 1};

        while (!pq.empty()) {
            Point current_pos = pq.top().second;
            pq.pop();

            if (grid.getNode(current_pos.r, current_pos.c).visited) continue;
            
            grid.getNode(current_pos.r, current_pos.c).visited = true;
            visualizer.printState(grid, &current_pos);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
            if (current_pos == end) return true;

            for (int i = 0; i < 4; ++i) {
                Point neighbor_pos = {current_pos.r + dr[i], current_pos.c + dc[i]};
                
                if (grid.isValid(neighbor_pos.r, neighbor_pos.c) && !grid.getNode(neighbor_pos.r, neighbor_pos.c).is_wall) {
                    int weight = grid.getNode(neighbor_pos.r, neighbor_pos.c).weight;
                    int tentative_g_score = grid.getNode(current_pos.r, current_pos.c).g_score + weight;

                    if (tentative_g_score < grid.getNode(neighbor_pos.r, neighbor_pos.c).g_score) {
                        Node& neighbor_node = grid.getNode(neighbor_pos.r, neighbor_pos.c);
                        neighbor_node.parent = current_pos;
                        neighbor_node.g_score = tentative_g_score;
                        int h_score = isDijkstra ? 0 : heuristic(neighbor_pos, end);
                        pq.push({tentative_g_score + h_score, neighbor_pos});
                    }
                }
            }
        }
        return false;
    }

    bool findPathBFS(Grid& grid, Visualizer& visualizer) {
        grid.reset();
        Point start = grid.getStart();
        Point end = grid.getEnd();

        std::queue<Point> q;
        q.push(start);
        grid.getNode(start.r, start.c).visited = true;
        grid.getNode(start.r, start.c).g_score = 0;
        
        const int dr[] = {-1, 1, 0, 0};
        const int dc[] = {0, 0, -1, 1};
        
        while (!q.empty()) {
            Point current_pos = q.front(); q.pop();
            
            visualizer.printState(grid, &current_pos);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
            if (current_pos == end) return true;

            for (int i = 0; i < 4; ++i) {
                int next_r = current_pos.r + dr[i];
                int next_c = current_pos.c + dc[i];
                
                if (grid.isValid(next_r, next_c) && !grid.getNode(next_r, next_c).is_wall && !grid.getNode(next_r, next_c).visited) {
                    Node& neighbor_node = grid.getNode(next_r, next_c);
                    neighbor_node.visited = true;
                    neighbor_node.parent = current_pos;
                    neighbor_node.g_score = grid.getNode(current_pos.r, current_pos.c).g_score + 1;
                    q.push({next_r, next_c});
                }
            }
        }
        return false;
    }
}

// --- Hàm main() ---
int main() {
    util::clearScreen();
    std::cout << "--- PATHFINDING VISUALIZER ---\n";
    std::cout << "1. Load maze tu file\n";
    std::cout << "2. Tao maze ngau nhien\n";
    std::cout << "Lua chon cua ban: ";
    int main_choice = util::getSafeIntegerInput(); // Sử dụng hàm nhập an toàn

    int ROWS, COLS;
    std::vector<std::string> maze_layout;
    
    if (main_choice == 1) {
        std::cout << "\nChon file maze:\n";
        std::cout << "1. Maze co ban (maze.txt)\n";
        std::cout << "2. Maze co trong so (maze_weighted.txt)\n";
        std::cout << "Lua chon: ";
        int file_choice = util::getSafeIntegerInput(); // Sử dụng hàm nhập an toàn
        std::string filename = (file_choice == 2) ? "maze_weighted.txt" : "maze.txt";
        
        std::ifstream maze_file(filename);
        if (!maze_file) {
            std::cerr << "Loi: Khong mo duoc file " << filename << '\n';
            return 1;
        }
        std::string line;
        while (getline(maze_file, line)) maze_layout.push_back(line);
        if (maze_layout.empty()) {
            std::cerr << "Loi: File maze trong.\n";
            return 1;
        }
        ROWS = maze_layout.size();
        COLS = maze_layout[0].size();
    } else if (main_choice == 2) {
        ROWS = 21; COLS = 41;
    } else {
        std::cout << "Lua chon khong hop le.\n";
        return 1;
    }
    
    Grid myGrid(ROWS, COLS);
    if (main_choice == 1) myGrid.createMazeFromLayout(maze_layout);
    else myGrid.generateMaze();
    
    Visualizer visualizer;
    util::clearScreen();
    std::cout << "--- MAZE DA SAN SANG ---\n";
    visualizer.printState(myGrid);

    std::cout << "\nChon thuat toan:\n";
    std::cout << "1. BFS (Khong tinh trong so)\n";
    std::cout << "2. Dijkstra's Algorithm (Tinh trong so)\n";
    std::cout << "3. A* Search (Tinh trong so, hieu qua)\n";
    std::cout << "Lua chon cua ban: ";
    int algo_choice = util::getSafeIntegerInput(); // Sử dụng hàm nhập an toàn

    bool path_found = false;
    switch(algo_choice) {
        case 1: path_found = Pathfinder::findPathBFS(myGrid, visualizer); break;
        case 2: path_found = Pathfinder::findPathAStar(myGrid, visualizer, true); break;
        case 3: path_found = Pathfinder::findPathAStar(myGrid, visualizer, false); break;
        default: std::cout << "Lua chon khong hop le.\n"; return 1;
    }
    
    if (path_found) {
        visualizer.printFinalPath(myGrid);
    } else {
        std::cout << "Khong tim thay duong di!\n";
    }

    std::cout << "\n------------------------------\n";
    return 0;
}