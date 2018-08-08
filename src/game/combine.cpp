#include "game/combine.hpp"
#include "tracker_manager/tracker_manager.hpp"
#include "game/head_data.hpp"

const int l_cent_dist_thresh = 80;

Combine::Combine()
{
}

Combine::~Combine()
{
}

void Combine::OnInit()
{
}

void Combine::OnDestroy()
{
}

void Combine::OnUpdate(cv::Mat& frame, std::vector<cv::Rect>& bbox)
{
    std::map<size_t, std::shared_ptr<HeadData> >& trackers = TrackerManager::Instance() -> Trackers();
    for(const auto& roi : bbox)
    {
        bool find = false;
        for(auto& p : trackers)
        {
            int dist = distance(roi, p.second->Box());
            if(dist < l_cent_dist_thresh)
            {
                find = true;
                break;
            }
        }
        if(find == false)
        {
            TrackerManager::Instance() -> AddTracker(frame, roi); 
        }
    }
}


