#include "signalGenerater/signalGenerater.hpp"

signalGenerater::signalGenerater(std::string configPath)
{
    boost::filesystem::path p(configPath);
    std::ifstream ifs(p);
    std::string jsonstr((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
    boost::json::value jv = boost::json::parse(jsonstr);
    boost::json::object jo = jv.as_object();

    amplitude = jo["signal"].as_object().at("amplitude").as_double();
    signslOffset = jo["signal"].as_object().at("offset").as_double();
    signalType = jo["signal"].as_object().at("type").as_string();
    signal_limit = jo["signal"].as_object().at("remote_signal_limit").as_double();
    signal_limit /= 660.; // 除以遥感的最大值
    if_reverse = jo["signal"].as_object().at("remote_direction_reversal").as_bool();
}

double signalGenerater::generateSignal(double crr, int sign)
{
    double res;
    if (signalType == "sin") { // 模拟正弦信号
        res = signslOffset + amplitude * sin(x + M_PI / 2);
        x += 0.01;
        if (x >= 2 * M_PI)
            x = x - 2 * M_PI;
    } else if (signalType == "step") { // 模拟阶跃信号
        res = signslOffset + amplitude;
    } else if (signalType == "remote") { //* 遥控器控制，实际上场使用
        // 这里使用固定的增量，即匀速运动
        // TODO: 变为匀加速运动？
        sign = if_reverse ? -sign : sign;
        res = crr + signal_limit * sign * 0.001;
    } else if (signalType == "none" ||   // 不设置信号，用于调试，找物理限位
               signalType == "stayed") { // 或者保持不动，用于调 pid
        return crr;
    } else {
        std::cerr << "Unknown signal type: " << signalType << std::endl;
        exit(1);
    }
    return res;
}