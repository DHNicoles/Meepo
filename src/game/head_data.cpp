#include "game/head_data.hpp"
#include "tracker_manager/tracker_manager.hpp"

const int l_verify_interval = 7;
const int l_verify_hit_num  = 5;
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
        return;
    }
    else
    {
        //LOG(INFO) << "id, life, hit: " << id_ << "," << life_ << ", " << hit_; 
    }

    //check out board
    if(TrackerManager::Instance()->IsOutBoard(trace_.back()))
    {
        dice_ = true;
        return;
    }


    ++life_;
}
