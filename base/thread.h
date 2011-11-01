
// �̻߳��������ķ�װ

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

	// ��׼�������������
	Thread();
	~Thread();

	//! �����߳�
	virtual bool start();

	//! ֹͣ�߳�
	virtual bool stop();
		
	// �ж��߳��Ƿ�������
	bool IsRunning() const { return bRunning_;}
		
	// �ȴ��߳�ֹͣ
	bool WaitThreadEnd(unsigned long timeout = -1);

	// �߳̾��
#ifdef OS_WIN
	typedef HANDLE pthread_t;
	DWORD dwThreadId;
#endif
	pthread_t hThread;

protected:

	// �߳�ѭ�������߳̿ռ���ִ��,��Ҫ������ʵ��
	// return
	// true �߳̽�����ѭ������ThreadLoop(),
	// false �߳̽�ֹͣ
	virtual bool ThreadLoop() = 0;
		
	//! �ȴ��߳�ֹͣ
	//	���ú�����Ϊ�ȴ�ֹͣ״̬,�����������߳�,ֱ��CBaseThread�߳�ֹͣ
	virtual bool WaitForStop(unsigned long timeout = -1); 
		
	// �߳̿�ʼʱ������
	// ���߳̿ռ���ִ��,�߳̿�ʼʱ������
	virtual void OnBegin(){}

	// �߳�ֹͣʱ������
	// ���߳̿ռ���ִ��,�߳�ֹͣʱ������
	virtual void OnExit(){}

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

private:
	DISALLOW_COPY_AND_ASSIGN(Thread);
}; // class Thread

} // namespace end

#endif
