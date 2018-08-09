#include "game/solver.hpp"
#include "camera/camera_manager.hpp"
#include "tracker_manager/tracker_manager.hpp"
#include "detector/detector.hpp"
#include "game/sync.hpp"
#include "game/combine.hpp"
#include "game/head_data.hpp"

INITIALIZE_EASYLOGGINGPP;

const cv::Size l_view_scope(600, 400);
const double l_adapted_frame_rate = 20.0;
int Solver::OnInit()
{
    view_scope_ = l_view_scope;
    adapted_frame_rate_ = l_adapted_frame_rate;

	tracker_manager_.reset(new TrackerManager());
	TrackerManager::Instance()->OnInit();

	detector_.reset(new Detector());
	Detector::Instance()->OnInit();
    syncor_.reset(new Sync());
    Sync::Instance()->OnInit();

    camera_manager_.reset(new CameraManager());
	CameraManager::Instance()->SetBrand(CameraManager::VIDEO_FILE);	
	CameraManager::Instance()->OnInit();
    const std::string video_file = "/home/intel/workspace/Meepo/resource/video_file/crop_01.mkv";
    //const std::string video_file = "/home/intel/Test/code/test.mp4";
    CameraManager::Instance()->RegisterFileCam(0, video_file);
	
	return 0;
}

/////////////////////////
///Meepo server start!///
/////////////////////////
void Solver::Start()
{
	LOG(INFO) << " - Meepo server starting...";
	solver_continue_ = true;
	CameraManager::Instance()->StartDecoder();
	LOG(INFO) << " - Meepo server start successfully!";
	Solve();
}

/////////////////////////
///Meepo server stop!////
/////////////////////////
int Solver::Stop()
{
	LOG(ERROR) << " - [Java-Call] Meepo server stopping...";

	//Stop Alg
	CameraManager::Instance()->StopDecoder();
	solver_continue_ = false;

	return 0;
}

Solver::~Solver()
{
	OnDestroy();
}

void Solver::OnDestroy()
{
}

/////////////////////////////////////////
//Run Meepo services in the foreground///
/////////////////////////////////////////
void Solver::Solve()
{
	static Stopwatch T0;
	static int cnt = 0, fps_inter = 50;
	while (solver_continue_)
	{
		if (cnt % fps_inter == 0)
		{
			T0.Reset();	T0.Start();
		}
        //Getbatch;
		cv::Mat frame;
		while(frame.empty())
			CameraManager::Instance()->GetInputBatch(frame); 

        // head detector runs ///
        std::vector<cv::Rect> bbox;
        std::thread t_head_detector([&]
                {
                Detector::Instance()->OnUpdate(frame, bbox);
                });


        /// tracker runs ////////
        TrackerManager::Instance()->OnUpdate_Ex(frame);		

        t_head_detector.join();

        ////combine runs/////////
        Combine::Instance()->OnUpdate(frame, bbox);

        /// draw tracker ///
        cv::Mat tmp = frame.clone();
        DisplayTracker(tmp);
        //for(auto& r : bbox)
        //{
        //    cv::rectangle(frame, r, cv::Scalar(0, 0, 0), 4, 4);
        //}
        //cv::imshow("detector", frame);
        //cv::waitKey(1);

		if (++cnt % fps_inter == 0)
		{
			T0.Stop();
			LOG(INFO) << " - Meepo-FPS:\t" << fps_inter / T0.GetTime();
			LOG(INFO) << " - Tracker num:\t" << TrackerManager::Instance()->Trackers().size();
		}
	}
	LOG(INFO) << " - Meepo server stoped!";
}

void Solver::DisplayTracker(cv::Mat& frame)
{
	static std::vector<cv::Scalar> color_pool =
	{   
		CV_RGB(255, 0, 0), 
		CV_RGB(0, 255, 0), 
		CV_RGB(0, 0, 255),
		CV_RGB(49, 79, 79),
		CV_RGB(139, 58, 59),
		CV_RGB(148, 0, 211),
		CV_RGB(255, 140, 0)
	};

    std::map<size_t, std::shared_ptr<HeadData> >& trackers = TrackerManager::Instance() -> Trackers();
	for(auto& p : trackers)
		cv::rectangle(frame, p.second->Box(), color_pool[p.first % color_pool.size()], 2, 1, 0);

    cv::imshow("tracker", frame);
    cv::waitKey(1);
}
