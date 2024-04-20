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

    double x = 0.;
private:
    double amplitude, signslOffset;
    std::string signalType;
};