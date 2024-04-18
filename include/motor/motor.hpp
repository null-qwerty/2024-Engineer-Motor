#pragma once

#include "serialPort/SerialPort.h"
#include "unitreeMotor/unitreeMotor.h"

#include <map>

class UniTreeMotor {
public:
    /**
     * @brief 构造函数
     * 
     * @param[in] port 串口位置 
     */
    UniTreeMotor(const std::string port = "/dev/ttyUSB0");
    /**
     * @brief 添加电机
     * 
     * @param[in] id 电机 ID
     * @param[in] type 电机类型
     */
    void addMotor(const int &id, const MotorType &type);
    /**
     * @brief 析构函数
     * 
     */
    ~UniTreeMotor();
    /**
     * @brief 发送并接收电机消息
     * 
     * @param[in] id 电机 ID
     * 
     * @return true 成功
     * @return false 失败
     */
    bool sendRecv(const int &id = 0)
    {
        return serial.sendRecv(&getCmd(id), &getData(id));
    }
    /**
     * @brief 发送并接收电机消息
     * 
     * @param[in] cmd 控制数据
     * @param[out] data 反馈数据 
     * @return true 成功
     * @return false 失败
     */
    bool sendRecv(const MotorCmd &cmd, MotorData &data)
    {
        this->getCmd(cmd.id) = cmd;
        bool state = this->sendRecv(cmd.id);
        data = this->getData(cmd.id);
        return state;
    }
    /**
     * @brief 获取特定 id 的电机控制数据
     * 
     * @param id 电机 id
     * @return MotorCmd& 电机控制数据 
     */
    MotorCmd& getCmd(const int &id)
    {
        return cmdList[id];
    }
    /**
     * @brief 获取特定 id 的电机反馈数据
     * 
     * @param id 电机 id
     * @return MotorData& 电机反馈数据 
     */
    MotorData& getData(const int &id)
    {
        return dataList[id];
    }
    /**
     * @brief 获取特定 id 的电机控制数据，除以减速比
     * 
     * @param id 电机 id
     * @return MotorData 电机反馈数据，q 和 dq 除以减速比 
     */
    MotorData getDataDivGearRatio(const int& id)
    {
        MotorData temp = dataList[id];
        temp.q /= queryGearRatio(motorList[id]);
        temp.dq /= queryGearRatio(motorList[id]);
        return temp;
    }
    
    std::map<int,MotorCmd> cmdList; ///< 电机命令
    std::map<int,MotorData> dataList; ///< 电机数据
    std::map<int, MotorType> motorList; ///< 电机列表

private:
    SerialPort serial; ///< 串口
};