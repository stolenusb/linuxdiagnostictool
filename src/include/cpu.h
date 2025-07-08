#pragma once

#include <atomic>

struct CpuData {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;

    unsigned long long Total() {
        return user + nice + system + idle + iowait + irq + softirq + steal;
    }

    unsigned long long Idle() {
        return idle + iowait;
    }
};

class CpuInfo {
private:
    std::atomic<float> usage{0.0f};
    std::thread updater;
    bool running;

    struct CpuData readCpuStats();
    
public:
    CpuInfo();
    ~CpuInfo();

    float getCpuUsage();
};