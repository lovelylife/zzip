
#ifndef DownloadeHandler_h__
#define DownloadeHandler_h__

namespace q {

struct IDownloadeHandler 
{
	// 装载
	virtual void onattach(IDownloadObject*) = 0;
	// 卸载
	virtual void ondettach() = 0;
	// 状态变化
	virtual void onstatuschanged(int) = 0;
};

} // namespace q

#endif // Downloaders_h__