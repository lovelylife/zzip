
#ifndef DownloadeHandler_h__
#define DownloadeHandler_h__

namespace q {

struct IDownloadeHandler 
{
	// װ��
	virtual void onattach(IDownloadObject*) = 0;
	// ж��
	virtual void ondettach() = 0;
	// ״̬�仯
	virtual void onstatuschanged(int) = 0;
};

} // namespace q

#endif // Downloaders_h__