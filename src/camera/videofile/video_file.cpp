#include "camera/videofile/video_file.hpp"
#include "camera/camera_manager.hpp"
#include "game/solver.hpp"

int VideoFile::OnInit(int cameraID, std::string file_path)
{
    camera_id_ = cameraID;
    video_path_ = file_path;
    cv_cap_.open(file_path); 
    decoder_continue_ = false;
    if(cv_cap_.isOpened())
    {
        StartDecoder();
    }
    return cv_cap_.isOpened() == true? 0 : 1;
}

void VideoFile::OnDestroy()
{
    StopDecoder();
}

void VideoFile::StartDecoder()
{
    decoder_continue_ = true;
    pthread_t id_ext;
    pthread_create(&id_ext, NULL, _thread_t<VideoFile, &VideoFile::Decoder>, this);  
}

void VideoFile::StopDecoder()
{
    decoder_continue_ = false;
}

void* VideoFile::Decoder()
{
   while(decoder_continue_)
   {
        cv::Mat src;
        auto start = std::chrono::system_clock::now();
        cv_cap_ >> src;
        auto end = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        double decode_cost = double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den * 1000;
        if(CameraManager::Instance()->CameraRunning() && src.empty() == false)
            CameraManager::Instance()->InsertMatBuf(camera_id_, src);
        //sleep for a while
        std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0 / Solver::Instance()->AdaptedFrameRate() - decode_cost)));
   }
}

