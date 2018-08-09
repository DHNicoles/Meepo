#ifndef solver_h__
#define solver_h__
#include "util/util.hpp"
#include "util/singleton.h" 

class CameraManager;
class TrackerManager;
class Detector;
class Sync;
class Solver : public Singleton<Solver>
{
	public:
        Solver(){}
		int OnInit();
		void Start();
		int Stop();
		~Solver();
		void OnDestroy();
        const cv::Size& ViewScope(){ return view_scope_; }
        const double AdaptedFrameRate() { return adapted_frame_rate_; }
	private:
		void Solve();
		void DisplayTracker(cv::Mat& frame);

	private:
		std::atomic_bool solver_continue_;
		std::shared_ptr<CameraManager> camera_manager_;
		std::shared_ptr<TrackerManager> tracker_manager_;
		std::shared_ptr<Detector> detector_;
		std::shared_ptr<Sync> syncor_;
        cv::Size view_scope_;
        double adapted_frame_rate_;

	DISABLE_COPY_AND_ASSIGN(Solver);
};

#endif // solver_h__

