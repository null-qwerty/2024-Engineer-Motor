#include <PHOENIX/Serial/Serial.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>

int main()
{
    PHOENIX::Serial remote("/dev/ttyUSB0", 115200);
    uint8_t recv[3];
    boost::interprocess::message_queue mq(boost::interprocess::open_or_create,
                                          "remote", 2, 3);
    while(true)
    {
        remote.receive(recv, 1);
        if(recv[0] == 0xa1){
            remote.receive(recv + 1, 2);
            break;
        }
    }

    while (1) {
        remote.receive(recv, sizeof(recv));
        std::cout << std::hex << (int)recv[0] << " " << (int)recv[1] << " "
                  << (int)recv[2] << std::endl;
        recv[0] = recv[1];
        recv[1] = 0x00;
        mq.try_send(recv, sizeof(recv), 0);
    }

    return 0;
}