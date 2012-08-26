
#ifndef QHttp_h__
#define QHttp_h__

#include "DownloadController.h"

namespace q {

class Http : public Object {
public:
	virtual long Request (const char* sUrl, ICallBack*);
	virtual long Download(const char* sUrl, const char* sSavePath, IDownloadController* control) = 0;
};

Http* create_downloaders();

} // namespace q

#endif // QHttp_h__