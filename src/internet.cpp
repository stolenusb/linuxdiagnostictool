#include <sstream>
#include <fstream>
#include <vector>
#include <thread>
#include <internet.h>

NetInfo::NetInfo()
{
    wan_if = "";

    std::ifstream File("/proc/net/route");
    std::string Line;

    while(std::getline(File, Line)) {
        std::istringstream iss(Line);

        std::vector<std::string> cols;
        std::string value;

        while(iss >> value)
            cols.push_back(value);
        
        if(cols[1] == "00000000")
            wan_if = cols[0];
    }

    running = true;

    updater = std::thread([this]() {
        while(running) {
            this->getNetUsage();
        }
    });
}

NetInfo::~NetInfo()
{
    running = false;
    
    if(updater.joinable())
        updater.join();
}

void NetInfo::getNetUsage()
{
    NetData data_1 = readNetData();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    NetData data_2 = readNetData();
    
    received = static_cast<double>(data_2.received - data_1.received) / (1024 * 1024);
    transmitted = static_cast<double>(data_2.transmitted - data_1.transmitted) / (1024 * 1024);
}

NetData NetInfo::readNetData()
{
    NetData data;
    data.received = 0;
    data.transmitted = 0;

    std::ifstream File("/proc/net/dev");
    std::string Line;
    
    while(std::getline(File, Line)) {
        if(Line.find(wan_if.c_str()) != std::string::npos) {
            std::string interface;
            std::istringstream iss(Line);
            iss >> Line;
            
            std::vector<unsigned long long> values;
            unsigned long long value;
            
            while(iss >> value)
                values.push_back(value);
            
            data.received = values[0];
            data.transmitted = values[8];

            break;
        }
    }

    return data;
}

void NetInfo::displayInfo(WINDOW* win)
{
    wclear(win);
    box(win, 0, 0);
    
    mvwprintw(win, 0, 0, "Net Usage: %s", wan_if.c_str());
    mvwprintw(win, 1, 2, "In: %.3f MB/s", received);
    mvwprintw(win, 2, 2, "Out: %.3f MB/s", transmitted);

    wrefresh(win);
}