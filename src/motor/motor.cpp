#include "motor/motor.hpp"

UniTreeMotor::UniTreeMotor(const std::string port)
    : serial(port)
    , cmdList()
    , dataList()
{
}

void UniTreeMotor::addMotor(const int &id, const MotorType &type)
{
    dataList.insert(std::pair<int, MotorData>(id, MotorData()));
    dataList[id].motorType = type;

    cmdList.insert(std::pair<int, MotorCmd>(id, MotorCmd()));
    cmdList[id].mode = queryMotorMode(type, MotorMode::FOC);
    cmdList[id].motorType = type;
    cmdList[id].id = id;
    cmdList[id].kp = 0.;
    cmdList[id].kd = 0.;
    cmdList[id].q = 0.;
    cmdList[id].dq = 0.;
    cmdList[id].tau = 0.;
    motorList.insert(std::pair<int, MotorType>(id, type));
    serial.sendRecv(&(cmdList[id]), &(dataList[id]));
}

UniTreeMotor::~UniTreeMotor()
{
    for (auto cmd : cmdList) {
        cmd.second.mode =
            queryMotorMode(motorList[cmd.first], MotorMode::BRAKE);
        serial.sendRecv(&cmd.second, &dataList.find(cmd.first)->second);
    }
}