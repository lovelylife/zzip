
//////////////////////////////////////////////////////////////////////////
// class Thread
// 线程基本操作的封装
//////////////////////////////////////////////////////////////////////////

#ifndef __THREADS_H__
#define __THREADS_H__

#include "../base/builder_config.h"
#include "typedefs.h"



namespace q {

struct ITask : Object {
	virtual bool work() = 0;
	virtual bool onfinish() = 0;
};

class ThreadPool;

class WorkThread 
	: public base::Thread 
{
public:
	WorkThread(ThreadPool* pool);
	~WorkThread();

protected:
	bool loop();

private:
	ThreadPool* pool_;
};

class ThreadPool {
public:
	bool run();
	bool stop();
	bool newtask(ITask* task, int time = -1, int times = 0);

public:
	ThreadPool(int num);
	~ThreadPool();
	friend class WorkThread;

private:
	bool work_proc();

private:
	int threads_num;
	std::vector<WorkThread*> threads;
	SemaphoreQueue< RefPtr<ITask> > task_queue_;	
};

class ThreadPoolManager {
public:
	bool run();
	bool newtask(ITask* task, int time = -1, int times = 0);

public:
	ThreadPoolManager();
	ThreadPoolManager(int thread_num);

private:
	ThreadPool* thread_pool_;
	int number_of_thread;
};

} // namespace q

#endif
