#pragma once

#include <opencv2/opencv.hpp>
#include <queue>
#include <map>

struct Line {
    std::shared_ptr<std::deque<float>> pdata;
    cv::Scalar color;
    int show = 1;

    Line(const std::shared_ptr<std::deque<float>> pdata, const cv::Scalar color)
        : pdata(pdata)
        , color(color)
        , show(0)
    {
    }
};

class Table {
public:
    Table();
    ~Table();
    void DrawLine(std::deque<float> &Points,
                  cv::Scalar color = cv::Scalar(0, 0, 255));
    void DrawHorizontalLine();
    void ShowTable();
    void DrawText(std::string text, cv::Point point,
                  cv::Scalar color = cv::Scalar(0, 0, 255));
    void addLine(const std::string name, const Line line);
    void addLine(const std::string name,
                 const std::shared_ptr<std::deque<float>> line,
                 const cv::Scalar color);

private:
    cv::Mat table;
    float unitX;
    float unitY;
    std::map<std::string, Line> lines;
};

void TrackbarCallback(int pos, void *userdata);