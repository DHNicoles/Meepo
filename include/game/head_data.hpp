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
    void SetHit() { ++hit_; }
    const bool Dice() { return dice_; }
    void OnUpdate();
private:
    size_t      id_;
    cv::Rect    bbox_;
    TraitsType  trace_;
    int         life_;
    int         hit_;
    bool        dice_;
};


#endif//head_data_h__
