#pragma once
#include <ncurses.h>
#include <string>
#include <vector>

struct NetData {
    unsigned long long received;
    unsigned long long transmitted;
};

struct PortData {
    int port;
    bool tcp;
};

class NetInfo {
public:
    NetInfo();
    ~NetInfo();

    void displayInfo(WINDOW* win, WINDOW* win_ports);
    std::string wan_if;
    
private:
    NetData readNetData();
    void getNetUsage();
    std::vector<PortData> getOpenPorts();

    double received;
    double transmitted;
    std::thread updater;
    bool running;
};