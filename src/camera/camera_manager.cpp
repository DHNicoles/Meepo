#include "camera/camera_manager.hpp"
#include "camera/hikvision/hik_capture.hpp"
#include "camera/usbvision/usb_capture.hpp"
#include "camera/videofile/video_file.hpp"
#include "game/solver.hpp"

CameraManager::CameraManager()
{
    brand_ = NOT_KNOWN;
    camera_running_ = false;
}

CameraManager::~CameraManager()
{
    OnDestroy();
}

void CameraManager::SetBrand(Brand brand)
{
    brand_ = brand;	
}

void CameraManager::OnInit()
{
    if(brand_ == HIKVISION)
    {
        HikCapture::InitSdk();
    }	
}

void CameraManager::RegisterHikCam(int ID, const std::string& ip_addr, const std::string& login_name, const std::string& password)
{
    CHECK_EQ(brand_, CameraManager::HIKVISION) << "The brand(CameraManager::HIKVISION) is not set correctly before registering the camera!";
    CHECK_EQ(cam_ptr_.count(ID), 0) << "Camera id has been registered already! ID: " << ID;
    std::shared_ptr<Capture> cap(new HikCapture);
    cam_ptr_[ID] = cap;
    CHECK_EQ(cap->OnInit(ID, ip_addr, login_name, password), 0) << "Camera registered failed! " << ip_addr;
    LOG(INFO) << " - Hikvision Camera ID [" << ID << "] registered done.";
}

void CameraManager::RegisterUsbCam(int ID, int device_id)
{
    CHECK_EQ(brand_, CameraManager::USB) << "The brand(CameraManager::USB) is not set correctly before registering the camera!";
    CHECK_EQ(cam_ptr_.count(ID), 0) << "Camera id has been registered already! ID: " << ID;
    std::shared_ptr<Capture> cap(new UsbCapture);
    cam_ptr_[ID] = cap;
    CHECK_EQ(cap->OnInit(ID, device_id), 0) << "Camera registered failed! " << device_id;
    LOG(INFO) << " - USB Camera ID [" << ID << "] registered done.";
}

void CameraManager::RegisterFileCam(int ID, std::string file_path)
{
    CHECK_EQ(brand_, CameraManager::VIDEO_FILE) << "The brand(CameraManager::VIDEO_FILE) is not set correctly before registering the camera!";
    CHECK_EQ(cam_ptr_.count(ID), 0) << "Camera id has been registered already! ID: " << ID;
    std::shared_ptr<Capture> cap(new VideoFile);
    cam_ptr_[ID] = cap;
    CHECK_EQ(cap->OnInit(ID, file_path), 0) << "Camera registered failed! " << file_path;
    LOG(INFO) << " - File Capture ID [" << ID << "] registered done.";
}

void CameraManager::OnDestroy()
{
    StopDecoder();
    if(brand_ == HIKVISION)
    {
        HikCapture::ReleaseSdk();
    }	
}

void CameraManager::StartDecoder()
{
    CHECK_EQ(cam_ptr_.size(), 1) << " Only one camera support!";
    camera_running_ = true;
}

void CameraManager::StopDecoder()
{
    camera_running_ = false;
}

bool CameraManager::CameraRunning()
{
    return camera_running_;
}
void CameraManager::InsertMatBuf(int cam_id, cv::Mat& src)
{
    static Stopwatch T0; 
    static int cnt = 0, fps_inter = 50;
    if (cnt % fps_inter == 0)
        T0.Reset(); T0.Start();

    mat_buf_guard_.lock();
    cv::resize(src, src, Solver::Instance()->ViewScope(), 0, 0, cv::INTER_NEAREST);
    mat_buf_[cam_id].emplace_back(src);
    mat_buf_guard_.unlock();
    if (++cnt % fps_inter == 0)
    {
        T0.Stop();
        LOG(INFO) << " - camera-FPS:\t" << fps_inter / T0.GetTime();
    }

}
void CameraManager::GetInputBatch(cv::Mat& frame)
{
    mat_buf_guard_.lock();
    int cam_id = mat_buf_.begin()->first;
    if(mat_buf_[cam_id].empty() == false)    
    {
        frame = mat_buf_[cam_id].front();
        mat_buf_[cam_id].pop_front();
    }
    mat_buf_guard_.unlock(); 
}
