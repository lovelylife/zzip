

#ifndef _SYNC_H
#define _SYNC_H

#ifdef WIN32
	#include <windows.h>
	typedef HANDLE sem_t;
#else
	#include <pthread.h>
	#include <semaphore.h>
#endif

class Semaphore {
// ��������
public:
	Semaphore();
	~Semaphore();

// ����
public:
	void down();
	void up();

private:
	sem_t sem;
};


#ifdef WIN32

inline Semaphore::Semaphore() {
	sem = CreateSemaphore(NULL, 0, 8192, NULL);
}

inline Semaphore::~Semaphore() {
	if (sem) CloseHandle(sem);
}

inline void Semaphore::down() {
	WaitForSingleObject(sem, INFINITE);
}

inline void Semaphore::up() {
	ReleaseSemaphore(sem, 1, NULL);
}

#else		// NO-WIN32

inline Semaphore::Semaphore() {
	sem_init(&sem, 0, 0);
}

inline Semaphore::~Semaphore() {
	sem_destroy(&sem);
}

inline void Semaphore::down() {
	sem_wait(&sem);
}

inline void Semaphore::up() {
	sem_post(&sem);
}

#endif		// WIN32

#endif