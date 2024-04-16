#include "table/table.hpp"

#include <opencv2/highgui/highgui.hpp>
Table::Table()
{
    table = cv::Mat(cv::Size(1000, 1000), CV_8UC3, cv::Scalar(0, 0, 0));
    unitX = 1000. / 100;
    unitY = 1000. / 100;
    cv::namedWindow("table", cv::WINDOW_NORMAL);
}

Table::~Table()
{
}

void Table::DrawLine(std::deque<float> &Points, cv::Scalar color)
{
    if (Points.size() > 100) {
        Points.erase(Points.begin(), Points.begin() + Points.size() - 100);
    }
    for (int i = 0; i < Points.size(); i++) {
        cv::circle(table, cv::Point(i * unitX, 1000 / 2 - Points[i] * unitY), 1,
                   color, 2);
        if (i) {
            cv::line(
                table,
                cv::Point((i - 1) * unitX, 1000 / 2 - Points[i - 1] * unitY),
                cv::Point(i * unitX, 1000 / 2 - Points[i] * unitY), color, 2);
        }
    }
}

void Table::DrawHorizontalLine()
{
    for (int i = 0; i < 100; i++) {
        cv::circle(table, cv::Point(i * unitX, 1000 / 2), 1,
                   cv::Scalar(255, 255, 255), 1);
    }
    cv::line(table, cv::Point(0, 1000 / 2), cv::Point(1000, 1000 / 2),
             cv::Scalar(255, 255, 255), 2);
}

void Table::ShowTable()
{
    DrawHorizontalLine();
    int i = 1;
    for (auto line : lines) {
        if (line.second.show) {
            DrawLine(*line.second.pdata, line.second.color);
            DrawText(line.first + ": " +
                         std::to_string(line.second.pdata->back()),
                     cv::Point(10, 50 * i), line.second.color);
            i++;
        }
    }
    cv::imshow("table", table);
    cv::waitKey(1);
    table = cv::Scalar(0, 0, 0);
}

void Table::DrawText(std::string text, cv::Point point, cv::Scalar color)
{
    cv::putText(table, text, point, cv::FONT_HERSHEY_SIMPLEX, 1, color, 2);
}

void Table::addLine(const std::string name, const Line line)
{
    lines.insert(std::pair<std::string, Line>(name, line));
    auto l = lines.find(name);
    cv::createTrackbar(l->first, "table", NULL, 1, TrackbarCallback, &(l->second.show));
}

void Table::addLine(const std::string name,
                    const std::shared_ptr<std::deque<float>> line,
                    const cv::Scalar color)
{
    addLine(name, Line(line, color));
}

void TrackbarCallback(int pos, void* userdata)
{
    *(int *)userdata = pos;
    return;
}