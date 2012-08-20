
#ifndef Downloaders_h__
#define Downloaders_h__

#include "Object.h"
#include "Thread.h"
#include "DownloadController.h"
#include <vector>
#include <list>
#include "RefCounted.h"

namespace q {



struct ITask : Object {
	virtual bool work() = 0;
	virtual bool onfinish() = 0;
};

class WorkThread : public base::Thread {
protected:
	bool loop() { return true; }
};

class ThreadPool {
public:
	ThreadPool(int num) : threads_num(num) {
		
	}

public:
	bool run() {
		for(int i=0; i < threads_num; i++) {
			WorkThread* p = new WorkThread();
			threads.push_back(p);
			p->start(function(this, ThreadPool::work_proc));
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

	bool newtask(ITask*, int time = -1, int times = 0) {
		return true;
	}

private:
	bool work_proc() {
		return true;
	}

private:
	int threads_num;
	std::vector<WorkThread*> threads;
	SemaphoreQueue< RefPtr<ITask> > task_queue_;	
};

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