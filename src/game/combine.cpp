#include "game/combine.hpp"
#include "tracker_manager/tracker_manager.hpp"
#include "game/head_data.hpp"

const int l_cent_dist_thresh = 40;
const float l_iou_dist_thresh_hi = 0.6;
const float l_iou_dist_thresh_lo = 0.2;

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
    //LOG(INFO) << "+++++++++++++++++++++++++++++";
    std::map<size_t, std::shared_ptr<HeadData> >& trackers = TrackerManager::Instance() -> Trackers();

    for(int i = 0; i < bbox.size(); ++i)
    {
        int min_dist = INT_MAX, min_id = -1;
        cv::Rect& scale_roi = (bbox[i]);
        for(auto& p : trackers)
        {
            int cdist = distance(scale_roi, p.second->Box());
            if(cdist < min_dist && cdist < l_cent_dist_thresh)
            {
                min_dist = cdist;
                min_id = p.first;
            }
        }
        if(min_dist != INT_MAX)
        {
            //LOG(INFO) << "box " << i <<" hit " << min_id << ", dist " << min_dist;
            trackers[min_id]->SetHit();
            float iou = Iou(scale_roi, trackers[min_id]->Box());
            if(iou < l_iou_dist_thresh_hi && iou > l_iou_dist_thresh_lo)
                TrackerManager::Instance() -> ReplaceTracker(frame, scale_roi, min_id);
        }
        else
        {
            TrackerManager::Instance() -> AddTracker(frame, scale_roi);
        }
    }

    Scanning();
}

void Combine::Scanning()
{
    std::map<size_t, std::shared_ptr<HeadData> >& trackers = TrackerManager::Instance() -> Trackers();
    std::map<size_t, std::shared_ptr<KCFTracker> >& kcfers  = TrackerManager::Instance() -> Kcfers();
    auto t_itr = trackers.begin();
    auto k_itr = kcfers.begin();
    for(; t_itr != trackers.end();)
    {
        t_itr -> second -> OnUpdate();
        if(t_itr->second->Dice())
        {
            //LOG(INFO) << " - rm " << t_itr->first;
            t_itr = trackers.erase(t_itr);
            k_itr = kcfers.erase(k_itr);
        }
        else
        {
            ++t_itr; ++k_itr;
        }
    }

}

