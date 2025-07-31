#include <fstream>
#include <sstream>
#include <thread>
#include <iomanip>
#include <mem.h>

MemInfo::MemInfo()
{
    std::ifstream File("/proc/meminfo");
    std::string Line;

    while(std::getline(File, Line)) {
        if(Line.rfind("MemTotal:") == 0) {
            std::istringstream iss(Line);
            std::string label, unit;

            iss >> label >> totalMemNum >> unit;

            double totalMemGb = static_cast<double>(totalMemNum) / (1024 * 1024);
            std::ostringstream oss;

            oss << std::fixed << std::setprecision(2) << totalMemGb << " GB";
            totalMemStr = oss.str();
        } else if(Line.rfind("SwapTotal:") == 0) {
            std::istringstream iss(Line);
            std::string label, unit;

            iss >> label >> totalMemSwapNum >> unit;

            double totalMemSwapGb = static_cast<double>(totalMemSwapNum) / (1024 * 1024);
            std::ostringstream oss;

            oss << std::fixed << std::setprecision(2) << totalMemSwapGb << " GB";
            totalMemSwapStr = oss.str();

            break;
        }
    }
}

void MemInfo::displayInfo(WINDOW *win)
{
    MemData memdata = readMemData();
    
    wclear(win);
    box(win, 0, 0);

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << 100.0 * memdata.memusage / totalMemNum << "%";
    mvwprintw(win, 0, 1, "Mem Usage: %s", oss.str().c_str());
    
    printInGb(win, 1, "Used:", memdata.memusage, true, totalMemStr.c_str());
    printInGb(win, 3, "Free:", memdata.memfree);
    printInGb(win, 4, "Buffers:", memdata.membuffers);
    printInGb(win, 5, "Cached:", memdata.memcached);
    printInGb(win, 6, "Swap:", memdata.memswapusage, true, totalMemSwapStr.c_str());

    wrefresh(win);
}

MemData MemInfo::readMemData()
{
    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    MemData memdata;
    std::ifstream File("/proc/meminfo");
    std::string Line;

    while(std::getline(File, Line)) {
        if(Line.rfind("MemFree:") == 0) {
            std::istringstream iss(Line);
            std::string label, unit;
            unsigned long long memfree;

            iss >> label >> memfree >> unit;

            memdata.memfree = memfree;
        } else if(Line.rfind("MemAvailable:") == 0) {
            std::istringstream iss(Line);
            std::string label, unit;
            unsigned long long memavailable;

            iss >> label >> memavailable >> unit;

            memdata.memusage = totalMemNum - memavailable;
        } else if(Line.rfind("Buffers:") == 0) {
            std::istringstream iss(Line);
            std::string label, unit;
            unsigned long long membuffers;

            iss >> label >> membuffers >> unit;

            memdata.membuffers = membuffers;
        } else if(Line.rfind("Cached:") == 0) {
            std::istringstream iss(Line);
            std::string label, unit;
            unsigned long long cached;

            iss >> label >> cached >> unit;

            memdata.memcached = cached;
        } else if(Line.rfind("SwapFree:") == 0) {
            std::istringstream iss(Line);
            std::string label, unit;
            unsigned long long memswapfree;

            iss >> label >> memswapfree >> unit;

            memdata.memswapusage = totalMemSwapNum - memswapfree;

            break;
        }
    }

    return memdata;
}

void MemInfo::printInGb(WINDOW* win, const int pos_x, const char* memstr, const unsigned long long mem, bool usage, const char* memusagestr) const
{
    std::ostringstream oss;
    double memcast = static_cast<double>(mem) / (1024 * 1024);
    oss << std::fixed << std::setprecision(2) << memcast << " GB";

    if(usage)
        mvwprintw(win, pos_x, 2, "%s %s / %s", memstr, oss.str().c_str(), memusagestr);
    else
        mvwprintw(win, pos_x, 2, "%s %s", memstr, oss.str().c_str());
}