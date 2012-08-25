
#ifndef DownloadObject_h__
#define DownloadObject_h__

#include "TypeDefs.h"
#include "Object.h"

namespace q {

struct IDownloadObject : Object {
	virtual uint64 get_downloaded() = 0;
	virtual uint64 get_size() = 0;
	virtual const char* get_actual_url() = 0;
	virtual const char* get_url() = 0;
};

struct IDownloadEvent : Object {
	virtual void OnBegin() = 0;
	virtual void OnEnd(int code) = 0;
};

} // namespace q

#endif // DownloadObject_h__