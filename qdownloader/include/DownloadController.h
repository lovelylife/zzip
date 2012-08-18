
#ifndef DownloadController_h__
#define DownloadController_h__

#include "Object.h"
#include "DownloadObject.h"

namespace q {

struct IDownloadController : Object {
	virtual void onattach(IDownloadObject*) = 0;
	virtual void ondettach() = 0;
	virtual void onstatuschanged(int) = 0;
};

} // namespace q

#endif // Downloaders_h__