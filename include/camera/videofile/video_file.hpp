#ifndef video_file_h__
#define video_file_h__
#include "util/util.hpp"
#include "camera/camera_manager.hpp"
#ifndef WIN32
#include <unistd.h>
#endif
#define CALL_BACK

class VideoFile : public Capture 
{
	public:
		virtual int OnInit(int cameraID, std::string file_path);
		void OnDestroy();
        void StartDecoder();
        void StopDecoder();
        void* Decoder();
        cv::VideoCapture cv_cap_;
    private:
        int camera_id_;
        std::string video_path_;
        std::atomic_bool decoder_continue_;
};

typedef void* (VideoFile::*FileDecoderProc)(); 
template <typename TYPE, FileDecoderProc RunThread>  
void* _thread_t(void* param) 
{
    pthread_detach(pthread_self());
    TYPE* This = (TYPE*)param;     
    (This->*RunThread)();     
    return NULL;  
}  

#endif // video_file_h__
