#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <iomanip>
#include <cpu.h>

CpuInfo::CpuInfo()
{
    running = true;

    updater = std::thread([this]() {
        CpuCoreData prev = readCpuStats();
        while(running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CpuCoreData curr = readCpuStats();

            if(curr.find("cpu") != curr.end() && prev.find("cpu") != prev.end()) {
                auto total_diff = curr["cpu"].Total() - prev["cpu"].Total();
                auto idle_diff = curr["cpu"].Idle() - prev["cpu"].Idle();

                if(total_diff == 0)
                    usage = 0.0f;
                else
                    usage = 100.0f * (total_diff - idle_diff) / total_diff;
            }
            
            for(const auto& [core, cdata] : curr) {
                if(core == "cpu")
                    continue;
                
                if(prev.find(core) != prev.end()) {
                    auto total_diff = curr[core].Total() - prev[core].Total();
                    auto idle_diff = curr[core].Idle() - prev[core].Idle();
                    if(total_diff == 0)
                        coreUsage[core] = 0.0f;
                    else
                        coreUsage[core] = 100.0f * (total_diff - idle_diff) / total_diff;
                }
            }

            prev = curr;
        }
    });
}

CpuInfo::~CpuInfo()
{
    running = false;

    if(updater.joinable())
        updater.join();
}

void CpuInfo::displayInfo(WINDOW *win)
{
    std::string usage = getCpuUsage();
    std::istringstream iss(usage);
    
    std::string lineText;
    std::getline(iss, lineText);
    std::string cpuTitle = "CPU Usage: " + lineText.substr(lineText.find(" ") + 1) + "%";
    
    wclear(win);
    box(win, 0, 0);
    mvwprintw(win, 0, 1, "%s", cpuTitle.c_str());
    
    int line = 1;
    while(std::getline(iss, lineText)) {
        mvwprintw(win, line++, 2, "%s", lineText.c_str());
    }

    wrefresh(win);
}

CpuCoreData CpuInfo::readCpuStats()
{
    CpuCoreData dataMap;
    std::ifstream file("/proc/stat");
    std::string line;

    while(std::getline(file, line)) {
        if(line.compare(0, 3, "cpu") == 0) {
            std::istringstream iss(line);
            std::string cpuLabel;
            struct CpuData data;
            
            iss >> cpuLabel >> data.user >> data.nice >> data.system >> data.idle >> data.iowait >> data.irq >> data.softirq >> data.steal;

            dataMap[cpuLabel] = data;
        }
    }

    return dataMap;
}

std::string CpuInfo::getCpuUsage()
{
    
    std::ostringstream oss;

    oss << "global" << " " << std::fixed << std::setprecision(2) << usage << "\n";

    for(auto const& [core, use] : coreUsage)
        oss << core << ": " << std::fixed << std::setprecision(2) << use << "%\n";

    return oss.str();
}