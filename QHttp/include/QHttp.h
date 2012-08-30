
#ifndef QHttp_h__
#define QHttp_h__

#include "QHttpController.h"

namespace q {

class Http : public Object {
public:
	virtual long request (const char* sUrl, IRequestController*);
	virtual long download(const char* sUrl, const char* sSavePath, IDownloadController* control) = 0;
};

Http* create_qhttp(int thread_number);

} // namespace q

#endif // QHttp_h__