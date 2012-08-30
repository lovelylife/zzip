
#ifndef DownloadObject_h__
#define DownloadObject_h__

#include "TypeDefs.h"
#include "Object.h"

namespace q {

//class IHttpObject : public Object {
//public:
// 	virtual const char* url() = 0;
// 	virtual const char* header(const char*) = 0;
// 	virtual int status() = 0;
// 	virtual ~IHttpObject() {};
//};

class IHttpResponse : public Object {
public:
	virtual const char* url() = 0;
	virtual int status() = 0;
	virtual const char* header(const char*) = 0;
	virtual uint64 size() = 0;
	virtual ~IHttpResponse() {}
};

class IDownloadObject : public IHttpResponse {
public:
	virtual uint64 downloaded_size() = 0;
	virtual const char* actual_url() = 0;
	virtual bool completed() = 0;
	virtual ~IDownloadObject() {}
};

class IRequestObject : public IHttpResponse {
public:
	virtual const char* get_content() = 0;
	virtual ~IRequestObject() {}
};

} // namespace q

#endif // DownloadObject_h__