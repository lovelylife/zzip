

#include "ThreadPool.h"

namespace q {

WorkThread::WorkThread( ThreadPool* pool )
: pool_(pool)
{

}

WorkThread::~WorkThread() {}

bool WorkThread::loop() { 
	if(NULL != pool_) {
		return pool_->work_proc();
	}

	return false;
}


////////////////////////////////////////////////////////////
// class ThreadPool
////////////////////////////////////////////////////////////

ThreadPool::ThreadPool( int num ) : threads_num(num)
{

}

ThreadPool::~ThreadPool()
{

}

bool ThreadPool::run()
{
	for(int i=0; i < threads_num; i++) {
		WorkThread* p = new WorkThread(this);
		threads.push_back(p);
		p->start();
	}
	return true;
}

void ThreadPool::stop()
{
	for(size_t i=0; i < threads.size(); i++) {
		WorkThread* p = threads[i];
		if(p) {
			p->timedwait_stop();
		}
	}
}

bool ThreadPool::newtask( ITask* task, int time /*= -1*/, int times /*= 0*/ )
{
	return task_queue_.push(task);
}

bool ThreadPool::work_proc()
{
	RefPtr<ITask> pTask;
	while(task_queue_.pop(pTask)) {
		pTask->work();
		pTask->onfinish();
	}
	return true;
}


////////////////////////////////////////////////////////////
// class ThreadPoolManager
////////////////////////////////////////////////////////////





ThreadPoolManager::ThreadPoolManager()
{
	number_of_thread = 5;
}

ThreadPoolManager::ThreadPoolManager( int thread_num ) : number_of_thread(thread_num)
{
	if(number_of_thread <= 0) {
		number_of_thread = 5;
	}
}

bool ThreadPoolManager::run()
{
	thread_pool_ = new ThreadPool(number_of_thread);
	return thread_pool_->run();
}

bool ThreadPoolManager::newtask( ITask* task, int time /*= -1*/, int times /*= 0*/ )
{
	//assert(NULL != thread_pool_);
	return thread_pool_->newtask(task, time, times);
}
} // namespace q