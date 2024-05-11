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
    motorList.insert(std::pair<int, MotorType>(id, type));
    while (dataList[id].merror) {
        initailizeMotor(id);
    }
    motorState.insert(std::pair<int, int>(id, MotorState::CONNECTED));
}

UniTreeMotor::~UniTreeMotor()
{
    for (auto cmd : cmdList) {
        // cmd.second.mode =
        //     queryMotorMode(motorList[cmd.first], MotorMode::BRAKE);
        cmd.second.q = 0.;
        cmd.second.dq = 0.;
        cmd.second.tau = 0.;
        cmd.second.kp = 0.;
        cmd.second.kd = 0.;
        serial.sendRecv(&cmd.second, &dataList.find(cmd.first)->second);
        usleep(1000);
    }
}

void UniTreeMotor::initailizeMotor(int id)
{
    cmdList[id].mode = queryMotorMode(motorList[id], MotorMode::FOC);
    cmdList[id].motorType = motorList[id];
    cmdList[id].id = id;
    cmdList[id].kp = 0.;
    cmdList[id].kd = 0.;
    cmdList[id].q = 0.;
    cmdList[id].dq = 0.;
    cmdList[id].tau = 0.;
    serial.sendRecv(&(cmdList[id]), &(dataList[id]));
    if (!dataList[id].merror) {
        motorState[id] = MotorState::CONNECTING;
    }
}