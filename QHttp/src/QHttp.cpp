
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

// content impliment interface 
class IContentImpl : public Object{
public:
	typedef enum {TYPE_BUFFER = 0, TYPE_FILE =1} CTYPE;
	virtual CTYPE type() = 0;
	virtual int64 size(int64 s) = 0;
	virtual int64 write_data(const char* data, int64 size) = 0;
	virtual ~IContentImpl() {};
};

class ContentFileImpl: public IContentImpl {
public:
	CTYPE type() { return TYPE_FILE; }
	int64 size(int64 size) 
	{
		read_size_ = 0; 
		total_size_ = size;

		if(fstream_.is_open()) { 
			fstream_.clear();
			fstream_.close();
		}

		std::string sFile = save_path_;
		std::replace(sFile.begin(), sFile.end(), '\\', '/');

		fstream_.open(sFile.c_str(), std::ios::binary);

		if(!fstream_.is_open()) {
			std::cout << "open file failed [file://" << sFile << "], error code:" << errno << std::endl;
			return false;
		}

		return true;
	}

	uint64 write_data(const char* data, int64 size) {
		read_size_ += size;
		//printf("downloaded size: %d\r\n", read_size_);
		if(!fstream_.fail()) {
			fstream_.write(data, size);
		}

		size_t writebytes = fstream_.tellp();
		if((read_size_ == total_size_) && (total_size_ > 0)) {
			fstream_.close();
		}

		return size;
	}

	int64 file_size() { return total_size_; }
	int64 read_size() { return read_size_; }
	// bool completed() { return (file_size_ == read_size_) && (file_size_ > 0); }

public:
	ContentFileImpl(const std::string& save_path)
	: save_path_(save_path)
	, total_size_(0)
	, read_size_(0)
	{

	}
	
private:
	std::string save_path_;
	int64 total_size_;
	int64 read_size_;
	std::ofstream fstream_;
};

class ContentBufferImpl: public IContentImpl {
public:
	ContentBufferImpl()
	: total_size_(0)
	, read_size_(0)
	{

	}

	~ContentBufferImpl() {}

public:
	CTYPE type() { return TYPE_BUFFER; }
	int64 write_data(const char* data, int64 size) {
		read_size_ += size;
		sContent_.append(data, size);
		
		return size;
	}

	int64 size(int64 sizes) { 
		read_size_ = 0; 
		total_size_ = sizes; 
		return true; 
	}

// methods
public:
	const char* content() { return sContent_.c_str(); }
	bool completed() {
		return (total_size_ > 0) && (read_size_ == total_size_);
	}
private:
	std::string sContent_;
	int64 total_size_;
	int64 read_size_;
};

class HttpObject 
	: virtual public Object 
{
public:
	void write_httpv_status(const std::string& httpv, int status) {
		http_version_ = httpv;
		status_code_  = status;
	}

	void write_header(const std::string& name, const std::string& value) {
		if(status() == 200) {
			if(!name.empty()) {
				headers_[name] = value;
			}

			if(name == "Content-Length") {
				content_impl()->size(atoi(value.c_str()));
			}
		} else if( status() == 302 ) {
			if(name == "Location") {
				actual_url_ = value;
			}
		}
	}

	void write_data(char* buffer, size_t size) {
		content_impl()->write_data(buffer, size);
	}

public:
	virtual IContentImpl* content_impl() = 0;

public:
	int status() {	return status_code_; }
	const char* url() { return url_.c_str(); }
	const char* actual_url() { return actual_url_.c_str(); }
	const char* header(const char* name) {
		if(name != NULL) {
			std::map<std::string, std::string>::const_iterator cit = headers_.find(name);
			if(cit == headers_.end()) {
				return cit->second.c_str();
			}		
		}

		return "";
	}

public:
	HttpObject(const std::string& sUrl)
	: url_(sUrl)
	, actual_url_("")
	, status_code_(0)
	{
		
	}

	~HttpObject() {}

protected:
	std::string url_;
	std::string actual_url_;
	uint32 status_code_;
	std::string http_version_;
	std::map<std::string, std::string> headers_;
};

