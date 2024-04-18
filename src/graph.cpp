#include <thread>
#include <iostream>
#include <fstream>
#include <map>

#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include <boost/filesystem.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/exception/exception.hpp>

#include <PHOENIX/cvlib/graphTable.hpp>

using namespace boost::interprocess;

typedef std::shared_ptr<std::deque<float>> Points;

#pragma pack(1)
typedef struct {
    int id;
    float data;
} graph_msg;
#pragma pack()

int main()
{
    boost::filesystem::path p("../config.json");
    std::ifstream ifs(p);
    std::string jsonstr((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
    boost::json::value jv = boost::json::parse(jsonstr);
    boost::json::object jo = jv.as_object();

    message_queue observemq(open_or_create, "observe", 5, sizeof(graph_msg));
    message_queue targetmq(open_or_create, "target", 5, sizeof(graph_msg));

    std::map<int, Points> observedPoints;
    std::map<int, Points> targetPoints;

    PHOENIX::cvlib::graphTable table;

    cv::RNG rng(time(NULL));

    for (auto v : jo.at("motor_list").as_array()) {
        int id = v.at("id").as_int64();
        observedPoints[id] = std::make_shared<std::deque<float>>();
        targetPoints[id] = std::make_shared<std::deque<float>>();
        table.addLine("target" + std::to_string(id), targetPoints[id],
                      cv::Scalar(rng.uniform(128, 255), rng.uniform(128, 255),
                                 rng.uniform(128, 255)));
        table.addLine("observe" + std::to_string(id), observedPoints[id],
                      cv::Scalar(rng.uniform(128, 255), rng.uniform(128, 255),
                                 rng.uniform(128, 255)));
    }

    float target, observed, error;
    float target_1, observed_1, error_1;
    boost::interprocess::message_queue::size_type recvSize;
    unsigned int priority;

    while (true) {
        graph_msg msg[5];
        try {
            observemq.try_receive(&msg, sizeof(graph_msg), recvSize, priority);
            observedPoints[msg[0].id]->push_back(msg[0].data);
            targetmq.try_receive(&msg, sizeof(graph_msg), recvSize, priority);
            targetPoints[msg[0].id]->push_back(msg[0].data);
        } catch (boost::exception &e) {
            observemq.remove("observe");
            targetmq.remove("target");
            exit(-1);
        }

        table.ShowTable();
        // std::this_thread::yield();
    }
    return 0;
}