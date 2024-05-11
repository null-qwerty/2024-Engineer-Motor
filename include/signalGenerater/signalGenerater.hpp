#pragma once

#include <fstream>
#include <iostream>

#include <boost/json.hpp>
#include <boost/filesystem.hpp> 

class signalGenerater {
public:
    signalGenerater(std::string configPath);
    ~signalGenerater() = default;
    double generateSignal(double crr, int sign);
    std::string getSignalType() { return signalType; }

    double x = 0.;
private:
    double amplitude, signslOffset;
    double signal_limit;
    bool if_reverse;
    std::string signalType;
};