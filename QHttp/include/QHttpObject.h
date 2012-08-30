
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

class IHttpResponse : virtual public Object {
public:
	virtual const char* url() = 0;
	virtual int status() = 0;
	virtual const char* header(const char*) = 0;
	virtual ~IHttpResponse() {}
};

class IDownloadObject : public IHttpResponse {
public:
	virtual const char* actual_url() = 0;
	virtual uint64 file_size() = 0;
	virtual uint64 downloaded_size() = 0;	
	virtual ~IDownloadObject() {}
};

class IRequestObject : public IHttpResponse {
public:
	virtual const char* content() = 0;
	virtual ~IRequestObject() {}
};

} // namespace q

#endif // DownloadObject_h__