//////////////////////////////////////////////////////////////////////////
// class DownloadObject
//////////////////////////////////////////////////////////////////////////
class DownloadObject : virtual public IDownloadObject, virtual public HttpObject {
public:
	IContentImpl* content_impl() { return impl_; }

// HttpObject
public:
	const char* url() { return HttpObject::url(); }
	int status() { return HttpObject::status(); }
	const char* header(const char* name) {	return HttpObject::header(name); }

// IDownloadObject
public:
	const char* actual_url() { return HttpObject::actual_url(); }
	uint64 file_size() { return impl_->file_size(); }
	uint64 read_size() { return impl_->read_size; }	
	// bool completed() { return impl_->completed(); }

// constructor/destructor
public:
	DownloadObject(const char* sUrl, const char* sSavePath) 
	: HttpObject(sUrl)
	, impl_(new ContentFileImpl(sSavePath))
	{
		
	}

	~DownloadObject(void) {}

private:
	RefPtr<ContentFileImpl> impl_;

}; // class DownloadObject



//////////////////////////////////////////////////////////////////////////
// class RequestObject
//////////////////////////////////////////////////////////////////////////
class RequestObject : public IRequestObject, virtual public HttpObject {
public:
	RequestObject(const std::string& sUrl) : HttpObject(sUrl) {
		impl_ = new ContentBufferImpl();
	}

	~RequestObject() {}

// HttpObject
public:
	IContentImpl* content_impl() { return impl_; };
	bool completed() { return impl_->completed(); }

// IHttpResponse
public:
	const char* url() { return HttpObject::url(); }
	int status() { return HttpObject::status(); }
	const char* header(const char* name) {	return HttpObject::header(name); }

// IRequestObject
public:
	const char* content() {
		return impl_->content();
	}

private:
	RefPtr<ContentBufferImpl> impl_;
};


//////////////////////////////////////////////////////////////////////////
// class HttpRequest

class HttpRequest 
	: public IHttpRequest 
{
public:
	HttpRequest() : curl_(NULL), res_(CURLcode::CURLE_OK) {}
	~HttpRequest() {}

// IHttpRequest
public:
	bool open(const char* url, const char* action) {
		std::string action_string = "get";
		if(NULL == url) return false;
		if(NULL != action) {
			action_string = action;
		}
		curl_ = curl_easy_init();
		curl_easy_setopt(curl_, CURLOPT_URL, url);
		curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1);
//		curl_easy_setopt(curl_, CURLOPT_HEADERFUNCTION, &ControllerProxy::recvHeader);
		curl_easy_setopt(curl_, CURLOPT_HEADERDATA, this);
//		curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION,  &ControllerProxy::recvData);
		curl_easy_setopt(curl_, CURLOPT_WRITEDATA, this);
		return true;
	}

	bool addRequestHeader(const char* name, const char* value){
		curl_slist_append(header_list_,);
	}

	bool cookie(const char* name, const char* cookies) {
		std::string
	}

	void onReadyStateChange(READYSTATE state) {

	}

private:
	CURL *curl_;
	CURLcode res_;
	struct curl_slist* header_list_;
};



//////////////////////////////////////////////////////////////////////////
// class ControllerProxy
//////////////////////////////////////////////////////////////////////////
class ControllerProxy 
	: public Object 
{
public:
	virtual void recvHeader(const std::string& name, const std::string& value) = 0;
	virtual void recvData(char* buffer, size_t size) = 0;
	virtual void recvHttpvStatus(const char* http_version, int status) = 0;
	virtual const char* url() = 0;
// libcurl
public:
	void httpWork() 
	{
		const char* url = url();
		if(NULL != url) {
			CURL *curl;
			CURLcode res;
			curl = curl_easy_init();
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
			curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &ControllerProxy::recvHeader);
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, this);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  &ControllerProxy::recvData);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

			res = curl_easy_perform(curl); /* ignores error */ 
			if(res != CURLE_OK)
				fprintf(stderr, "curl_easy_perform() failed: %s\n",	curl_easy_strerror(res));
			curl_easy_cleanup(curl);
		}			
	}

	static size_t recvHeader(void *ptr, size_t size, size_t nmemb, void *userdata) {
		ControllerProxy* this_ = static_cast<ControllerProxy*>(userdata);
		this_->parseHeader((char*)ptr, size * nmemb);
		return (size * nmemb);
	}

	static size_t recvData(void *ptr, size_t size, size_t nmemb, void *userdata) {
		ControllerProxy* this_ = static_cast<ControllerProxy*>(userdata);
		this_->recvData((char*)ptr, size * nmemb);
		return (size * nmemb);
	}

	size_t parseHeader(char* buffer, size_t size) {
		// parse header info
		std::string header(buffer, size);
		size_t pos = header.find_first_of(":");
		if(pos != std::string::npos) {
			std::string name = header.substr(0, pos);
			std::string value = header.substr(pos+1, header.size()-pos-1);
			trim_left(value);
			trim_right(value, "\n\r");
			recvHeader(name.c_str(), value.c_str());
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

			recvHttpvStatus(http_version.c_str(), status);
		}

		return size;
	}

