
#ifndef Downloaders_h__
#define Downloaders_h__

#include "Object.h"
#include "Thread.h"
#include "DownloadController.h"
#include <vector>
#include <list>
#include "RefCounted.h"
#include "SemaphoreQueue.h"

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
	WorkThread(ThreadPool* pool) : pool_(pool) {
	
	}

	~WorkThread() {
	
	}

protected:
	bool loop();

private:
	ThreadPool* pool_;
};

class ThreadPool {

friend class WorkThread;

public:
	ThreadPool(int num) : threads_num(num) {
		
	}

public:
	bool run() {
		for(int i=0; i < threads_num; i++) {
			WorkThread* p = new WorkThread(this);
			threads.push_back(p);
			p->start();
		}
	}

	bool stop() {
		for(size_t i=0; i < threads.size(); i++) {
			WorkThread* p = threads[i];
			if(p) {
				p->timedwait_stop();
			}
		}
	}

	bool newtask(ITask* task, int time = -1, int times = 0) {
		return task_queue_.push(task);
	}

private:
	bool work_proc() {
		RefPtr<ITask> pTask;
		while(task_queue_.pop(pTask)) {
			pTask->work();
			pTask->onfinish();
		}
		return true;
	}

private:
	int threads_num;
	std::vector<WorkThread*> threads;
	SemaphoreQueue< RefPtr<ITask> > task_queue_;	
};


bool WorkThread::loop() { 
	if(NULL != pool_) {
		return pool_->work_proc();
	}

	return false;
}

class ThreadPoolManager {
public:
	ThreadPoolManager() {
		number_of_thread = 5;
	}

	ThreadPoolManager(int thread_num) 
	: number_of_thread(thread_num)
	{
		if(number_of_thread <= 0) {
			number_of_thread = 5;
		}
	}

	bool run() {
		thread_pool_ = new ThreadPool(number_of_thread);
		thread_pool_->run();
	}

	bool newtask(ITask* task, int time = -1, int times = 0) {
		//assert(NULL != thread_pool_);
		return thread_pool_->newtask(task, time, times);
	}

private:
	ThreadPool* thread_pool_;
	int number_of_thread;
};

class Task {

};

class Reactor {

	std::list<Task> list;
};

class IDownloaders : public Object {
public:
	virtual void initialize() = 0;
	virtual long create_task(const char* sUrl, const char* sSavePath, IDownloadController* control) = 0;
	virtual void remove_task(const char* sUrl);
};

static IDownloaders* create_downloaders();

} // namespace q

#endif // Downloaders_h__