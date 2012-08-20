
// �̻߳��������ķ�װ

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
	//! �����߳�
	virtual bool start(function<>&);

	//! ֹͣ�߳�
	virtual bool stop();
		
	// �ж��߳��Ƿ�������
	bool running() const { return bRunning_;}
		
	// �ȴ��߳�ֹͣ
	bool timedwait_stop(unsigned long timeout = -1);

// ��׼�������������
public:
	Thread();
	~Thread();

public:
// �߳̾��
#ifdef OS_WIN
	typedef HANDLE pthread_t;
	DWORD dwThreadId;
#endif
	pthread_t hThread;

protected:
	//! �ȴ��߳�ֹͣ
	//	���ú�����Ϊ�ȴ�ֹͣ״̬,�����������߳�,ֱ���߳�ֹͣ
	virtual bool begin_waitstop(unsigned long timeout = -1); 
		
	// �߳̿�ʼʱ������
	// ���߳̿ռ���ִ��,�߳̿�ʼʱ������
	virtual void begin(){}

	// �߳�ֹͣʱ������
	// ���߳̿ռ���ִ��,�߳�ֹͣʱ������
	virtual void end(){}

	// �ȴ�ֹͣ
	volatile bool bWaitStop_;

	// �Ƿ�������
	volatile bool bRunning_;

	// �̻߳ص�����
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
