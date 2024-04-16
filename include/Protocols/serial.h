#ifndef PROTOCOL_SERIAL_H
#define PROTOCOL_SERIAL_H

#pragma pack(1)
/**
 * * 数据格式均采用 int16
 * ! 注意通信协议中使用大端序，需要做大小端序的转换
 */
typedef struct Header_s {
    unsigned char data[3];
} Header_t;

typedef struct Tail_s {
    unsigned char data[3];
} Tail_t;

typedef short ControlValue_t; // 电机控制值，发送电流

typedef union ControlBuffer_u {
    struct {
        ControlValue_t M1;
        ControlValue_t M2;
        ControlValue_t M3;
        ControlValue_t M4;
        ControlValue_t M5;
        ControlValue_t M6;
        ControlValue_t M7;
        ControlValue_t M8;
        ControlValue_t M9;
        ControlValue_t M10;
        ControlValue_t M11;
        ControlValue_t M12;
        ControlValue_t M13;
        ControlValue_t M14;
    } Motors;
    ControlValue_t Motor[14];
} ControlBuffer_t;

typedef struct MotorValue_s {
    short Position; // 0 ~ 1 位置
    short Speed; // 2 ~ 3 速度
    short Torque; // 4 ~ 5 扭矩
    unsigned char State; // 6 ~ 7   状态，0 表示未读到数据，1表示数据正常
} MotorValue_t; // 电机状态值

typedef union InfoBuffer_u {
    struct {
        MotorValue_t M1;
        MotorValue_t M2;
        MotorValue_t M3;
        MotorValue_t M4;
        MotorValue_t M5;
        MotorValue_t M6;
        MotorValue_t M7;
        MotorValue_t M8;
        MotorValue_t M9;
        MotorValue_t M10;
        MotorValue_t M11;
        MotorValue_t M12;
        MotorValue_t M13;
        MotorValue_t M14;
    } Motors;
    MotorValue_t Motor[14];
} InfoBuffer_t;

typedef struct SendData_s {
    Header_t Header = { 0xAA, 0xBB, 0xCC };
    ControlBuffer_t Buffer;
    Tail_t Tail = { 0xBC, 0xBF, 0xDD };
} DataSend_t; // 上位机下发数据格式

typedef struct ReceiveData_s {
    Header_t Header = { 0xFA, 0xBA, 0xAD };
    InfoBuffer_t Buffer;
    Tail_t Tail = { 0xDA, 0xDC, 0x5D };
} DataReceive_t; // 上位机接收数据格式

#pragma pack()


#define BigDianToLittleDian16(x) ((x >> 8) | (x << 8))
#define LittleDianToBigDian16(x) ((x >> 8) | (x << 8))

/**
 * @brief 将发送的数据包从默认的小端序转成协议的大端序
 * 
 * @param datasend 需要发送的数据包的指针
 */
void ValidDataSend(DataSend_t *datasend)
{
    for (int i = 0; i < 14; i++)
        LittleDianToBigDian16(datasend->Buffer.Motor[i]);
}

/**
 * @brief 将接收的数据包从协议中的大端序转成 linux 默认的小端序
 * 
 * @param datareceive 接收到的数据包的指针
 */
void ValidDataReceive(DataReceive_t *datareceive)
{
    for (int i = 0; i < 14; i++) {
        BigDianToLittleDian16(datareceive->Buffer.Motor[i].Position);
        BigDianToLittleDian16(datareceive->Buffer.Motor[i].Speed);
        BigDianToLittleDian16(datareceive->Buffer.Motor[i].Torque);
    }
}

#endif