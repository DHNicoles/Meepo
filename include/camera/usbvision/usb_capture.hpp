#ifndef usb_capture_h__
#define usb_capture_h__
#include "util/util.hpp"
#include "camera/camera_manager.hpp"
#ifndef WIN32
#include <unistd.h>
#endif
#define CALL_BACK

class UsbCapture : public Capture 
{
	public:
		virtual int OnInit(int cameraID, int device_id);
		void OnDestroy();
        void StartDecoder();
        void StopDecoder();
        void* Decoder();
        cv::VideoCapture cv_cap_;
    private:
        int camera_id_, device_id_;
        std::atomic_bool decoder_continue_;
};

typedef void* (UsbCapture::*UsbDecoderProc)(); 
template <typename TYPE, UsbDecoderProc RunThread>  
void* _thread_t(void* param) 
{
    pthread_detach(pthread_self());
    TYPE* This = (TYPE*)param;     
    (This->*RunThread)();     
    return NULL;  
}  

#endif // usb_capture_h__
