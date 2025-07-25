#pragma once
#include <ncurses.h>

class UpTime {
public:
    void displayInfo(WINDOW* win);
private:
    double getUpTime();
};