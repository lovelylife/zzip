
// 线程基本操作的封装

#ifndef __THREADS_H__
#define __THREADS_H__
#include "../base/builder_config.h"

#ifndef OS_WIN
	#include <pthread.h>
	#include <time.h>
#else
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <process.h>
#include "typedefs.h"
#endif

namespace base {

class Thread {
public:

	// 标准构造和析构函数
	Thread();
	~Thread();

	//! 启动线程
	virtual bool start();

	//! 停止线程
	virtual bool stop();
		
	// 判断线程是否在运行
	bool IsRunning() const { return bRunning_;}
		
	// 等待线程停止
	bool WaitThreadEnd(unsigned long timeout = -1);

	// 线程句柄
#ifdef OS_WIN
	typedef HANDLE pthread_t;
	DWORD dwThreadId;
#endif
	pthread_t hThread;

protected:

	// 线程循环，在线程空间内执行,需要由子类实现
	// return
	// true 线程将继续循环调用ThreadLoop(),
	// false 线程将停止
	virtual bool ThreadLoop() = 0;
		
	//! 等待线程停止
	//	调用后将设置为等待停止状态,并阻塞调用线程,直到CBaseThread线程停止
	virtual bool WaitForStop(unsigned long timeout = -1); 
		
	// 线程开始时被调用
	// 在线程空间内执行,线程开始时被调用
	virtual void OnBegin(){}

	// 线程停止时被调用
	// 在线程空间内执行,线程停止时被调用
	virtual void OnExit(){}

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

private:
	DISALLOW_COPY_AND_ASSIGN(Thread);
}; // class Thread

} // namespace end

#endif
