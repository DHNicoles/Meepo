#ifndef sync_h__
#define sync_h__

/*******************************************************************************/
/*Sync tool for sending data to JD-Cloud                                       */
/*******************************************************************************/

#include "util/util.hpp"
#include "util/singleton.h"
class HttpConnect;
class Sync : public Singleton<Sync>
{
    public:
        void OnInit();
        void OnDestory();
        void IncreseRecord() { ++period_num_; }
        void ClearRecord() { period_num_ = 0; }
        ~Sync();	
    private:
        //////////////////////////
        //Send traits data to DB//
        //////////////////////////
        void* SendProc(/*void* param*/); 
		void Post(std::string& period_time_be, std::string& period_time_en, int num);
        std::atomic_bool send_proc_continue_;
    private:
        std::shared_ptr<HttpConnect> http_ptr_;	
        std::atomic_int period_num_;

};

//线程启动函数，声明为模板函数 
typedef void* (Sync::*SyncProc)(); 
    template <typename TYPE, SyncProc RunThread>  
void* _thread_t(void* param) 
{     
    pthread_detach(pthread_self());
    TYPE* This = (TYPE*)param;     
    (This->*RunThread)();     
    return NULL;  
}  



#endif // sync_h__
