#include "camera/hikvision/hik_capture.hpp"

std::map<LONG, LONG> HikCapture::handle_port_map_;
std::map<LONG, LONG> HikCapture::port_cameraID_map_;

void HikCapture::InitSdk()
{
	if (!NET_DVR_Init()) {
		LOG(FATAL) << "init Hik sdk error";
	}
	//设置连接时间与重连时间
	NET_DVR_SetConnectTime(2000, 1);
	NET_DVR_SetReconnect(10000, true);
	//---------------------------------------
	//设置异常消息回调函数
	NET_DVR_SetExceptionCallBack_V30(0, NULL, g_ExceptionCallBack, NULL);
}

void HikCapture::ReleaseSdk()
{
	NET_DVR_Cleanup();
}

int HikCapture::OnInit(int cameraID, std::string ip, std::string user, std::string password)
{
	//登录参数，包括设备地址、登录用户、密码等
	int port = 8000;
	//---------------------------------------
	// 注册设备

	NET_DVR_USER_LOGIN_INFO struLoginInfo = { 0 };
	struLoginInfo.bUseAsynLogin = 0; //同步登录方式
	strcpy(struLoginInfo.sDeviceAddress, ip.c_str()); //设备IP地址
	struLoginInfo.wPort = port; //设备服务端口
	strcpy(struLoginInfo.sUserName, user.c_str()); //设备登录用户名
	strcpy(struLoginInfo.sPassword, password.c_str()); //设备登录密码

	//设备信息, 输出参数
	NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = { 0 };

	UserID_ = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);

	if (UserID_ < 0) {
		printf("Login failed, error code: %d\n", NET_DVR_GetLastError());
		NET_DVR_Cleanup();
		return -1;
	}

	NET_DVR_PREVIEWINFO struPlayInfo = { 0 };
	struPlayInfo.hPlayWnd = NULL;         //需要SDK解码时句柄设为有效值，仅取流不解码时可设为空
	struPlayInfo.lChannel = 1;       //预览通道号
	struPlayInfo.dwStreamType = 0;       //0-主码流，1-子码流，2-码流3，3-码流4，以此类推
	struPlayInfo.dwLinkMode = 0;       //0- TCP方式，1- UDP方式，2- 多播方式，3- RTP方式，4-RTP/RTSP，5-RSTP/HTTP
	struPlayInfo.bBlocked = 1;       //0- 非阻塞取流，1- 阻塞取流

	NET_DVR_COMPRESSIONCFG_V30  struParams = { 0 };
	DWORD dwReturnLen;
	int Ret = NET_DVR_GetDVRConfig(UserID_, NET_DVR_GET_COMPRESSCFG_V30, struPlayInfo.lChannel, &struParams, sizeof(NET_DVR_COMPRESSIONCFG_V30), &dwReturnLen);
	if (!Ret)
	{
		printf("Reset camera parameters failed, error code: %d\n", NET_DVR_GetLastError());
		NET_DVR_Cleanup();
		return -1;
	}
	struParams.struNormHighRecordPara.byStreamType = 0;
	struParams.struNormHighRecordPara.dwVideoFrameRate = 16; //15-18, 16-22, 17-25
	struParams.struNormHighRecordPara.dwVideoBitrate = 23;
	struParams.struNormHighRecordPara.byResolution = 27;   //16可以到子码流的最大分辨率
	int SetCamera;
	SetCamera = NET_DVR_SetDVRConfig(UserID_, NET_DVR_SET_COMPRESSCFG_V30, struPlayInfo.lChannel,
		&struParams, sizeof(NET_DVR_COMPRESSIONCFG_V30));
	if (SetCamera)
	{
		//LOG(INFO) << " - 修改分辨率成功" << std::endl;
		//LOG(INFO) << " - 帧率" << struParams.struNormHighRecordPara.dwVideoBitrate << std::endl;
		//LOG(INFO) << " - 分辨率" << struParams.struNormHighRecordPara.byResolution << std::endl;
	}
	else
	{
		printf("Reset camera parameters failed, error code: %d\n", NET_DVR_GetLastError());
		NET_DVR_Cleanup();
		return -1;
	}
	//---------------------------------------
	//启动预览并设置回调数据流

	RealPlayHandle_ = NET_DVR_RealPlay_V40(UserID_, &struPlayInfo, g_RealDataCallBack_V30, NULL);
	//LOG(INFO) << " - UserID_ = " << UserID_ << ", RealPlayHandle_ = " << RealPlayHandle_ << ", port = " << handle_port_map_[RealPlayHandle_];
	if (RealPlayHandle_ < 0) {
		LOG(ERROR) << "NET_DVR_RealPlay_V40 error, " << NET_DVR_GetLastError();
		NET_DVR_Logout(UserID_);
		NET_DVR_Cleanup();
		return -1;
	}
	sleep(1);

	//获取当前视频文件的分辨率
	int dwWidth = 0;
	int dwHeight = 0;

	int bFlag = PlayM4_GetPictureSize(handle_port_map_[RealPlayHandle_], &dwWidth, &dwHeight);
	//Set map from port to cameraID 
	port_cameraID_map_[handle_port_map_[RealPlayHandle_]] = cameraID;
	//end Set 
	dwSize_ = dwWidth * dwHeight;
	//LOG(INFO) << " - Test dwSize_ = " << dwSize_;

	return 0;
}

