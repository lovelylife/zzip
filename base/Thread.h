
// 线程基本操作的封装

#ifndef __THREADS_H__
#define __THREADS_H__
#include "../base/builder_config.h"
#include "typedefs.h"
#ifndef OS_WIN
	#include <pthread.h>
	#include <time.h>
#else
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <process.h>
#endif

namespace base {


template<class TFunc, class result_type>
struct function {
	result_type operator()() {
		return TFunc();
	}
};


class Thread {
public:
	//! 启动线程
	virtual bool start(function<>&);

	//! 停止线程
	virtual bool stop();
		
	// 判断线程是否在运行
	bool running() const { return bRunning_;}
		
	// 等待线程停止
	bool timedwait_stop(unsigned long timeout = -1);

// 标准构造和析构函数
public:
	Thread();
	~Thread();

public:
// 线程句柄
#ifdef OS_WIN
	typedef HANDLE pthread_t;
	DWORD dwThreadId;
#endif
	pthread_t hThread;

protected:
	//! 等待线程停止
	//	调用后将设置为等待停止状态,并阻塞调用线程,直到线程停止
	virtual bool begin_waitstop(unsigned long timeout = -1); 
		
	// 线程开始时被调用
	// 在线程空间内执行,线程开始时被调用
	virtual void begin(){}

	// 线程停止时被调用
	// 在线程空间内执行,线程停止时被调用
	virtual void end(){}

	// 等待停止
	volatile bool bWaitStop_;

	// 是否在运行
	volatile bool bRunning_;

	// 线程回调函数
#ifdef OS_WIN
	static unsigned __stdcall ThreadFunc( void* pArguments );
#else
	static void* ThreadFunc( void* pArguments );
#endif

	function func;

private:
	DISALLOW_COPY_AND_ASSIGN(Thread);

}; // class Thread

} // namespace base

#endif
