
//////////////////////////////////////////////////////////////////////////
// class Thread
// �̻߳��������ķ�װ
//////////////////////////////////////////////////////////////////////////

#ifndef Thread_h__
#define Thread_h__

#include "../base/builder_config.h"
#include "TypeDefs.h"

# ifndef OS_WIN
#	include <pthread.h>
#	include <time.h>
# else
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <process.h>
# endif

namespace q {

class Thread {
// ���÷���
public:
	virtual bool start();
	virtual bool stop();

// ����
public:
	bool running() const;

// ��׼�������������
public:
	Thread();
	~Thread();

// �߳̾��
public:
#  ifdef OS_WIN
	typedef HANDLE pthread_t;
	DWORD thread_id_;
#  endif
	pthread_t thread_;

// ��������
protected:
#  ifdef OS_WIN
	static unsigned __stdcall threadfunc( void* pArguments );
#  else
	static void* threadfunc( void* pArguments );
#  endif

	virtual bool loop() = 0;

	// �ȴ��߳�ֹͣ
	// ���ú�����Ϊ�ȴ�ֹͣ״̬,�����������߳�,ֱ���߳�ֹͣ
	virtual bool timedwait_stop(unsigned long timeout = -1); 

	// �߳̿�ʼʱ������
	// ���߳̿ռ���ִ��,�߳̿�ʼʱ������
	virtual void OnBegin();

	// �߳�ֹͣʱ������
	// ���߳̿ռ���ִ��,�߳�ֹͣʱ������
	virtual void OnEnd();

	// �ȴ�ֹͣ
	volatile bool bWaitStop_;
	// �Ƿ�������
	volatile bool bRunning_;

private:
	DISALLOW_COPY_AND_ASSIGN(Thread);

}; // class Thread

} // namespace base

#endif
