
#include "QHttp.h"
#include "RefCounted.h"
#include "QHttpObject.h"
#include "ThreadPool.h"

#include <list>
#include <curl/curl.h>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <map>
#include <iostream>

void static trim_right(std::string& s, const char* str = " ") {
	s.erase(s.find_last_not_of(str)+1);
}

void static trim_left(std::string& s, const char* str = " ") {
	s.erase(0, s.find_first_not_of(str));
}

namespace q {

class HttpManager;

class IHttpWriterObject : public Object {
	// 应答结果和http的版本字符串
	virtual void write_httpv_status(const char*, int)    = 0;
	virtual void write_header(char* buffer, size_t size) = 0;
	virtual void write_data  (char* buffer, size_t size) = 0;
};

//////////////////////////////////////////////////////////////////////////
// class DownloadObject
//////////////////////////////////////////////////////////////////////////
class DownloadObject : public IDownloadObject, virtual public IHttpWriterObject {
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

// IHttpResponse
public:
	const char* url() {
		return url_.c_str();
	}

	virtual int status(int s = 0) {
		if(s > 0) {
			status_code_ = s;
		}

		return status_code_;
	}

	const char* header(const char* name, const char* value) {

		if(name != NULL) {
			if(value != NULL) {
				headers_[std::string(name)] = value;
				return value;
			}
			
			std::map<std::string, std::string>::const_iterator cit = headers_.find(name);

			if(cit == headers_.end()) {
				return cit->second;
			}			
		}
		
		return "";
	}

	uint64 size(uint64 content_length = 0) {
		if(content_length > 0) {
			downloaded_size_ = 0; 
			file_size_ = size;

			if(fstream_.is_open()) { 
				fstream_.clear();
				fstream_.close();
			}

			std::string sFile = save_path_;
			std::replace(sFile.begin(), sFile.end(), '\\', '/');

			// 打开文件
			fstream_.open(sFile.c_str(), std::ios::binary);

			if(!fstream_.is_open()) {
				std::cout << "open file failed [file://" << sFile << "], error code:" << errno << std::endl;
				return 0;
			}			
		}

		return file_size_;
	}

// IDownloadObject
public:
	uint64 downloaded() { 
		return downloaded_size_; 
	}

	virtual const char* actual_url() { 
		return actual_url_.c_str(); 
	}

	virtual bool completed() = 0;

// 接口IDownloadObject
public:
	uint64 get_downloaded() 

	const char* get_actual_url() 
	const char* get_url() { return url_.c_str(); }

// IHttpWriterObject
public:

	void write_header(const std::string& name, const std::string& value) {
		if(status() == 200) {
			if(!name.empty()) {
				headers_[name] = value;
			}

			if(name == "Content-Length") {
				size(atoi(value.c_str()));
			}
		} else if( status() == 302 ) {
			if(name == "Location") {
				actual_url_ = value;
			}
		} 	
	}

	void write_data(char* buffer, size_t size) {
		downloaded_size_ += size;
		//printf("downloaded size: %d\r\n", downloaded_size_);
		if(!fstream_.fail()) {
			fstream_.write(buffer, size);
		}

		size_t writebytes = fstream_.tellp();
		if((downloaded_size_ == file_size_) && (file_size_ > 0)) {
			fstream_.close();
		}
	}



protected:
	DownloadObject(void) 
	: url_("")
	, actual_url_("")
	, save_path_("")
	, file_size_(0)
	, downloaded_size_(0)
	, status_code_(0)
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
// class RequestObject
//////////////////////////////////////////////////////////////////////////
class RequestObject : public IRequestObject {

};


//////////////////////////////////////////////////////////////////////////
// class ControllerProxy
//////////////////////////////////////////////////////////////////////////
class ControllerProxy : Object {
public:
	size_t write_header(char* buffer, size_t size) {
		// parse header info
		std::string header(buffer, size);
		size_t pos = header.find_first_of(":");
		if(pos != std::string::npos) {
			std::string name = header.substr(0, pos);
			std::string value = header.substr(pos+1, header.size()-pos-1);
			trim_left(value);
			trim_right(value, "\n\r");
			object_->write_header(name.c_str(), value.c_str());
		} else {
			// get status code
			std::string http_version;
			int status = 0;

			size_t blank_pos = header.find(' ');
			if(blank_pos != std::string::npos) {
				// http version
				http_version = header.substr(0, blank_pos);
			}
			size_t old_blank_pos = blank_pos;
			blank_pos = header.find(' ', old_blank_pos+1);
			if(blank_pos != std::string::npos) {
				// http status code
				status = (atoi(header.substr(old_blank_pos+1, blank_pos).c_str()));
			}

			object_->write_httpv_status(http_version.c_str(), status);
		}

		return size;
	}

