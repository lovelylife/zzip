
#ifndef Downloaders_h__
#define Downloaders_h__

namespace q {

class IDownloadeHandler;

class IDownloaders {
public:
	// ��ʼ��
	virtual void initialize() = 0;

	// ������񣬷���ֵΪ0��ʾ�ɹ�������Ϊ�����
	virtual long create_task(const char* sUrl, IDownloadeHandler* control, const char* sSavePath) = 0;
};

static IDownloaders* CreateDownloaders();

} // namespace q

#endif // Downloaders_h__