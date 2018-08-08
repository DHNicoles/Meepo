#ifndef head_data_h__
#define head_data_h__

#include "util/util.hpp"

class HeadData
{
    typedef std::list<cv::Point> TraitsType;
public:
    HeadData(size_t id, const cv::Rect& bbox);
    TraitsType& HeadTrace(){ return trace_; }
    cv::Rect& Box(){ return bbox_; }
private:
    size_t id_;
    cv::Rect bbox_;
    TraitsType trace_;
};


#endif//head_data_h__
