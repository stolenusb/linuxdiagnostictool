#include <iostream>
#include <sstream>
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
    
    int height = 2 + std::thread::hardware_concurrency();
    int width = 20;
    int startY = 0;
    int startX = 0;
    WINDOW* cpuWin = newwin(height, width, startY, startX);

    while(1) {
        if(getch() == ESC_BTN)
            break;
        
        wclear(cpuWin);

        std::string usage = Cpu.getCpuUsage();
        std::istringstream iss(usage);
        std::string lineText;
        
        std::getline(iss, lineText);
        std::string globalUsage = lineText.substr(lineText.find(" ") + 1);
        std::string title = "CPU Usage: " + globalUsage + "%";
        
        box(cpuWin, 0, 0);
        mvwprintw(cpuWin, 0, (width - title.size()) / 2, "%s", title.c_str());
        
        int line = 1;
        while(std::getline(iss, lineText)) {
            mvwprintw(cpuWin, line++, 2, "%s", lineText.c_str());
        }

        wrefresh(cpuWin);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    delwin(cpuWin);
    endwin();
    
    return 0;
}