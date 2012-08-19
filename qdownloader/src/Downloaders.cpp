
#include "Downloaders.h"
#include <list>
#include "RefCounted.h"
#include "DownloadObject.h"


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
				controller->onattach(downobject);
			}
		}
		return 0;
	} 



public:
	Downloaders(void) {};
	~Downloaders(void) {};

private:
	std::list< RefPtr<IDownloadObject> > queue_;
	Tr
};


// create downloaders object 
static IDownloaders* create_downloaders() {
	return(new Downloaders());
}

} // namespace q