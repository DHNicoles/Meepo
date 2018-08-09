#ifndef util_h__
#define util_h__

#include <opencv2/opencv.hpp>
#include <omp.h>

#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>     
#include <algorithm>
#include <assert.h>
#include <iomanip>
#include <iosfwd>
#include <memory>
#include <utility>
#include <deque>
#include <chrono>
#include <future>
#include <thread>
#include <atomic>
#include <pthread.h>
#include "util/easylogging.hpp"
#include "util/Stopwatch.hpp"

// Disable the copy and assignment operator for a class.
#define DISABLE_COPY_AND_ASSIGN(classname) \
private:\
	classname(const classname&); \
	classname& operator=(const classname&)

inline float Iou(const cv::Rect& r1, const cv::Rect& r2)
{
    cv::Rect inter = r1 & r2;
    float ratio = (float)inter.area() / (float)(r1.area() + r2.area() - inter.area());
    return ratio;
}

inline int distance(const cv::Rect& r1, const cv::Rect& r2)
{
    int cent_x_1 = r1.x + (r1.width >> 1);
    int cent_y_1 = r1.y + (r1.height >> 1);
    int cent_x_2 = r2.x + (r2.width >> 1);
    int cent_y_2 = r2.y + (r2.height >> 1);
    return std::abs(cent_x_1 - cent_x_2) + std::abs(cent_y_1 - cent_y_2);
}

inline cv::Rect scaleRoi(const cv::Rect& r)
{
    int cent_x = r.x + (r.width  >> 1);
    int cent_y = r.y + (r.height >> 1);
    int short_side = std::min(r.width, r.height) * 0.7;
    return cv::Rect(cent_x - (short_side >> 1), cent_y - (short_side >> 1), short_side, short_side);
}

#endif//util_h__
