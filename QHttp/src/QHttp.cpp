
#include "QHttp.h"
#include "RefCounted.h"
#include "ThreadPool.h"

#include <list>
#include <curl/curl.h>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <map>
#include <iostream>
#include "QCookieManager.h"

void static trim_right(std::string& s, const char* str = " ") {
	s.erase(s.find_last_not_of(str)+1);
}

void static trim_left(std::string& s, const char* str = " ") {
	s.erase(0, s.find_first_not_of(str));
}

namespace q {

class HttpManager;

// content impliment interface 
class IHttpResponse : public Object{
public:
	virtual int64 write(const char* data, int64 size) = 0;
	virtual void  writeEnd() = 0;
	virtual const char* text() = 0;
	virtual ~IHttpResponse() {};
};

class ContentFileImpl: public IHttpResponse {
// IHttpResponse
public:
	int64 write(const char* data, int64 size) {
		//printf("downloaded size: %d\r\n", read_size_);
		if(!fstream_.fail()) {
			fstream_.write(data, size);
		}
		return size;
	}

	void writeEnd() {
		if(fstream_.is_open()) {
			fstream_.close();
		}
	}

	const char* text() {
		// not get in here
		return 0;
	}

public:
	ContentFileImpl(const std::string& save_path)
	: save_path_(save_path)
	{
		if(fstream_.is_open()) { 
			fstream_.clear();
			fstream_.close();
		}

		std::string sFile = save_path_;
		std::replace(sFile.begin(), sFile.end(), '\\', '/');

		fstream_.open(sFile.c_str(), std::ios::binary);

		if(!fstream_.is_open()) {
			std::cout << "open file failed [file://" << sFile << "], error code:" << errno << std::endl;
		}
	}

private:
	std::string save_path_;
	std::ofstream fstream_;
};

class ContentBufferImpl: public IHttpResponse {
// constructor/destructor
public:
	ContentBufferImpl()	{}
	~ContentBufferImpl() {}

// IHttpResponse
public:
	int64 write(const char* data, int64 size) 
	{
		sContent_.append(data, size);
		return size;
	}

	void writeEnd() {}

	const char* text() { return sContent_.c_str(); }

private:
	std::string sContent_;
};


/*
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
	int64 file_size() { return impl_->file_size(); }
	int64 read_size() { return impl_->read_size(); }	
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

*/

//////////////////////////////////////////////////////////////////////////
// class HttpRequest

class HttpRequest 
	: public IHttpRequest 
{
public:
	HttpRequest(HttpManager* p);
	~HttpRequest() ;
	friend class HttpTask;
	typedef std::map<std::string, std::string> HEADERS;
// IHttpRequest
public:
	bool open(const char* url, ACTION action, IEvent* event, const char* save_path);
	bool setRequestHeader(const char* name, const char* value);
	void send(const char* data =  NULL);
	void escape(const std::string& sIn, std::string& sOut);
	const char* responseText() { return response_->text(); }
	int  status() { return status_; }

protected:
	bool execute();  // 工作线程调用

private:
	static size_t curlRecvHeader(void *ptr, size_t size, size_t nmemb, void *userdata);
	static size_t curlRecvData(void *ptr, size_t size, size_t nmemb, void *userdata);
	int64 recvData(char* buffer, int64 size);
	size_t parseHeader(char* buffer, size_t size);

private:
	std::string url_;
	std::string actual_url_;
	int status_;
	int64 content_length_;
	int64 read_bytes_;
	std::string http_version_;
	HEADERS request_headers_;
	std::string post_data_;
	std::string cookie_data_;

	CURL *curl_;
	CURLcode res_;
	struct curl_slist* header_list_;

	HttpManager* http_manager_;
	RefPtr<IEvent> event_;
	RefPtr<IHttpResponse> response_;
	HEADERS response_headers_;
};

/*

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

			res = curl_easy_perform(curl); 
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

*/

//////////////////////////////////////////////////////////////////////////
// class HttpTask
//////////////////////////////////////////////////////////////////////////

class HttpTask : public ITask {
public:
	HttpTask(HttpRequest* c) 
	: http_request_(c) 
	{
		
	}

	~HttpTask() 
	{

	}

	bool Task() {
		if(http_request_) {
			http_request_->execute();
		}
		return true;
	}

	void OnFinish() {}

private:
	RefPtr<HttpRequest> http_request_;
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
	virtual bool createHttpRequest(IHttpRequest** ppOut) {
		if(*ppOut != NULL) {
			return false;
		}

		IHttpRequest* p = new HttpRequest(this);
		p->add_ref();
		*ppOut = p;

		return true;
	}

// 方法
public:
	bool addTask(HttpRequest* task) 
	{
		list_.push_back(task);
		return thread_pool_manager_.newtask(new HttpTask(task), -1, 1);
	}

	void removeTask(HttpRequest* task) 
	{

	}

	void SetCookie(std::string url, const std::string& cookie_data);
	void GetCookies(std::string url, std::string& out_data);

protected:
	void initialize(int thread_num) {
		/* Must initialize libcurl before any threads are started */ 
		curl_global_init(CURL_GLOBAL_ALL);
		// create thread pool
		thread_pool_manager_.run(thread_num);
	}

public:
	HttpManager(void) 
	{

	}

