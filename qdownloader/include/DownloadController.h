
#ifndef DownloadController_h__
#define DownloadController_h__

#include "Object.h"
#include "DownloadObject.h"

namespace q {

struct IDownloadController : Object {
	virtual void OnAttach(IDownloadObject*) = 0;
	virtual void OnDettach() = 0;
	virtual void OnStatusChanged(int) = 0;
};

} // namespace q

#endif // Downloaders_h__