#include <iostream>
#include <sstream>
#include <thread>
#include <ncurses.h>
#include <iomanip>
#include <cpu.h>
#include <mem.h>
#include <uptime.h>
#include <internet.h>

#define ESC_BTN 27

int main()
{
    std::cout << "[*] Initializing Tool." << std::endl;

    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    curs_set(0);

    CpuInfo Cpu;
    MemInfo Mem;
    UpTime upTime;
    NetInfo Net;
    
    int height = 2 + std::thread::hardware_concurrency();
    int width = 20;
    int startY = 0;
    int startX = 0;

    WINDOW* cpuWin = newwin(height, width, startY, startX);
    WINDOW* memWin = newwin(3, width, startY, startX + width);
    WINDOW* uptimeWin = newwin(2, width, startY, startX + width * 2);
    WINDOW* netWin = newwin(4, width, startY, startX + width * 3);

    while(1) {
        if(getch() == ESC_BTN)
            break;
        
        // CPU
        Cpu.displayInfo(cpuWin, width);
        
        // RAM
        Mem.displayInfo(memWin, width);

        // UPTIME
        upTime.displayInfo(uptimeWin);

        // NET
        Net.displayInfo(netWin);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    delwin(cpuWin);
    delwin(memWin);
    delwin(uptimeWin);
    delwin(netWin);
    endwin();
    
    return 0;
}