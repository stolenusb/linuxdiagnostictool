#pragma once
#include <ncurses.h>

struct MemData {
    unsigned long long memusage;
    unsigned long long memfree;
    unsigned long long membuffers;
    unsigned long long memcached;
    unsigned long long memswapusage;
};

class MemInfo {
public:
    MemInfo();

    MemData readMemData();
    void displayInfo(WINDOW* win);
private:
    void printInGb(WINDOW* win, int pos_x, const char* memstr, unsigned long long mem, bool usage = false, const char* memusagestr = nullptr) const;

    unsigned long long totalMemNum;
    std::string totalMemStr;

    unsigned long long totalMemSwapNum;
    std::string totalMemSwapStr;
};