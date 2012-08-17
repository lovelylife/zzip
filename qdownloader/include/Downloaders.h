
#ifndef Downloaders_h__
#define Downloaders_h__

namespace q {

class IDownloadeHandler;

class IDownloaders {
public:
	// 初始化
	virtual void initialize() = 0;

	// 添加任务，返回值为0表示成功，否则为错误号
	virtual long create_task(const char* sUrl, IDownloadeHandler* control, const char* sSavePath) = 0;
};

static IDownloaders* CreateDownloaders();

} // namespace q

#endif // Downloaders_h__