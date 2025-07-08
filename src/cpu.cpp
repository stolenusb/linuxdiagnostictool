#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <cpu.h>

struct CpuData CpuInfo::readCpuStats()
{
    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line);

    std::istringstream iss(line);

    std::string cpu;
    struct CpuData data;
    
    iss >> cpu >> data.user >> data.nice >> data.system >> data.idle >> data.iowait >> data.irq >> data.softirq >> data.steal;

    return data;
}

CpuInfo::CpuInfo()
{
    running = true;

    updater = std::thread([this]() {
        CpuData prev = readCpuStats();
        while(running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CpuData curr = readCpuStats();
            auto total_diff = curr.Total() - prev.Total();
            auto idle_diff = curr.Idle() - prev.Idle();
            prev = curr;

            if(total_diff == 0)
                usage = 0.0f;
            else
                usage = 100.0f * (total_diff - idle_diff) / total_diff;
        }
    });
}

CpuInfo::~CpuInfo()
{
    running = false;

    if(updater.joinable())
        updater.join();
}

float CpuInfo::getCpuUsage()
{
    return usage.load();
}