#ifndef tracking_task_h__
#define tracking_task_h__
#include "util/thread_pool.hpp"
#include "tracker_manager/kcftracker.hpp"
#include "game/head_data.hpp"

struct TrackingTaskParam
{
    int _idx;
    cv::Mat& _frame;
    std::shared_ptr<KCFTracker> _kcf; 
    std::shared_ptr<HeadData> _head_data;
    TrackingTaskParam(int idx, cv::Mat& frame, std::shared_ptr<KCFTracker> kcf, std::shared_ptr<HeadData> head_data)
    :_idx(idx), _frame(frame), _kcf(kcf), _head_data(head_data)
    {}
};

class TrackingTask : public ice::Task
{
public:
    TrackingTask(TrackingTaskParam* tracking_task_param)
    {
        tracking_task_param_ = tracking_task_param;
    }
    virtual int run()
    {
		cv::Rect pos = tracking_task_param_->_kcf->update(tracking_task_param_->_frame);
		int cent_x = pos.x + (pos.width >> 1);
		int cent_y = pos.y + (pos.height >> 1);
		tracking_task_param_->_head_data->HeadTrace().emplace_back(cv::Point(cent_x, cent_y));
		tracking_task_param_->_head_data->Box() = pos;
        delete tracking_task_param_;
        return 0;
    } 
    TrackingTaskParam* tracking_task_param_;
};



#endif//tracking_task_h__
