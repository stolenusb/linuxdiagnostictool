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
        std::string usage = Cpu.getCpuUsage();
        std::istringstream iss(usage);
        std::string lineText;
        std::getline(iss, lineText);
        std::string cpuTitle = "CPU Usage: " + lineText.substr(lineText.find(" ") + 1) + "%";
        wclear(cpuWin);
        box(cpuWin, 0, 0);
        mvwprintw(cpuWin, 0, (width - cpuTitle.size()) / 2, "%s", cpuTitle.c_str());
        
        int line = 1;
        while(std::getline(iss, lineText)) {
            mvwprintw(cpuWin, line++, 2, "%s", lineText.c_str());
        }

        wrefresh(cpuWin);
        
        // RAM
        unsigned long long memUsage = Mem.readMemUsage();
        double memUsageNum = static_cast<double>(memUsage) / (1024 * 1024);
        
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << 100.0 * memUsage / Mem.totalMemNum << "%";
        std::string memTitle = "Mem Usage: " + oss.str();

        std::ostringstream oss2;
        oss2 << std::fixed << std::setprecision(2) << memUsageNum << " GB";
        
        wclear(memWin);
        box(memWin, 0, 0);
        mvwprintw(memWin, 0, std::max(1, (int)(width - memTitle.size()) / 2), "%s", memTitle.c_str());
        mvwprintw(memWin, 1, 2, "%s / %s", oss2.str().c_str(), Mem.totalMemStr.c_str());
        wrefresh(memWin);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    delwin(cpuWin);
    delwin(memWin);
    endwin();
    
    return 0;
}