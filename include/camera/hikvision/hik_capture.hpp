#ifndef hik_capture_h__
#define hik_capture_h__
#include "util/util.hpp"
#include "HCNetSDK.h"
#include "PlayM4.h"
#include "camera/camera_manager.hpp"
#include <cstring>
#ifndef WIN32
#include <unistd.h>
#endif
#define CALL_BACK

class HikCapture : public Capture 
{
	private:
		LONG PlayPort_;
		LONG RealPlayHandle_;
		LONG UserID_;
		int dwSize_;
		/************************************************************************/
		/* static global callback function and var                              */
		/************************************************************************/
		static std::map<LONG, LONG> handle_port_map_;
		static std::map<LONG, LONG> port_cameraID_map_;
		CALL_BACK static void g_RealDataCallBack_V30(LONG RealPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void *dwUser);
		CALL_BACK static void g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser);
		CALL_BACK static void g_DecCBFun(LONG nPort, char *pBuf, LONG nSize, FRAME_INFO *pFrameInfo, void* nReserved1, LONG nReserved2);
	public:
		static void InitSdk();
		static void ReleaseSdk();
	public:
		virtual int OnInit(int cameraID, std::string ip, std::string user, std::string password);
		void OnDestroy();
		cv::Mat Query();
};

#endif // hik_capture_h__
