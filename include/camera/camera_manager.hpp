#ifndef camera_manager_h__
#define camera_manager_h__

#include "util/singleton.h" 

class Capture
{
public:
    virtual int OnInit(int cameraID, std::string ip, std::string user, std::string password){}
    virtual int OnInit(int cameraID, int device_id){}
    virtual int OnInit(int cameraID, std::string file_path){}
};

class CameraManager : public Singleton<CameraManager>
{
public:
	enum Brand
	{
		USB = 0,
		HIKVISION = 1,
		VIDEO_FILE = 2,
        NOT_KNOWN = 3
	};

    CameraManager();
    ~CameraManager();
    void SetBrand(Brand brand);
    void OnInit();
	void OnDestroy();
	void StartDecoder();
	void StopDecoder();
    void RegisterHikCam(int ID, const std::string& ip_addr, const std::string& login_name, const std::string& password);
    void RegisterUsbCam(int ID, int device_id);
    void RegisterFileCam(int ID, std::string file_path);
    bool CameraRunning(); 
    void InsertMatBuf(int cam_id, cv::Mat& src);
    void GetInputBatch(cv::Mat& frame);
private:
	std::map<int, std::shared_ptr<Capture> > cam_ptr_;
    std::map<int, std::list<cv::Mat> > mat_buf_;
    Brand brand_;
    std::atomic_bool camera_running_;
    std::mutex mat_buf_guard_;


	DISABLE_COPY_AND_ASSIGN(CameraManager);
};
#endif//camera_manager_h__
