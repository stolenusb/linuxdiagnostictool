#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <ncurses.h>

struct ProcData {
    unsigned int pid;
    std::string pname;
    double cpu_curr;
    double cpu_prev;
    std::string mem;
};

class ProcInfo {
public:
    ProcInfo();
    ~ProcInfo();
    void displayInfo(WINDOW* win) const;

private:
    void getProcUsage();
    std::vector<ProcData> readProcData();
    unsigned long long getTotalCpuTime();

    std::vector<std::string> procs_cpu_usage;
    std::unordered_map<int, unsigned long long> prev_cpu_map;
    std::thread updater;
    bool running;
    mutable std::mutex proc_mutex;
    long page_size_kb;
};