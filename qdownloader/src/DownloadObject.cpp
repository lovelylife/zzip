
#include "DownloadObject.h"
#include <string>

namespace q {

class DownloadObject : public IDownloadObject {
public:
	static DownloadObject* create(const char* sUrl, const char* sSavePath) {
		if(NULL == sUrl || NULL == sSavePath) {
			return NULL;
		}

		DownloadObject* p = new DownloadObject;
		p->url_       = sUrl;
		p->save_path_ = sSavePath;

		return p;
	}

public:
	uint64 get_downloaded() { return 0; }
	uint64 get_size() { return 0; }
	const char* get_actual_url() { return ""; }
	const char* get_url() { return ""; }

private:
	DownloadObject(void) {

	}

	~DownloadObject(void) {

	}

private:
	std::string url_;
	std::string actual_url_;
	std::string save_path_;
	uint64 file_size_;
	uint64 downloaded_size_;

}; // class DownloadObject

IDownloadObject* 
downloadobject_createinstance( const char* sUrl, const char* sSavePath ) {
	return DownloadObject::create(sUrl, sSavePath);
}

} // namespace q