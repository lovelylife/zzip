
#ifndef DownloadeHandler_h__
#define DownloadeHandler_h__

namespace q {

struct IDownloadeHandler 
{
	// ×°ÔØ
	virtual void onattach(IDownloadObject*) = 0;
	// Ð¶ÔØ
	virtual void ondettach() = 0;
	// ×´Ì¬±ä»¯
	virtual void onstatuschanged(int) = 0;
};

} // namespace q

#endif // Downloaders_h__