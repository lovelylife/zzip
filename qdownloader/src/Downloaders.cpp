
#include "Downloaders.h"
#include <boost/shared_ptr.hpp>
#include <list>

namespace q {

class IDownloadObject;

//////////////////////////////////////////////////////////////////////////
// class Downloaders
// 文件下载管理器
//////////////////////////////////////////////////////////////////////////
class Downloaders : public IDownloaders {
// IDownloaders 接口
public:
	void initialize() {}
	long create_task(const char* sUrl, IDownloadeHandler* controller = NULL, const char* sSavePath = NULL) {
		return 0;
	}

public:
	Downloaders(void) {};
	~Downloaders(void) {};

private:
	std::list< boost::shared_ptr<IDownloadObject> > queue_;
};


// create downloaders object 
static IDownloaders* CreateDownloaders() {
	return(new Downloaders());
}

} // namespace q