#include <sstream>
#include <fstream>
#include <uptime.h>

void UpTime::displayInfo(WINDOW* win)
{
    wclear(win);
    box(win, 0, 0);
    
    unsigned long total_seconds = static_cast<unsigned long>(getUpTime());
    unsigned int days = total_seconds / (24 * 3600);
    unsigned int hours = total_seconds / (24 * 60);
    unsigned int minutes = total_seconds / 60;
    unsigned int seconds = total_seconds % 60;

    mvwprintw(win, 0, 0, "UpTime: %u:%02u:%02u:%02u", days, hours, minutes, seconds);
    
    wrefresh(win);
}

double UpTime::getUpTime()
{
    std::ifstream File("/proc/uptime");
    std::string Line;
    
    std::getline(File, Line);
    std::istringstream iss(Line);

    double uptime, idle;

    iss >> uptime >> idle;

    return uptime;
}