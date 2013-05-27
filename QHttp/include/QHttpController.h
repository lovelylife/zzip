
#ifndef DownloadController_h__
#define DownloadController_h__

#include "Object.h"
#include "QHttpObject.h"

namespace q {

enum READYSTATE {
	UNINITIALIZED = 0,
	LOADING,
	LOADED,
	INTERACTIVE,
	COMPLETED
};

struct IHttpRequest : Object {
	virtual void open(const char* url, const char* action) = 0;
	virtual void add_request_header(const char* name, const char* value) = 0;
	virtual void cookie(const char* name, const char* cookies) = 0;
	virtual void onreadystatechange(READYSTATE state) = 0;
};

} // namespace q

#endif // DownloadController_h__