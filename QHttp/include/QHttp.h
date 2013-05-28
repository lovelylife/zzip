
#ifndef QHttp_h__
#define QHttp_h__

#include "Object.h"

namespace q {

enum READYSTATE {
	UNINITIALIZED = 0,
	LOADING,
	LOADED,
	INTERACTIVE,
	COMPLETED
};

enum ACTION {
	GET = 0,
	POST,
};

struct IHttpRequest : Object {
	virtual void open(const char* url, ACTION action) = 0;
	virtual void addRequestHeader(const char* name, const char* value) = 0;
	virtual void cookie(const char* name, const char* cookies) = 0;
	virtual void onReadyStateChange(READYSTATE state) = 0;
};

class IHttp : public Object {
public:
	virtual bool send(IHttpRequest*) = 0;
};

IHttp* create(int thread_number);

} // namespace q

#endif // QHttp_h__