void HikCapture::OnDestroy()
{
	//---------------------------------------
	//关闭预览
	NET_DVR_StopRealPlay(RealPlayHandle_);

	//释放播放库资源
	PlayM4_Stop(handle_port_map_[RealPlayHandle_]);
	PlayM4_CloseStream(handle_port_map_[RealPlayHandle_]);
	PlayM4_FreePort(handle_port_map_[RealPlayHandle_]);

	//注销用户
	NET_DVR_Logout(UserID_);
}

cv::Mat HikCapture::Query()
{
	unsigned int capSize = 0;
	BYTE *jPic = new BYTE[dwSize_ * 5];
	int bFlag = PlayM4_GetJPEG(handle_port_map_[RealPlayHandle_], jPic, dwSize_, &capSize);
	if (bFlag == FALSE)
	{
		LOG(ERROR) << "get bmp pic error, code=" << PlayM4_GetLastError(handle_port_map_[RealPlayHandle_]);
		return cv::Mat();
	}
	std::vector<BYTE>vec(jPic, jPic + capSize);
	delete jPic;
	return cv::imdecode(vec, 1);
}
void HikCapture::g_RealDataCallBack_V30(LONG RealPlayHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, void *dwUser) {
	switch (dwDataType) {
	case NET_DVR_SYSHEAD: //系统头
		//LOG(INFO) << "[]g_RealDataCallBack_V30] RealPlayHandle = " << RealPlayHandle;
		handle_port_map_[RealPlayHandle] = -1;
		if (!PlayM4_GetPort(&handle_port_map_[RealPlayHandle]))  //获取播放库未使用的通道号
		{
			LOG(ERROR) << "get port error";
			break;
		}
		//m_iPort = lPort; //第一次回调的是系统头，将获取的播放库port号赋值给全局port，下次回调数据时即使用此port号播放
		if (dwBufSize > 0) {
			if (!PlayM4_SetStreamOpenMode(handle_port_map_[RealPlayHandle], STREAME_REALTIME))  //设置实时流播放模式
			{
				LOG(ERROR) << "set stream mode error, code=" << PlayM4_GetLastError(handle_port_map_[RealPlayHandle]);
				break;
			}

			if (!PlayM4_OpenStream(handle_port_map_[RealPlayHandle], pBuffer, dwBufSize, 1024 * 1024)) //打开流接口
			{
				LOG(ERROR) << "open stream error, code=" << PlayM4_GetLastError(handle_port_map_[RealPlayHandle]);
				break;
			}
			//设置解码回调函数 只解码不显示  
			if (!PlayM4_SetDecCallBack(handle_port_map_[RealPlayHandle], g_DecCBFun))
			{
				LOG(ERROR) << "set DecCBFun back error, code=" << PlayM4_GetLastError(handle_port_map_[RealPlayHandle]);
				break;
			}
			if (!PlayM4_Play(handle_port_map_[RealPlayHandle], NULL)) //播放开始
			{
				LOG(ERROR) << "play error, code=" << PlayM4_GetLastError(handle_port_map_[RealPlayHandle]);
				break;
			}
			//LOG(INFO) << " - play success";

		}
		break;
	case NET_DVR_STREAMDATA:   //码流数据
		if (dwBufSize > 0) {
			//if (!PlayM4_InputData(handle_port_map_[RealPlayHandle], pBuffer, dwBufSize)) {
			//	LOG(ERROR) << "input data error, code=" << PlayM4_GetLastError(handle_port_map_[RealPlayHandle]);
			//	break;
			//}
			BOOL inData = PlayM4_InputData(handle_port_map_[RealPlayHandle], pBuffer, dwBufSize);
			while (!inData)
			{
				sleep(1);
				inData = PlayM4_InputData(handle_port_map_[RealPlayHandle], pBuffer, dwBufSize);
				LOG(ERROR) << "PlayM4_InputData failed ";
			}
		}
		break;
	default: //其他数据
		//            LOG(INFO)<<"other data";
		if (dwBufSize > 0) {
			if (!PlayM4_InputData(handle_port_map_[RealPlayHandle], pBuffer, dwBufSize)) {
				LOG(ERROR) << "input other data error, code=" << handle_port_map_[RealPlayHandle];
				break;
			}
		}
		break;
	}
}
void HikCapture::g_DecCBFun(LONG nPort, char *pBuf, LONG nSize, FRAME_INFO *pFrameInfo, void* nReserved1, LONG nReserved2)
{
	//Not ready for Start!
	if (!CameraManager::Instance()->CameraRunning()) return;

	//Run!!!
	long lFrameType = pFrameInfo->nType;
	int cameraID = port_cameraID_map_[nPort];
	if (lFrameType == T_YV12)
	{
		cv::Mat src(pFrameInfo->nHeight + pFrameInfo->nHeight / 2, pFrameInfo->nWidth, CV_8UC1, (uchar *)pBuf);
		cv::cvtColor(src, src, cv::COLOR_YUV2BGR_YV12);
		CameraManager::Instance()->InsertMatBuf(cameraID, src);
	}
	usleep(10000);
}
void HikCapture::g_ExceptionCallBack(DWORD dwType, LONG lUserID, LONG lHandle, void *pUser) {
	char tempbuf[256] = { 0 };
	switch (dwType) {
		case
		EXCEPTION_RECONNECT:    //预览时重连
			LOG(ERROR) << "----------reconnect--------" << time(NULL);
			break;
		default:
			break;
	}
}


