#include <boost/interprocess/ipc/message_queue.hpp>
#include <thread>
#include <boost/exception/exception.hpp>
#include <PHOENIX/cvlib/graphTable.hpp>

using namespace boost::interprocess;

typedef std::shared_ptr<std::deque<float>> Points;

int main()
{
    message_queue observemq(open_or_create, "observe0", 2, sizeof(float));
    message_queue targetmq(open_or_create, "target0", 2, sizeof(float));
    message_queue observemq_1(open_or_create, "observe1", 2, sizeof(float));
    message_queue targetmq_1(open_or_create, "target1", 2, sizeof(float));

    Points observedPoints = std::make_shared<std::deque<float>>();
    Points targetPoints = std::make_shared<std::deque<float>>();
    Points observedPoints_1 = std::make_shared<std::deque<float>>();
    Points targetPoints_1 = std::make_shared<std::deque<float>>();

    PHOENIX::cvlib::graphTable table;

    table.addLine("target", targetPoints, cv::Scalar(0, 255, 0));
    table.addLine("observe", observedPoints, cv::Scalar(0, 0, 255));
    table.addLine("target_1", targetPoints_1, cv::Scalar(255, 255, 0));
    table.addLine("observe_1", observedPoints_1, cv::Scalar(0, 255, 255));

    float target, observed, error;
    float target_1, observed_1, error_1;
    boost::interprocess::message_queue::size_type recvSize;
    unsigned int priority;

    while (true) {
        try {
            observemq.try_receive(&observed, sizeof(float), recvSize, priority);
            targetmq.try_receive(&target, sizeof(float), recvSize, priority);
            observemq_1.try_receive(&observed_1, sizeof(float), recvSize,
                                    priority);
            targetmq_1.try_receive(&target_1, sizeof(float), recvSize,
                                   priority);
        } catch (boost::exception& e) {
            observemq.remove("observe0");
            targetmq.remove("target0");
            observemq_1.remove("observe1");
            targetmq_1.remove("target1");
            exit(-1);
        }

        observedPoints->push_back(observed);
        targetPoints->push_back(target);
        observedPoints_1->push_back(observed_1);
        targetPoints_1->push_back(target_1);

        table.ShowTable();
        std::this_thread::yield();
    }
    return 0;
}