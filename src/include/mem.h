#pragma once

class MemInfo {
public:
    MemInfo();

    unsigned long long readMemUsage();
    unsigned long long totalMemNum;
    std::string totalMemStr;
};