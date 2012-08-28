
#ifndef DownloadController_h__
#define DownloadController_h__

#include "Object.h"
#include "QHttpObject.h"

namespace q {

struct IDownloadController : Object {
	virtual void OnAttach(IDownloadObject*) = 0;
	virtual void OnDettach() = 0;
};

struct IRequestController : Object {
	virtual void OnFinish(IRequestObject*);
};

} // namespace q

#endif // Downloaders_h__