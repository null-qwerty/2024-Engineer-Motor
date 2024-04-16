// STD
#include <iostream>
#include <vector>
#include <queue>
#include <boost/property_tree/json_parser.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <unistd.h>
// User
#include "pid/pid.hpp"
#include "motor/motor.hpp"

using namespace boost::interprocess;

int main()
{
    // 读取配置文件
    boost::property_tree::ptree pt, pidPram, signal;
    boost::property_tree::read_json("../config.json", pt);
    double kp, ki, kd, amplitude, signslOffset;
    std::string signalType;

    pidPram = pt.get_child("pidPram");
    signal = pt.get_child("signal");
    kp = pidPram.get<double>("kp");
    ki = pidPram.get<double>("ki");
    kd = pidPram.get<double>("kd");
    signalType = signal.get<std::string>("type");
    amplitude = signal.get<double>("amplitude");
    signslOffset = signal.get<double>("offset");

    message_queue observemq(open_or_create, "observe0", 2, sizeof(float));
    message_queue targetmq(open_or_create, "target0", 2, sizeof(float));
    message_queue observemq_1(open_or_create, "observe1", 2, sizeof(float));
    message_queue targetmq_1(open_or_create, "target1", 2, sizeof(float));

    message_queue remotemq(open_or_create, "remote", 2, 3);

    std::map<int, pidController> SpeedLoop;
    SpeedLoop.insert(std::pair<int, pidController>(
        0, PID(kp, ki, kd, 20 * queryGearRatio(MotorType::A1),
               -20 * queryGearRatio(MotorType::A1))));
    SpeedLoop.insert(std::pair<int, pidController>(
        2, PID(kp, ki, kd, 20 * queryGearRatio(MotorType::A1),
               -20 * queryGearRatio(MotorType::A1))));
    // SpeedLoop.setFilter(50., 1 / 1000.0);
    // 初始化电机
    UniTreeMotor motor("/dev/ttyUSB1");
    motor.addMotor(0, MotorType::A1);
    motor.addMotor(2, MotorType::A1);

    SpeedLoop[0].target = motor.dataList[0].q / queryGearRatio(MotorType::A1);
    SpeedLoop[2].target = motor.dataList[2].q / queryGearRatio(MotorType::A1);
    SpeedLoop[0].setLimit(1.18, -2.45);
    SpeedLoop[2].setLimit(4.4, 0.2);
    SpeedLoop[0].init_pos = SpeedLoop[0].target;
    SpeedLoop[2].init_pos = SpeedLoop[2].target;

    double x = 0.;

    while (true) {
        if (signalType == "sin") {
            SpeedLoop[0].target = signslOffset + amplitude * sin(x);
            SpeedLoop[2].target = signslOffset + amplitude * sin(x + M_PI / 2);
            x += 0.01;
            if (x >= 2 * M_PI)
                x = x - 2 * M_PI;
        } else if (signalType == "step") {
            SpeedLoop[0].target = signslOffset + amplitude;
            SpeedLoop[2].target = signslOffset + amplitude + 1;
        } else if (signalType == "remote") {
            int8_t recv[3] = { 0, 0, 0 };
            boost::interprocess::message_queue::size_type recvSize;
            unsigned int priority;

            remotemq.try_receive(recv, sizeof(recv), recvSize, priority);
            SpeedLoop[0].target = SpeedLoop[0].target_limit(
                SpeedLoop[0].target + (int)recv[1] * 0.005);
            SpeedLoop[2].target = SpeedLoop[2].target_limit(
                SpeedLoop[2].target + (int)recv[2] * 0.005);
        } else if (signalType == "none") {
            SpeedLoop[0].target =
                motor.dataList[0].q / queryGearRatio(MotorType::A1);
            SpeedLoop[2].target =
                motor.dataList[2].q / queryGearRatio(MotorType::A1);
        }

        SpeedLoop[0].observed =
            motor.getData(0).q / queryGearRatio(MotorType::A1);

        SpeedLoop[2].observed =
            motor.getData(2).q / queryGearRatio(MotorType::A1);

        // SpeedLoop[0].target = motor.getData(0).tau > 

        SpeedLoop[0].error = SpeedLoop[0].target - SpeedLoop[0].observed;
        SpeedLoop[2].error = SpeedLoop[2].target - SpeedLoop[2].observed;

        observemq.try_send(&(SpeedLoop[0].observed), sizeof(float), 0);
        targetmq.try_send(&(SpeedLoop[0].target), sizeof(float), 0);
        observemq_1.try_send(&(SpeedLoop[2].observed), sizeof(float), 0);
        targetmq_1.try_send(&(SpeedLoop[2].target), sizeof(float), 0);

        motor.getCmd(0).id = 0;
        motor.getCmd(0).q = SpeedLoop[0].target * queryGearRatio(MotorType::A1);
        motor.getCmd(0).kp = 0.05;
        motor.getCmd(0).kd = 1.2;
        motor.getCmd(0).dq = SpeedLoop[0].Update(
            SpeedLoop[0].target * queryGearRatio(MotorType::A1),
            motor.getData(0).q);
        motor.getCmd(2).id = 2;
        motor.getCmd(2).q = SpeedLoop[2].target * queryGearRatio(MotorType::A1);
        motor.getCmd(2).kp = 0.05;
        motor.getCmd(2).kd = 1.2;
        motor.getCmd(2).dq = SpeedLoop[2].Update(
            SpeedLoop[2].target * queryGearRatio(MotorType::A1),
            motor.getData(2).q);

        // motor.getData(0).q += motor.getCmd(0).dq * 0.01;
        // motor.getData(2).q += motor.getCmd(2).dq * 0.01;
        motor.sendRecv(0);
        motor.sendRecv(2);

        usleep(1000);
    }

    return 0;
}