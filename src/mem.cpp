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
        if(Line.compare(0, 9, "MemTotal:") == 0) {
            std::istringstream iss(Line);
            std::string label, unit;

            iss >> label >> totalMemNum >> unit;

            double totalMemGb = static_cast<double>(totalMemNum) / (1024 * 1024);
            std::ostringstream oss;

            oss << std::fixed << std::setprecision(2) << totalMemGb << " GB";
            totalMemStr = oss.str();

            break;
        }
    }
}

void MemInfo::displayInfo(WINDOW *win, int width)
{
    unsigned long long memUsage = readMemUsage();
    double memUsageNum = static_cast<double>(memUsage) / (1024 * 1024);
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << 100.0 * memUsage / totalMemNum << "%";
    std::string memTitle = "Mem Usage: " + oss.str();

    std::ostringstream oss2;
    oss2 << std::fixed << std::setprecision(2) << memUsageNum << " GB";
    
    wclear(win);
    box(win, 0, 0);
    
    mvwprintw(win, 0, std::max(1, (int)(width - memTitle.size()) / 2), "%s", memTitle.c_str());
    mvwprintw(win, 1, 2, "%s / %s", oss2.str().c_str(), totalMemStr.c_str());
    
    wrefresh(win);
}

unsigned long long MemInfo::readMemUsage()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::ifstream File("/proc/meminfo");
    std::string Line;

    while(std::getline(File, Line)) {
        if(Line.compare(0, 13, "MemAvailable:") == 0) {
            std::istringstream iss(Line);
            std::string label, unit;
            unsigned long long availableMem;

            iss >> label >> availableMem >> unit;

            return totalMemNum - availableMem;
        }
    }

    return 0;
}