
#ifndef Downloaders_h__
#define Downloaders_h__

#include "Object.h"
#include "DownloadController.h"

namespace q {

struct ITask : Object {
	virtual bool work() = 0;
	virtual bool onfinish() = 0;
};

class ThreadPool {

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
	}
private:
	ThreadPool* thread_pool_;
	int number_of_thread;
};


class Reactor {

	std::list<list> list;
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