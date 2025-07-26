#pragma once
#include <ncurses.h>
#include <string>

struct NetData {
    unsigned long long received;
    unsigned long long transmitted;
};

class NetInfo {
public:
    NetInfo();
    ~NetInfo();

    void displayInfo(WINDOW* win);
    std::string wan_if;
private:
    NetData readNetData();
    void getNetUsage();

    double received;
    double transmitted;
    std::thread updater;
    bool running;
};