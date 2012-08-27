
#ifndef DownloadObject_h__
#define DownloadObject_h__

#include "TypeDefs.h"
#include "Object.h"

namespace q {

struct IHttpObject : Object {
	virtual const char* header(const char*) = 0;
	virtual int status(int) = 0;
};

struct IDownloadObject : Object {
	virtual uint64 get_downloaded() = 0;
	virtual uint64 get_size() = 0;
	virtual const char* get_actual_url() = 0;
	virtual const char* get_url() = 0;
};

struct IRequestObject : Object {
	virtual const char* get_content() = 0;
	virtual const char* get_header(const char*) = 0;
};

} // namespace q

#endif // DownloadObject_h__