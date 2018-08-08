#include "camera/usbvision/usb_capture.hpp"
#include "camera/camera_manager.hpp"
#include "game/solver.hpp"

int UsbCapture::OnInit(int cameraID, int device_id)
{
    camera_id_ = cameraID;
    device_id_ = device_id;
    cv_cap_.open(device_id); 
    decoder_continue_ = false;
    if(cv_cap_.isOpened())
    {
        cv_cap_.set(cv::CAP_PROP_FPS, Solver::Instance()->AdaptedFrameRate()); 
        StartDecoder();
    }
    return cv_cap_.isOpened() == true? 0 : 1;
}

void UsbCapture::OnDestroy()
{
    StopDecoder();
}

void UsbCapture::StartDecoder()
{
    decoder_continue_ = true;
    pthread_t id_ext;
    pthread_create(&id_ext, NULL, _thread_t<UsbCapture, &UsbCapture::Decoder>, this);  
}

void UsbCapture::StopDecoder()
{
    decoder_continue_ = false;
}

void* UsbCapture::Decoder()
{
   while(decoder_continue_)
   {
        cv::Mat src;
        cv_cap_ >> src;
        if(CameraManager::Instance()->CameraRunning())
            CameraManager::Instance()->InsertMatBuf(camera_id_, src);
   }
}

