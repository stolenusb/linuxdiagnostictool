#pragma once

#include <atomic>
#include <unordered_map>

struct CpuData {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;

    unsigned long long Total() {
        return user + nice + system + idle + iowait + irq + softirq + steal;
    }

    unsigned long long Idle() {
        return idle + iowait;
    }
};

using CpuCoreData = std::unordered_map<std::string, CpuData>;

class CpuInfo {
private:
    std::atomic<float> usage{0.0f};
    std::unordered_map<std::string, float> coreUsage;
    std::thread updater;
    bool running;

    CpuCoreData readCpuStats();
    
public:
    CpuInfo();
    ~CpuInfo();

    std::string getCpuUsage();
};