// constructor / destructor
public:
	ControllerProxy() {}
	~ControllerProxy() {}

}; // class ControllerProxy


//////////////////////////////////////////////////////////////////////////
// class DownloadControllerProxy
//////////////////////////////////////////////////////////////////////////
class DownloadControllerProxy : public ControllerProxy {
// static method
public:
	static DownloadControllerProxy* create(const char* sUrl, const char* sSavePath, IDownloadController* controller) {
		DownloadControllerProxy* p = new DownloadControllerProxy;
		p->object_ = new DownloadObject(sUrl, sSavePath);
 		p->controller_ = controller;
 		if(p->controller_) {
 			p->controller_->onreadystatechange();
 		}

		return p;
	}

// 重写ControllerProxy
public:
	void recvHeader(const std::string& name, const std::string& value) {
		object_->writeHeader(name, value);
	}

	void recvData(char* buffer, size_t size) {
		object_->write_data(buffer, size);
	}

	void recvHttpvStatus(const char* http_version, int s) {
		object_->write_httpv_status(http_version, s);
	}

	const char* url() {	return object_->url(); }

private:
	RefPtr<IDownloadController> controller_;
	RefPtr<DownloadObject> object_;
};


//////////////////////////////////////////////////////////////////////////
// class RequestControllerProxy
//////////////////////////////////////////////////////////////////////////
class RequestControllerProxy : public ControllerProxy {
// static method
public:
	static RequestControllerProxy* create(const char* sUrl, IRequestController* controller) {
		RequestControllerProxy* p = new RequestControllerProxy;
		p->object_ = new RequestObject(sUrl);
 		p->controller_ = controller;

		return p;
	}

public:
	RequestControllerProxy() : ControllerProxy() {

	}

	~RequestControllerProxy(){

	}

// 重写ControllerProxy
public:
	void recvHeader(const std::string& name, const std::string& value) {
		object_->writeHeader(name, value);
	}

	void recvData(char* buffer, size_t size) {
		object_->recvData(buffer, size);
		if(object_->completed()) {
			controller_->onReadyStateChange(READYSTATE::COMPLETED);
		}	
	}

	void recvHttpvStatus(const char* http_version, int s){
		object_->write_httpv_status(http_version, s);
	}

	const char* url() {	return object_->url(); }
private:
	RefPtr<IRequestController> controller_;
	RefPtr<RequestObject> object_;
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

	~HttpTask() {

	}

	bool Task() {
		if(controller_proxy) {
			controller_proxy->httpWork();
		}
		return true;
	}

	void OnFinish() {}

private:
	RefPtr<ControllerProxy> controller_proxy;
};

class HttpManager : public IHttp {
// static
public:
	static HttpManager* create(int thread_number) {
		HttpManager* p = new HttpManager;
		p->initialize(thread_number);

		return p;
	}

// IHttp
public:
	virtual bool send(IHttpRequest*) {
		return true;
	}

// IDownloaders 接口
//public:	
//	long request(const char* sUrl, IRequestController* controller) {
//		RefPtr<ControllerProxy> controller_proxy = RequestControllerProxy::create(sUrl, controller);
//		if(NULL != controller_proxy) {
//			queue_.push_back(controller_proxy);
//			if(!thread_pool_manager_.newtask(new HttpTask(controller_proxy), -1, 1)) {
//				// create task failed.
//				return -1;
//			}
//		}
//		
//		return 0;
//	}
//
//	long download(const char* sUrl, const char* sSavePath, IDownloadController* controller) {
//		RefPtr<DownloadControllerProxy> controller_proxy = DownloadControllerProxy::create(sUrl, sSavePath, controller);
//		if(NULL != controller_proxy) {
//			queue_.push_back(controller_proxy);
//			if(!thread_pool_manager_.newtask(new HttpTask(controller_proxy), -1, 1)) {
//				// create task failed.
//				return -1;
//			}
//		}
//
//		return 0;
//	}

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
	HttpManager(void) {

	}

	~HttpManager(void) {

	}

private:
	std::list< RefPtr<ControllerProxy> > queue_;
	ThreadPoolManager thread_pool_manager_;	
};



IHttp* create(int thread_number)
{
	return HttpManager::create(thread_number);
}

} // namespace q