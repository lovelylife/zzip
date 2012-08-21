
#ifndef Downloaders_h__
#define Downloaders_h__

#include "Object.h"
#include "Thread.h"
#include "DownloadController.h"
#include <vector>
#include <list>
#include "RefCounted.h"
#include "SemaphoreQueue.h"

namespace q {

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

IDownloaders* create_downloaders();

} // namespace q

#endif // Downloaders_h__