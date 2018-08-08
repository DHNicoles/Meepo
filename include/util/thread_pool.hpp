#ifndef thread_pool_h__
#define thread_pool_h__

/**********************************************
*Project:TinyThreadPool
*
*Author:lizhen
*email:lizhen_ok@aliyun.com
*
***********************************************/
#include "util/util.hpp"
#include "util/singleton.h"

namespace ice 
{
	class Task
	{
	public:
		Task(void* arg = NULL, const std::string taskName = "")
			: arg_(arg)
			, taskName_(taskName)
		{
		}
		virtual ~Task()
		{
		}
		void setArg(void* arg)
		{
			arg_ = arg;
		}

		virtual int run() = 0;

	protected:
		void*       arg_;
		std::string taskName_;
	};


	class ThreadPool : public Singleton<ThreadPool>
	{
	public:
		ThreadPool(int threadNum = 10);
		~ThreadPool();

	public:
		size_t addTask(Task *task);
		void   stop();
		int    size();
		void   start();
		Task*  take();

	private:
		int createThreads();
		static void* threadFunc(void * threadData);

	private:
		ThreadPool& operator=(const ThreadPool&);
		ThreadPool(const ThreadPool&);

	private:
		volatile  bool              isRunning_;
		int                         threadsNum_;
		pthread_t*                  threads_;

		std::deque<Task*>           taskQueue_;
		pthread_mutex_t             mutex_;
		pthread_cond_t              condition_;
	};
}
#endif // thread_pool_h__
