
#include "Downloaders.h"
#include <boost/shared_ptr.hpp>
#include <list>

namespace q {

class IDownloadObject;

//////////////////////////////////////////////////////////////////////////
// class Downloaders
// �ļ����ع�����
//////////////////////////////////////////////////////////////////////////
class Downloaders : public IDownloaders {
// IDownloaders �ӿ�
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