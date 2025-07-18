#include <iostream>
#include <sstream>
#include <thread>
#include <ncurses.h>
#include <iomanip>
#include <cpu.h>
#include <mem.h>

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
    
    int height = 2 + std::thread::hardware_concurrency();
    int width = 20;
    int startY = 0;
    int startX = 0;

    WINDOW* cpuWin = newwin(height, width, startY, startX);
    WINDOW* memWin = newwin(4, width, startY, startX + width);

    while(1) {
        if(getch() == ESC_BTN)
            break;
        
        // CPU
        Cpu.displayInfo(cpuWin, width);
        
        // RAM
        Mem.displayInfo(memWin, width);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    delwin(cpuWin);
    delwin(memWin);
    endwin();
    
    return 0;
}