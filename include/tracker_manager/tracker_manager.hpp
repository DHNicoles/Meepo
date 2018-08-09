/************************************************************************/
/* Multi-tracker managers                                               */
/************************************************************************/
#ifndef tracker_manager_h__
#define tracker_manager_h__

#include "util/util.hpp"
#include "util/singleton.h"
#include "util/thread_pool.hpp"
class HeadData;
class KCFTracker;
class TrackerManager :public Singleton<TrackerManager>
{
    public:
        TrackerManager();
        ~TrackerManager();
        void OnInit();
        void OnDestroy();
        void OnUpdate(cv::Mat & frame_batch);
        void OnUpdate_Ex(cv::Mat & frame_batch);
        void SetBound(cv::Size, cv::Rect);
        std::map<size_t, std::shared_ptr<HeadData> >& Trackers() { return head_data_map_; }
        std::map<size_t, std::shared_ptr<KCFTracker> >& Kcfers() { return traker_map_; }
        void AddTracker(cv::Mat & frame, const cv::Rect & positionBox);
        void ReplaceTracker(cv::Mat & frame, const cv::Rect & positionBox, size_t ID);
        bool IsOutBoard(const cv::Point& cur_pt);
    private:
        int GetIndex();
        void ExitScaning();
    private:
        std::map<size_t, std::shared_ptr<HeadData> > head_data_map_; 
        std::map<size_t, std::shared_ptr<KCFTracker> > traker_map_; 

        size_t cur_id_;
        std::set<size_t> index_pool_set_;
        cv::Mat bound_mask_;
        const bool HOG_ = true;
        const bool FIXEDWINDOW_ = false;
        const bool MULTISCALE_ = true;
        const bool SILENT_ = true;
        const bool LAB_ = true;

        std::shared_ptr<ice::ThreadPool> threadPool_;
};

#endif // tracker_manager_h__

