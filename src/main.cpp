#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <ctime>
#include <ncurses.h>
#include <iomanip>
#include <cpu.h>
#include <mem.h>
#include <uptime.h>
#include <internet.h>
#include <processes.h>

#define ESC_BTN 27

std::string getPcInfo(int info);
void displayPcInfo(const std::string& hostname, const std::string& kernelversion, const std::string &booted);

int main()
{
    initscr();
    start_color();
    use_default_colors();
    noecho();
    cbreak();
    nodelay(stdscr, TRUE);
    curs_set(0);

    const std::string hostname = getPcInfo(0);
    const std::string kernelversion = getPcInfo(1);
    const std::string booted = getPcInfo(2);

    CpuInfo Cpu;
    MemInfo Mem;
    UpTime upTime;
    NetInfo Net;
    ProcInfo Procs;
    
    int height = 2 + std::thread::hardware_concurrency();
    int width = 20;
    int startY = 2;
    int startX = 0;

    WINDOW* uptimeWin = newwin(2, width, startY, startX);
    WINDOW* cpuWin = newwin(height, width, startY + 2, startX);
    WINDOW* memWin = newwin(8, width + 7, startY, startX + width);
    WINDOW* netWin = newwin(4, width, startY, startX + width * 2 + 7);
    WINDOW* portsWin = newwin(2, width, startY + 4, startX + width * 2 + 7);
    WINDOW* procsWin = newwin(8, width + 26, startY, startX + width * 3 + 7);

    // PC Info
    displayPcInfo(hostname, kernelversion, booted);

    while(1) {
        if(getch() == ESC_BTN)
            break;

        // CPU
        Cpu.displayInfo(cpuWin);
        
        // RAM
        Mem.displayInfo(memWin);

        // UPTIME
        upTime.displayInfo(uptimeWin);

        // NET
        Net.displayInfo(netWin, portsWin);

        // PROCESSES
        Procs.displayInfo(procsWin);

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    delwin(cpuWin);
    delwin(memWin);
    delwin(uptimeWin);
    delwin(netWin);
    delwin(portsWin);
    delwin(procsWin);
    endwin();
    
    return 0;
}

std::string getPcInfo(int info)
{
    switch(info) {
        case 0: {
            std::ifstream File("/proc/sys/kernel/hostname");
            std::string Line;
            std::getline(File, Line);

            return Line;
        }

        case 1: {
            std::ifstream File("/proc/version");
            std::string Line;
            std::getline(File, Line);

            std::istringstream iss(Line);
            std::vector<std::string> fields;
            std::string field;

            while(iss >> field)
                fields.push_back(field);
            
            return fields.at(2);
        }

        case 2: {
            std::ifstream File("/proc/stat");
            std::string Line;
            while(std::getline(File, Line)) {
                if(Line.rfind("btime") == 0)
                    break;
            }

            std::istringstream iss(Line);
            unsigned long long btime;
            iss >> Line >> btime;
            
            std::time_t boottime = static_cast<time_t>(btime);
            std::tm* timeinfo = std::localtime(&boottime);

            std::ostringstream formatted;
            formatted << std::put_time(timeinfo, "%Y-%m-%d %H:%M");

            return std::string(formatted.str());
        }
    }

    return std::string("");
}

void displayPcInfo(const std::string& hostname, const std::string& kernelversion, const std::string &booted)
{
    init_pair(1, COLOR_GREEN, COLOR_BLACK);

    printw("Host: ");
    attron(COLOR_PAIR(1));
    printw("%s", hostname.c_str());
    attroff(COLOR_PAIR(1));

    printw("      ");

    printw("Kernel: ");
    attron(COLOR_PAIR(1));
    printw("%s", kernelversion.c_str());
    attroff(COLOR_PAIR(1));

    printw("      ");

    printw("Booted: ");
    attron(COLOR_PAIR(1));
    printw("%s", booted.c_str());
    attroff(COLOR_PAIR(1));
}