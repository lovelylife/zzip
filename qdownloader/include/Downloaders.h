
#ifndef Downloaders_h__
#define Downloaders_h__

#include "DownloadController.h"

namespace q {

class IDownloaders : public Object {
public:
	virtual void initialize(int thread_num) = 0;
	virtual long create_task(const char* sUrl, const char* sSavePath, IDownloadController* control) = 0;
	virtual void remove_task(const char* sUrl) = 0;
};

IDownloaders* create_downloaders();

} // namespace q

#endif // Downloaders_h__