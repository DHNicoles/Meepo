#include "game/sync.hpp"
#include "util/http_connect.hpp"

///////////////////////////////
//interval(sec) for sending ///
///////////////////////////////
const int l_send_interval = 2;//sec 

void Sync::OnInit()
{
    period_num_ = 0;
    http_ptr_.reset(new HttpConnect());
    send_proc_continue_ = true;
    pthread_t id_send;
    int ret = pthread_create(&id_send, NULL, _thread_t<Sync, &Sync::SendProc>, this);  
    CHECK_EQ(ret, 0) << " - Http connection established failed!";
    LOG(INFO) << " - Http connection established successfully!";
}

void Sync::OnDestory()
{
    send_proc_continue_ = false;
}

Sync::~Sync()
{
    OnDestory();
}

void* Sync::SendProc(/*void* param*/)
{
	std::string period_time_be;
	std::string period_time_en;
    while(send_proc_continue_)
    {
		period_time_be = getSystemTime(); 
        //sleep for a while
        std::this_thread::sleep_for(std::chrono::milliseconds(l_send_interval * 1000));
		period_time_en = getSystemTime(); 

        /////////
        //send //
        /////////
		int num = period_num_; 
		ClearRecord();
       	Post(period_time_be, period_time_en, num); 
    }
    LOG(INFO) << " - SendProc-Exit";

}

void Sync::Post(std::string& period_time_be, std::string& period_time_en, int num)
{
	std::string time_be_str = "{\"time_begin\":\"" + period_time_be + "\",";
	std::string time_en_str = "\"time_end\":\"" + period_time_en+ "\",";
	std::string period_num_str = "\"number\":\"" + std::to_string(num) + "\"}";

	std::string param = time_be_str + time_en_str + period_num_str;
	LOG(INFO) << " - [SYNC-INFO] " << param;
	//if(num > 0) HttpConnect::Instance()->PostData("aitag-local.jd.com", "/shop/outStoreUploadShelfData.action", param.c_str());
}
