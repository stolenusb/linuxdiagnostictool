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