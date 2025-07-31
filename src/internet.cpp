#include <sstream>
#include <fstream>
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

std::vector<PortData> NetInfo::getOpenPorts()
{
    std::vector<PortData> OpenPorts;
    std::string Line;

    {
        std::ifstream tcpFile("/proc/net/tcp");
        while(std::getline(tcpFile, Line)) {
            std::vector<std::string> Fields;
            std::istringstream iss(Line);
            std::string field;

            while(iss >> field)
                Fields.push_back(field);
            
            if(Fields.at(3) == "0A") {
                const int portnum = std::stoi(Fields.at(1).substr(9), nullptr, 16);
                bool exists = false;
                for(const PortData& portdata : OpenPorts) {
                    if(portdata.port == portnum) {
                        exists = true;
                        break;
                    }
                }

                if(exists)
                    continue;
                
                OpenPorts.push_back({.port = portnum, .tcp = true});
            }
        }
    }

    {
        std::ifstream udpFile("/proc/net/udp");
        while(std::getline(udpFile, Line)) {
            std::vector<std::string> Fields;
            std::string field;
            std::istringstream iss(Line);

            while(iss >> field)
                Fields.push_back(field);

            if(Fields.at(3) == "0A") {
                const int portnum = std::stoi(Fields.at(1).substr(9), nullptr, 16);
                bool exists = false;
                for(const PortData& portdata : OpenPorts) {
                    if(portdata.port == portnum) {
                        exists = true;
                        break;
                    }
                }

                if(exists)
                    continue;
                
                OpenPorts.push_back({.port = portnum, .tcp = false});
            }
        }
    }

    return OpenPorts;
}

void NetInfo::displayInfo(WINDOW* win, WINDOW* win_ports)
{
    {
        wclear(win);
        box(win, 0, 0);
        
        mvwprintw(win, 0, 2, "Net Usage: %s", wan_if.c_str());
        mvwprintw(win, 1, 2, "In: %.3f MB/s", received);
        mvwprintw(win, 2, 2, "Out: %.3f MB/s", transmitted);

        wrefresh(win);
    }

    {
        wclear(win_ports);
        box(win_ports, 0, 0);

        std::vector<PortData> OpenPorts = getOpenPorts();
        const size_t ports_count = OpenPorts.size();
        if(ports_count > 0) {
            wresize(win_ports, 2 + ports_count, 20);
            mvwprintw(win_ports, 0, 2, "Open ports:");

            for(size_t i = 0; i < ports_count; i++) {
                const PortData& portdata = OpenPorts.at(i);

                if(portdata.tcp)
                    mvwprintw(win_ports, i + 1, 2, "TCP: %d", portdata.port);
                else
                    mvwprintw(win_ports, i + 1, 2, "UDP: %d", portdata.port);
            }
        }

        wrefresh(win_ports);
    }
}