	void http_work() {
		const char* url = "";
		if(NULL != url) {
			CURL *curl;
			CURLcode res;
			curl = curl_easy_init();
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
			curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &ControllerProxy::recv_header);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, this);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  &ControllerProxy::recv_data);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

			res = curl_easy_perform(curl); /* ignores error */ 
			if(res != CURLE_OK)
				fprintf(stderr, "curl_easy_perform() failed: %s\n",	curl_easy_strerror(res));
			curl_easy_cleanup(curl);
		}			
	}

	static size_t recv_header(void *ptr, size_t size,size_t nmemb, void *userdata) {
		ControllerProxy* this_ = static_cast<ControllerProxy*>(userdata);
		this_->OnRecvHeaderLine(ptr, size * nmemb);
		return (size * nmemb);
	}

	static size_t recv_data(void *ptr, size_t size,size_t nmemb, void *userdata) {
		ControllerProxy* this_ = static_cast<ControllerProxy*>(userdata);
		this_->OnRecvData(ptr, size * nmemb);
		return size * nmemb;
	}


	size_t write_data(char* buffer, size_t size) {
		return object_->write_data(buffer, size);
	}

	const char* get_url() {
		return object_->url();
	}

private:
	ControllerProxy() 
	: controller_(NULL)
	, object_(NULL)
	{

	}

	~ControllerProxy() {}

private:
	RefPtr<IHttpWriterObject> object_;
	RefPtr<IController> controller_;
};


//////////////////////////////////////////////////////////////////////////
// class DownloadControllerProxy
//////////////////////////////////////////////////////////////////////////
class DownloadControllerProxy : public ControllerProxy {
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
};


//////////////////////////////////////////////////////////////////////////
// class RequestControllerProxy
//////////////////////////////////////////////////////////////////////////
class RequestControllerProxy : public IControllerProxy {
	// static method
public:
	static RequestControllerProxy* create(const char* sUrl, IRequestController* controller) {
		RequestControllerProxy* p = new RequestControllerProxy;
		p->download_object_ = DownloadObject::create(sUrl, sSavePath);
		p->controller_ = controller;
		if(p->controller_) {
			p->controller_->OnAttach(p->download_object_);
		}

		return p;
	}
};



//////////////////////////////////////////////////////////////////////////
// class HttpTask
//////////////////////////////////////////////////////////////////////////

class HttpTask : public ITask {
public:
	HttpTask(ControllerProxy* c) 
	: controller_proxy(c) 
	{
		
	}

	~HttpTask() {}

	
	bool Task() {
		if(controller_proxy) {
			controller_proxy->http_work();
		}
		return true;
	}

private:
	RefPtr<ControllerProxy> controller_proxy;
};

class HttpManager : public Http {
// IDownloaders 接口
public:
	
	long Request(const char* sUrl, IRequestController* controller) {
		RefPtr< ControllerProxy<HttpRequestController, HttpRequestObject> > controller_proxy = RequestControllerProxy::create(sUrl, sSavePath, controller);
		if(NULL != controller_proxy) {
			queue_.push_back(controller_proxy);
			if(!thread_pool_manager_.newtask(new HttpTask(controller_proxy, this), -1, 1)) {
				// create task failed.
				return -1;
			}
		}
		
		return 0;
	}

	long Download(const char* sUrl, const char* sSavePath, IDownloadController* controller) {
		RefPtr<DownloadControllerProxy> controller_proxy = DownloadControllerProxy::create(sUrl, sSavePath, controller);
		if(NULL != controller_proxy) {
			queue_.push_back(controller_proxy);
			if(!thread_pool_manager_.newtask(new HttpTask(controller_proxy, this), -1, 1)) {
				// create task failed.
				return -1;
			}
		}

		return 0;
	}

	void remove_task(const char* sUrl) {

	}

protected:
	void initialize(int thread_num) {
		/* Must initialize libcurl before any threads are started */ 
		curl_global_init(CURL_GLOBAL_ALL);
		// create thread pool
		thread_pool_manager_.run(thread_num);
	}

public:
	HttpManager(void) {};
	~HttpManager(void) {};

private:
	std::list< RefPtr<DownloadControllerProxy> > queue_;
	ThreadPoolManager thread_pool_manager_;	
};



Http* create_qhttp(int thread_num)
{
	return HttpManager::create(thread_num);
}

} // namespace q