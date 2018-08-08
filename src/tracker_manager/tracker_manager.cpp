#include "tracker_manager/tracker_manager.hpp"
#include "tracker_manager/tracking_task.hpp"
#include "tracker_manager/kcftracker.hpp"
#include "game/head_data.hpp"
#include "game/solver.hpp"

const int l_bound_margin = 20;
const int l_thread_num = 5;
TrackerManager::TrackerManager()
    :cur_id_(0)
{
    threadPool_.reset(new ice::ThreadPool(l_thread_num));
}
TrackerManager::~TrackerManager()
{
	OnDestroy();
}
void TrackerManager::OnInit()
{
    const cv::Size& scope = Solver::Instance()->ViewScope();
    cv::Rect bound(l_bound_margin, l_bound_margin, scope.width - 2 * l_bound_margin, scope.height - 2 * l_bound_margin);
    SetBound(scope, bound);

    ice::ThreadPool::Instance()->start();
    LOG(INFO) << " - tracker thread pool(" << l_thread_num << ") start done";
}

void TrackerManager::OnDestroy()
{
}

void TrackerManager::AddTracker(cv::Mat & frame, const cv::Rect & positionBox)
{
	int index = GetIndex();
	LOG(INFO) << " - add tracker ID: " << index;
    std::shared_ptr<KCFTracker> tracker_ptr(new KCFTracker(HOG_, FIXEDWINDOW_, MULTISCALE_, LAB_));
	tracker_ptr->init(positionBox, frame);
	traker_map_[index] = tracker_ptr;

	head_data_map_[index] = std::shared_ptr<HeadData>(new HeadData(index, positionBox));
}

void TrackerManager::OnUpdate_Ex(cv::Mat & frame)
{
	for(auto& p : traker_map_)
	{
	    TrackingTaskParam* param = new TrackingTaskParam(p.first, frame, p.second, head_data_map_[p.first]);	
        TrackingTask* task_obj = new TrackingTask(param);
        ice::ThreadPool::Instance()->addTask(task_obj);
	}
    while(ice::ThreadPool::Instance()->size() != 0);
}

void TrackerManager::OnUpdate(cv::Mat & frame)
{
	//update kcf_trakers and object information
	std::map<size_t, std::shared_ptr<KCFTracker> >::iterator tracker_itr = traker_map_.begin();
	std::map<size_t, std::shared_ptr<KCFTracker> >::iterator tracker_end_itr = traker_map_.end();
	for (; tracker_itr != tracker_end_itr; ++tracker_itr)
	{
		cv::Rect pos = tracker_itr->second->update(frame);
		int cent_x = pos.x + (pos.width >> 1);
		int cent_y = pos.y + (pos.height >> 1);
		head_data_map_[tracker_itr->first]->HeadTrace().emplace_back(cv::Point(cent_x, cent_y));
		head_data_map_[tracker_itr->first]->Box() = pos;
	}
}

void TrackerManager::SetBound(cv::Size size, cv::Rect bound)
{
	bound_mask_ = cv::Mat(size, CV_8UC1);
	bound_mask_ = cv::Scalar::all(0);
	bound_mask_(bound) = cv::Scalar::all(255);
}

int TrackerManager::GetIndex()
{
    return ++cur_id_;
}

