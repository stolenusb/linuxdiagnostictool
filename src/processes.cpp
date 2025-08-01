#include <filesystem>
#include <algorithm>
#include <fstream>
#include <thread>
#include <cmath>
#include <processes.h>
#include <unistd.h>

ProcInfo::ProcInfo()
{
    page_size_kb = sysconf(_SC_PAGESIZE) / 1024;

    running = true;
    updater = std::thread([this]() {
        while(running) {
            this->getProcUsage();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    });
}

ProcInfo::~ProcInfo()
{
 running = false;

    if(updater.joinable())
        updater.join();
}

void ProcInfo::displayInfo(WINDOW* win) const
{
    wclear(win);
    box(win, 0, 0);

    mvwprintw(win, 0, 1, "Top 5 Processes");
    std::ostringstream oss;
    oss.precision(2);
    oss << std::fixed;
    oss << std::setw(15) << std::left << "Process"
    << std::setw(8) << "PID"
    << std::setw(8) << "CPU"
    << std::setw(8) << "MEM";

    mvwprintw(win, 1, 2, "%s", oss.str().c_str());

    std::lock_guard<std::mutex> lock(proc_mutex);
    for(size_t i = 0; i < procs_cpu_usage.size(); i++)
        mvwprintw(win, 2 + i, 2, "%s", procs_cpu_usage.at(i).c_str());

    wrefresh(win);
}

void ProcInfo::getProcUsage()
{
    procs_cpu_usage.clear();

    static unsigned long long cputime_prev = 0;
    unsigned long long cputime_curr = getTotalCpuTime();
    unsigned long long cputime_delta = (cputime_prev == 0) ? 1 : (cputime_curr - cputime_prev);
    cputime_prev = cputime_curr;

    std::vector<ProcData> procdatas = readProcData();
    std::sort(procdatas.begin(), procdatas.end(), [](const ProcData& a, const ProcData& b) {
        return (a.cpu_curr - a.cpu_prev) > (b.cpu_curr - b.cpu_prev);
    });

    std::vector<std::string> local_usage;
    for(int i = 0; i < std::min(5, (int)procdatas.size()); i++) {
        const auto& procdata = procdatas.at(i);

        unsigned long long proc_delta = procdata.cpu_curr - procdata.cpu_prev;
        double cpu_percent = 0.0;
        if(cputime_delta > 0 && procdata.cpu_curr >= procdata.cpu_prev) {
            cpu_percent = 100.0 * static_cast<double>(proc_delta) / cputime_delta;
            if(std::isnan(cpu_percent) || std::isinf(cpu_percent))
                cpu_percent = 0.0;
        }
        
        std::ostringstream oss;
        oss.precision(2);
        oss << std::fixed << cpu_percent << "%";

        std::ostringstream oss2;
        oss2.precision(2);
        oss2 << std::fixed;
        oss2 << std::setw(15) << std::left << procdata.pname
        << std::setw(8) << procdata.pid
        << std::setw(8) << oss.str()
        << std::setw(8) << procdata.mem;

        local_usage.push_back(oss2.str());
    }

    std::lock_guard<std::mutex> lock(proc_mutex);
    procs_cpu_usage = std::move(local_usage);   
}

std::vector<ProcData> ProcInfo::readProcData()
{
    std::vector<ProcData> procdatas;

    for(const auto& entry : std::filesystem::directory_iterator("/proc")) {
        std::string pid_str = entry.path().filename().string();

        if(std::all_of(pid_str.begin(), pid_str.end(), ::isdigit)) {
            ProcData procdata;
            std::ifstream File(std::string("/proc/" + pid_str + "/stat"));
            std::string ignore, proc_name;
            unsigned long long utime, stime;
            
            File >> ignore >> proc_name;
            for(int i = 0; i < 11; i++)
                File >> ignore;
            
            File >> utime >> stime;

            unsigned long long curr_cpu = utime + stime;
            unsigned long long prev_cpu = 0;
            unsigned int pid = std::stoi(pid_str);

            if(prev_cpu_map.find(pid) != prev_cpu_map.end())
                prev_cpu = prev_cpu_map[pid];
            
            prev_cpu_map[pid] = curr_cpu;
            procdata.pname = (proc_name.length() > 2) ? proc_name.substr(1, proc_name.length() - 2) : proc_name;
            procdata.cpu_curr = curr_cpu;
            procdata.cpu_prev = prev_cpu;
            procdata.pid = pid;

            // READING RSS DATA
            for(int i = 0; i < 8; ++i)
                File >> ignore;

            unsigned long rss_pages;
            File >> rss_pages;
            unsigned long mem_kb = rss_pages * page_size_kb;
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2);

            if (mem_kb >= (1024.0 * 1024.0))
                oss << (mem_kb / (1024.0 * 1024.0)) << " GB";
            else if (mem_kb >= 1024.0)
                oss << (mem_kb / 1024.0) << " MB";
            else
                oss << mem_kb << " KB";

            procdata.mem = oss.str();

            procdatas.push_back(procdata);
        }
    }

    return procdatas;
}

unsigned long long ProcInfo::getTotalCpuTime()
{
    std::ifstream File("/proc/stat");
    std::string cpu;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;

    File >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

    return user + nice + system + idle + iowait + irq + softirq + steal;
}