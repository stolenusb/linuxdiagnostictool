#pragma once
#include <ncurses.h>

class MemInfo {
public:
    MemInfo();

    unsigned long long readMemUsage();
    unsigned long long totalMemNum;
    std::string totalMemStr;
    void displayInfo(WINDOW* win, int width);
};