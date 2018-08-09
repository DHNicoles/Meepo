#include "game/head_data.hpp"
#include "tracker_manager/tracker_manager.hpp"
#include "game/solver.hpp"

const int l_verify_interval = 7;
const int l_verify_hit_num  = 5;

const int l_stop_check_interval = 2;//sec
const float l_stop_check_thresh = 2;//L1 pixels

HeadData::HeadData(size_t id, const cv::Rect& bbox)
    :bbox_(bbox)
{
    id_     = id;
    life_   = 1;
    hit_    = 1;
    dice_   = false;
	int cent_x = bbox.x + (bbox.width  >> 1);
	int cent_y = bbox.y + (bbox.height >> 1);
    trace_.emplace_back(cv::Point(cent_x, cent_y));
}

void HeadData::OnUpdate()
{
    if(dice_ == true) return;
    //check hit
    if(life_ == l_verify_interval && hit_ < l_verify_hit_num)
    {
        dice_ = true;
        dice_type_ = Dice_Null;
        return;
    }

    //check out board
    if(TrackerManager::Instance()->IsOutBoard(trace_.back()))
    {
        dice_ = true;
        dice_type_ = Dice_OutBoard;
        return;
    }

    //check stop moving
    int stop_check_size = l_stop_check_interval * Solver::Instance()->AdaptedFrameRate();
    if(trace_.size() > stop_check_size)
    {
        float len = 0.;
        for(int i = trace_.size() - stop_check_size; i < trace_.size(); ++i)
           len += std::abs(trace_[i].x - trace_[i - 1].x) + std::abs(trace_[i].y - trace_[i - 1].y); 
        len /= stop_check_size; 
        LOG(INFO) << "len = " << len;
        if(len < l_stop_check_thresh)
        {
            dice_ = true;
            dice_type_ = (life_ > stop_check_size * 2) ? Dice_StopMove : Dice_Null;
            return;
        }
    }
    
    ++life_;
}
