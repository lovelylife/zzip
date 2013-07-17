
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
	GET = 0L,
	POST = 1L,
};

struct IHttpRequest;

struct IEvent : Object {
	virtual void onReadyStateChange(IHttpRequest*, READYSTATE state) = 0;
};

struct IHttpRequest : Object {
	virtual bool open(const char* url, ACTION action, IEvent* event, const char* save_path) = 0;
	virtual bool setRequestHeader(const char* name, const char* value) = 0;
	virtual void send(const char* data = NULL) = 0;
	virtual void escape(const std::string& sIn, std::string& sOut) = 0;
// response attribute
	virtual int status() = 0;
	virtual const char* responseText() = 0;
};

class IHttp : public Object {
public:
	virtual bool createHttpRequest(IHttpRequest**) = 0;
};

IHttp* create(int thread_number);

} // namespace q

#endif // QHttp_h__