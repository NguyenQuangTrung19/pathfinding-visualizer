#include "Util.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

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

    void setCursorPosition(int y, int x) {
    #ifdef _WIN32
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        COORD coord = {(SHORT)x, (SHORT)y};
        SetConsoleCursorPosition(hStdOut, coord);
    #else
        // ANSI escape code for other platforms
        std::cout << "\033[" << y + 1 << ";" << x + 1 << "H";
    #endif
    }
    
    void hideCursor() {
    #ifdef _WIN32
        HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hStdOut, &cursorInfo);
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(hStdOut, &cursorInfo);
    #else
        std::cout << "\e[?25l";
    #endif
    }
}