	~HttpManager(void) 
	{

	}

private:
	CookieManager cookies_manager_;
	std::list< RefPtr<HttpRequest> > list_;
	ThreadPoolManager thread_pool_manager_;	
};

void HttpManager::GetCookies( std::string url, std::string& out_data )
{
	CookieManager::Cookies* p = cookies_manager_.GetCookies(url, true);
	if(p)
		p->GetString(NULL, out_data);
}

void HttpManager::SetCookie( std::string url, const std::string& cookie_data )
{
	CookieManager::Cookies* p = cookies_manager_.GetCookies(url, true);
	if(p)
		p->SetString(NULL, cookie_data);
}

////////////////////////////////////////////////////////////////////////////
// HttpRequest Impliement

HttpRequest::HttpRequest(HttpManager* p) 
: curl_(NULL)
, res_(CURLE_OK)
, http_manager_(p)
, status_(0)
, content_length_(0)
, read_bytes_(0)
, header_list_(0)
{

}

HttpRequest::~HttpRequest() 
{
	
}


bool HttpRequest::open(const char* url, ACTION action, IEvent* event, const char* save_path) 
{
	if(NULL == url)
		return false;

	if(POST != action) 
		action = GET;

	if(NULL == save_path) {
		response_ = new ContentBufferImpl();
	} else {
		response_ = new ContentFileImpl(save_path);
	}

	url_ = url;
	event_ = event;
	curl_ = curl_easy_init();
	curl_easy_setopt(curl_, CURLOPT_URL, url);

	curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl_, CURLOPT_HEADERFUNCTION, &HttpRequest::curlRecvHeader);
	curl_easy_setopt(curl_, CURLOPT_HEADERDATA, this);
	curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION,  &HttpRequest::curlRecvData);
	curl_easy_setopt(curl_, CURLOPT_WRITEDATA, this);


	curl_easy_setopt(curl_, CURLOPT_POST, action);

	if(event_)
		event_->onReadyStateChange(this, UNINITIALIZED);

	return true;
}

void HttpRequest::send(const char* data)
{
	header_list_ = NULL;
	// combine header
	std::string sHeader;
	HEADERS::const_iterator it = request_headers_.begin();
	for(; it != request_headers_.end(); it++) {
		sHeader = it->first + ":\t";
		sHeader += it->second;
		header_list_ = curl_slist_append(header_list_, sHeader.c_str());
	}
	curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, header_list_);
	
	if(data) {
		post_data_ = data;
		curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, post_data_.c_str());
		curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, post_data_.size());
	}

	// set request cookie
	http_manager_->GetCookies(url_, cookie_data_);
	if(!cookie_data_.empty()) 
		curl_easy_setopt(curl_, CURLOPT_COOKIE, cookie_data_.c_str());

	http_manager_->addTask(this);
}

bool HttpRequest::execute() {
	if(!curl_) 
	{
		// curl is not initialize by open()
		return false;
	}

	if(event_) 
		event_->onReadyStateChange(this, LOADING);

	res_ = curl_easy_perform(curl_);

	curl_slist_free_all(header_list_);

	if(event_) 
		event_->onReadyStateChange(this, COMPLETED);

	if(res_ != CURLE_OK) {
		//fprintf(stderr, "curl_easy_perform() failed: %s\n",	curl_easy_strerror(res_));
		return false;
	}

	curl_easy_cleanup(curl_);

	return true;
}

size_t HttpRequest::curlRecvHeader(void *ptr, size_t size, size_t nmemb, void *userdata) {
	HttpRequest* this_ = static_cast<HttpRequest*>(userdata);
	this_->parseHeader((char*)ptr, size * nmemb);
	return (size * nmemb);
}

size_t HttpRequest::curlRecvData(void *ptr, size_t size, size_t nmemb, void *userdata) {
	HttpRequest* this_ = static_cast<HttpRequest*>(userdata);
	this_->recvData((char*)ptr, size * nmemb);
	return (size * nmemb);
}

int64 HttpRequest::recvData(char* buffer, int64 size) {
	int return_size = response_->write(buffer, size);
	read_bytes_ += return_size;
	if(read_bytes_ >= content_length_) {
		response_->writeEnd();
	}

	return return_size;
}

size_t HttpRequest::parseHeader(char* buffer, size_t size) {
	// parse header info
	std::string header(buffer, size);
	size_t pos = header.find_first_of(":");
	if(pos != std::string::npos) {
		std::string name = header.substr(0, pos);
		std::string value = header.substr(pos+1, header.size()-pos-1);
		trim_left(value);
		trim_right(value, "\n\r");

		if(status() == 200) {
			if(!name.empty()) {
				response_headers_[name] = value;
			}

			if(name == "Content-Length") {
				content_length_ = atoi(value.c_str());
				//content_impl()->size(atoi(value.c_str()));
			} else if(name == "Set-Cookie") {
				http_manager_->SetCookie(url_, value);
			}
		} else if( status() == 302 ) {
			if(name == "Location") {
				actual_url_ = value;
			}
		}
	} else {
		// get status code
		size_t blank_pos = header.find(' ');
		if(blank_pos != std::string::npos) {
			// http version
			http_version_ = header.substr(0, blank_pos);
		}
		size_t old_blank_pos = blank_pos;
		blank_pos = header.find(' ', old_blank_pos+1);
		if(blank_pos != std::string::npos) {
			// http status code
			status_ = (atoi(header.substr(old_blank_pos+1, blank_pos).c_str()));
		}
	}

	return size;
}

bool HttpRequest::setRequestHeader( const char* name, const char* value )
{
	std::string sName, sValue;
	if(name == NULL) 
		return false;

	sName = name;
	if(value) 
		sValue = value;

	request_headers_[sName] = sValue;

	return true;
}

void HttpRequest::escape( const std::string& sIn, std::string& sOut )
{
	sOut.clear();
	char *efc = curl_easy_escape(curl_, sIn.c_str(), sIn.size());
	sOut.append(efc);
	curl_free(efc);
}


////////////////////////////////////////////////////////////////////////////
// HttpManager 

IHttp* create(int thread_number)
{
	return HttpManager::create(thread_number);
}

} // namespace q