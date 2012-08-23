
#include "Downloaders.h"
#include "RefCounted.h"
#include "DownloadObject.h"
#include "ThreadPool.h"

#include <list>
#include <curl/curl.h>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <map>

void static trim_right(std::string& s) {
	s.erase(s.find_last_not_of(' ')+1);
}

void static trim_left(std::string& s) {
	s.erase(s.find_first_not_of(' ')+1);
}

namespace q {

//////////////////////////////////////////////////////////////////////////
// class DownloadObject
//////////////////////////////////////////////////////////////////////////
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

	bool set_filesize(size_t size) 
	{
		downloaded_size_ = 0; 
		file_size_ = size;

		if(fstream_.is_open()) { 
			fstream_.clear();
			fstream_.close();
		}

		std::string sFile = save_path_;
		// 	int pos = sFile.find_last_of('\\');
		// 	int pos1 = sFile.size() - 1;
		// 	if(	(sFileName_.find_first_of('/') != 0) 
		// 		|| (sFileName_.find_first_of('\\') != 0)
		// 		|| (sFile.find_last_of('\\') != (sFile.length()-1))
		// 		|| (sFile.find_last_of('/') != (sFile.length()-1))) {
		// 		sFile += "/";
		// 	}
		// 
		// 	sFile += sFileName_;	
		std::replace(sFile.begin(), sFile.end(), '\\', '/');

		// 打开文件
		fstream_.open(sFile.c_str(), std::ios::binary);

		if(!fstream_.is_open()) {
			std::ostringstream os;
			os << "open file failed [file://" << sFile << "]" << std::endl;
			//OnError(-3, os.str().c_str());
			return false;
		}
		return true; 
	}

	size_t write_data(char* buffer, size_t size) {
		downloaded_size_ += size;
		printf("downloaded size: %d\r\n", downloaded_size_);
		if(!fstream_.fail()) {
			fstream_.write(buffer, size);
		}

		return fstream_.tellp();
	}

	void write_header(const std::string& name, const std::string& value) {
		if(!name.empty()) {
			headers_[name] = value;
		}
	}

	uint32 status() { return status_code_; }

// 接口IDownloadObject
public:
	uint64 get_downloaded() { return downloaded_size_; }
	uint64 get_size() { return file_size_; }
	const char* get_actual_url() { return actual_url_.c_str(); }
	const char* get_url() { return url_.c_str(); }

protected:
	DownloadObject(void) 
	: url_("")
	, actual_url_("")
	, save_path_("")
	, file_size_(0)
	, downloaded_size_(0)
	{

	}

public:
	~DownloadObject(void) {

	}

private:
	std::string url_;
	std::string actual_url_;
	std::string save_path_;
	uint64 file_size_;
	uint64 downloaded_size_;
	std::ofstream fstream_;
	uint32 status_code_;
	std::map<std::string, std::string> headers_;

}; // class DownloadObject

//////////////////////////////////////////////////////////////////////////
// class DownloadControllerProxy
//////////////////////////////////////////////////////////////////////////
class DownloadControllerProxy : public Object {
// static method
public:
	static DownloadControllerProxy* create(const char* sUrl, const char* sSavePath, IDownloadController* controller) {
		DownloadControllerProxy* p = new DownloadControllerProxy;
		p->download_object_ = DownloadObject::create(sUrl, sSavePath);
		p->controller_ = controller;
		if(p->controller_) {
			p->controller_->OnAttach(p->download_object_);
		}

		return p;
	}

// 代理方法
public:
	size_t write_data(char* buffer, size_t size) {
		return download_object_->write_data(buffer, size);
	}

	size_t write_header(char* buffer, size_t size) {
		// parse header info
		std::string header(buffer, size);
		size_t pos = header.find_first_of(':');
		if(pos != std::string::npos) {
			std::string tmp = header.substr(0, pos-1);
			trim_left(tmp);
			download_object_->write_header(tmp, "2");
		} else {
			// get status code
			std::string http_version;
			size_t blank_pos = header.find(' ');
			if(blank_pos != std::string::npos) {
				// http version

			}
		}
		
		return size;
	}

	const char* get_url() {
		return download_object_->get_url();
	}

public:
	DownloadControllerProxy() 
	: controller_(NULL)
	, download_object_(NULL)
	{

	}

	~DownloadControllerProxy() {}

private:
	RefPtr<IDownloadController> controller_;
	RefPtr<DownloadObject> download_object_;
};



//////////////////////////////////////////////////////////////////////////
// class Downloaders
//////////////////////////////////////////////////////////////////////////

class Downloaders;

class DownloadTask : public ITask {
public:
	DownloadTask(DownloadControllerProxy* c, Downloaders* d) 
	: controller_proxy(c) 
	, downloaders(d)
	{
		
	}

	~DownloadTask() {}

	static size_t recv_header(void *ptr, size_t size,size_t nmemb, void *userdata) {
		DownloadTask* this_ = static_cast<DownloadTask*>(userdata);
		this_->OnRecvHeaderLine(ptr, size * nmemb);
		return (size * nmemb);
	}

	static size_t recv_data(void *ptr, size_t size,size_t nmemb, void *userdata) {
		DownloadTask* this_ = static_cast<DownloadTask*>(userdata);
		this_->OnRecvData(ptr, size * nmemb);
		return size * nmemb;
	}

	bool Task() {
		if(controller_proxy) {
			const char* url = controller_proxy->get_url();
			if(NULL != url) {
				CURL *curl;
				CURLcode res;
				curl = curl_easy_init();
				curl_easy_setopt(curl, CURLOPT_URL, url);
				curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
				curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &DownloadTask::recv_header);
				curl_easy_setopt(curl, CURLOPT_HEADERDATA, this);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  &DownloadTask::recv_data);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
				
				res = curl_easy_perform(curl); /* ignores error */ 
				if(res != CURLE_OK)
					fprintf(stderr, "curl_easy_perform() failed: %s\n",	curl_easy_strerror(res));
				curl_easy_cleanup(curl);
			}			
		}
		
		return true;
	}

	void OnFinish() {

	}

	size_t OnRecvData(void *ptr, size_t size) {
		return controller_proxy->write_data((char*)ptr, size);
	}

	size_t OnRecvHeaderLine(void *ptr, size_t size) {
		return controller_proxy->write_header((char*)ptr, size);
	}

private:
	RefPtr<DownloadControllerProxy> controller_proxy;
	Downloaders* downloaders;
};

class Downloaders : public IDownloaders {
// IDownloaders 接口
public:
	void initialize(int thread_num) {
		/* Must initialize libcurl before any threads are started */ 
		curl_global_init(CURL_GLOBAL_ALL);

		thread_pool_manager_.run(thread_num);
	}

	long create_task(const char* sUrl, const char* sSavePath, IDownloadController* controller) {
		RefPtr<DownloadControllerProxy> controller_proxy = DownloadControllerProxy::create(sUrl, sSavePath, controller);
		if(NULL != controller_proxy) {
			queue_.push_back(controller_proxy);
			if(!thread_pool_manager_.newtask(new DownloadTask(controller_proxy, this), -1, 1)) {
				// create task failed.
				return -1;
			}
		}
		
		return 0;
	}

	void remove_task(const char* sUrl) {

	}

public:
	Downloaders(void) {};
	~Downloaders(void) {};

private:
	std::list< RefPtr<DownloadControllerProxy> > queue_;
	ThreadPoolManager thread_pool_manager_;	
};


// create downloaders object 
IDownloaders* create_downloaders() {
	return(new Downloaders());
}

} // namespace q