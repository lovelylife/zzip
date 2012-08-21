
#include "Downloaders.h"
#include <list>
#include "RefCounted.h"
#include "DownloadObject.h"
#include "ThreadPool.h"


namespace q {

//////////////////////////////////////////////////////////////////////////
// class Downloaders
//////////////////////////////////////////////////////////////////////////
	
class Downloaders : public IDownloaders {
// IDownloaders 接口
public:
	void initialize() {}
	long create_task(const char* sUrl, const char* sSavePath = NULL, IDownloadController* controller = NULL) {
		RefPtr<IDownloadObject> downobject = downloadobject_createinstance(sUrl, sSavePath);
		if(NULL != downobject) {
			if(NULL != controller) {
				controller->OnAttach(downobject);
			}
		}
		
		return 0;
	}

	void remove_task(const char* sUrl) {

	}

public:
	Downloaders(void) {};
	~Downloaders(void) {};

private:
	std::list< RefPtr<IDownloadObject> > queue_;
	ThreadPoolManager ThreadPoolManager_;	
};


// create downloaders object 
IDownloaders* create_downloaders() {
	return(new Downloaders());
}

} // namespace q