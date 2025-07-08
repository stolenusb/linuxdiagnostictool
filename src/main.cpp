#include <iostream>
#include <thread>
#include <ncurses.h>
#include <cpu.h>

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

    while(1) {
        if(getch() == ESC_BTN)
            break;
        
        clear();

        printw("%s", Cpu.getCpuUsage());
        
        refresh();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    endwin();
    
    return 0;
}