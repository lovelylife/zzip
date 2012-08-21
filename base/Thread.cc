
#include "thread.h"

#ifndef WIN32
#include <pthread.h>
#include <time.h>
#else
#include <process.h>
#endif

namespace base {

Thread::Thread() 
: thread_(0)
, bRunning_(false)
, bWaitStop_(false)
{

}

Thread::~Thread(){}

bool Thread::start(function&){
	if(!bRunning_) {
#ifdef OS_WIN
		thread_ = (HANDLE)_beginthreadex( 0, 0, &ThreadFunc, this, 0, (unsigned*)&thread_id_ );
		if (thread_ == 0) return false;
#else
		if (0 != pthread_create(&thread_, NULL, &ThreadFunc, this)) {
			thread_ = 0;
			return false;
		}
#endif
	}	
	return bRunning_;
}

bool Thread::begin_waitstop(unsigned long timeout) {
	if(bRunning_) {
		bWaitStop_ = true;
#ifdef OS_WIN
		return WaitForSingleObject(thread_,timeout) == WAIT_OBJECT_0;
#else
		pthread_join(thread_, NULL);
#endif
	}
	return true;
}

bool Thread::timedwait_stop(unsigned long timeout) {
	if(bRunning_) {
#ifdef OS_WIN
		return (WaitForSingleObject(thread_,timeout) == WAIT_OBJECT_0);
#else
		pthread_join(thread_, NULL);
#endif
	}
	return true;
}

bool Thread::stop() {
	if(bRunning_) {
		WaitForStop();
#ifdef OS_WIN
		CloseHandle(thread_);
#endif

		thread_ = 0;
		bRunning_ = false;
	}
	return true;
}

#ifdef OS_WIN
unsigned __stdcall Thread::ThreadFunc( void* pArguments )
#else
void* Thread::ThreadFunc( void* pArguments )
#endif

{
	Thread* this_ = (Thread*)pArguments;
	this_->bRunning_ = true;
	this_->bWaitStop_ = false;
	unsigned exitCode = 0;
	this_->OnBegin();
	while(!this_->bWaitStop_){
		if(!func()){
			exitCode = 0;
			break;
		}
	}
	this_->OnEnd();
#ifdef OS_WIN
	return exitCode;
#else
	return 0;
#endif

} 

void Thread::OnBegin()
{

}

void Thread::OnEnd()
{

}

bool Thread::is_running() const
{
	return bRunning_;
}
} // namespace end