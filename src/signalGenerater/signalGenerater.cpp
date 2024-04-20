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
        // 由于下位机发送的数据只有 -1, 0, 1 表示反转，停止，正转
        // 这里使用固定的增量，即匀速运动
        // TODO: 变为匀加速运动？
        res = crr + sign * 0.005;
    } else if (signalType == "none") { // 不设置信号，用于调试，找物理限位
        return crr;
    }
    return res;
}