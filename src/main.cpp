// STD
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unistd.h>
// boost
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include <boost/filesystem.hpp>
// User
#include "pid/pid.hpp"
#include "motor/motor.hpp"
#include "signalGenerater/signalGenerater.hpp"

using namespace boost::interprocess;

#pragma pack(1)
typedef struct {
    int id;
    float data;
} graph_msg;
#pragma pack()

int main()
{
    // 读取配置文件
    // TODO: 读取配置文件的代码可以封装成一个函数

    double kp, ki, kd;

    boost::filesystem::path p("../config.json");
    std::ifstream ifs(p);
    std::string jsonstr((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
    boost::json::value jv = boost::json::parse(jsonstr);
    boost::json::object jo = jv.as_object();

    kp = jo["pidPram"].as_object().at("kp").as_double();
    ki = jo["pidPram"].as_object().at("ki").as_double();
    kd = jo["pidPram"].as_object().at("kd").as_double();

    std::map<int, pidController> SpeedLoop;
    UniTreeMotor motor("/dev/unitree");

    boost::json::array motorList = jo["motor_list"].as_array();
    for (auto &v : motorList) {
        std::string type;
        int id;
        MotorType motorType;
        double max_q, min_q;

        id = v.as_object().at("id").as_int64();
        type = v.as_object().at("type").as_string();
        max_q = v.as_object().at("max_q").as_double();
        min_q = v.as_object().at("min_q").as_double();
        
        if (type == "A1")
            motorType = MotorType::A1;
        else if (type == "B1")
            motorType = MotorType::B1;
        else if (type == "GO_M8010_6")
            motorType = MotorType::GO_M8010_6;
        else {
            std::cerr << "Motor type error! type:" << type << std::endl;
            exit(-1);
        }

        SpeedLoop.insert(std::pair<int, pidController>(
            id, PID(kp, ki, kd, 20 * queryGearRatio(motorType),
                    -20 * queryGearRatio(motorType))));
        SpeedLoop[id].setLimit(max_q, min_q); // 设置电机物理限位
        motor.addMotor(id, motorType);

        SpeedLoop[id].target = motor.dataList[id].q / queryGearRatio(motorType);
        SpeedLoop[id].init_pos = SpeedLoop[id].target;
    }

    message_queue observemq(open_or_create, "observe", 5, sizeof(graph_msg));
    message_queue targetmq(open_or_create, "target", 5, sizeof(graph_msg));

    message_queue remotemq(open_or_create, "remote", 2, 3);

    signalGenerater signal("../config.json");

    while (true) {
        int8_t recv[3] = { 0, 0, 0 };
        boost::interprocess::message_queue::size_type recvSize;
        unsigned int priority;

        remotemq.try_receive(recv, sizeof(recv), recvSize, priority);
        // 设置信号
        for (auto &v : SpeedLoop) {
            v.second.target = v.second.target_limit(
                signal.generateSignal(v.second.target, recv[v.first]));
            // 判定堵转，A1 最大输出力矩约 34 Nm
            // TODO: 堵转阈值数值计算
            v.second.target = motor.getData(v.first).tau > 20 ?
                                  motor.getDataDivGearRatio(v.first).q :
                                  v.second.target;
        }
        // 发送用于绘图的数据
        // TODO: 配置文件中添加开关
        for (auto &v : SpeedLoop) {
            graph_msg msg;
            msg.id = v.first;
            msg.data = v.second.target;
            targetmq.try_send(&msg, sizeof(graph_msg), 0);
            msg.data = motor.getDataDivGearRatio(v.first).q;
            observemq.try_send(&msg, sizeof(graph_msg), 0);
        }
        // 发送电机控制指令，接收电机数据
        for (auto &v : SpeedLoop) {
            motor.getCmd(v.first).id = v.first;
            motor.getCmd(v.first).q =
                v.second.target * queryGearRatio(motor.motorList[v.first]);
            motor.getCmd(v.first).kp = 0.05;
            motor.getCmd(v.first).kd = 1.2;
            motor.getCmd(v.first).dq = v.second.Update(
                v.second.target * queryGearRatio(motor.motorList[v.first]),
                motor.getData(v.first).q);
            
            motor.getData(v.first).q += motor.getCmd(v.first).dq * 0.01;
            // motor.sendRecv(v.first);
        }
        // 1000 Hz
        usleep(1000);
    }

    return 0;
}