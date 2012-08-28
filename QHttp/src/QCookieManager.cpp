
#include "QCookieManager.h"
#include <sstream>
#include <stdio.h>

namespace q {

CookieManager COOKIEMGR;

// Cookies 类实现

// protected 标准构造
CookieManager::Cookies::Cookies() {}

// protected 析构函数
CookieManager::Cookies::~Cookies() {}

// public 设置cookies字段
bool CookieManager::Cookies::Set(
	const std::string& sCkName, 
	const std::string& sCkValue
){
	if(sCkName.empty()) return false;
	cookies_data_[sCkName] = sCkValue;
	return true;
}

// public 获取cookies字段
bool CookieManager::Cookies::Get(
	const std::string& sCkName, 
	std::string& sCkValue
) const{
	COOKIES::const_iterator cit = cookies_data_.find(sCkName);
	if(cit != cookies_data_.end()) {
		sCkValue = cit->second;
		return true;
	} else {
		sCkValue.clear();
	}
	return false;
}

// public 移除cookies字段
void CookieManager::Cookies::Remove(const std::string& sCkName) {
	COOKIES::iterator it = cookies_data_.find(sCkName);
	if(it != cookies_data_.end()) {
		cookies_data_.erase(it);
	}
}

// public 组合Cookies
void CookieManager::Cookies::toString(const char* szFilterNames, std::string& sCookies) {
	sCookies.clear();
	std::ostringstream output;
	Cookies::COOKIES::const_iterator cit = cookies_data_.begin();
	while(cit != cookies_data_.end()) {
		output << cit->first << "=" << cit->second << ";";
		cit++;
	}
	sCookies = output.str();
}

// CookieManager 类的实现

// public 标准构造函数
CookieManager::CookieManager(void)
{
	//printf("CookieManager::CookieManager(void)\n");
	m_cookies.clear();
}

// public 标准析构函数
CookieManager::~CookieManager(void)
{
	//printf("CookieManager::~CookieManager(void)\n");
	HOST_COOKIES_MAP::iterator it = m_cookies.begin();
	while(it != m_cookies.end()) {
		if(it->second) {
			delete it->second;
			m_cookies.erase(it++);
		} else {
			it++;
		}
	}
}

// public 
CookieManager::Cookies* CookieManager::GetCookies(std::string&szHost, bool bCreateIfNotExists /* = false */)
{
	//! 返回的主机Cookies指针
	Cookies* pCookies = 0; 

	//! 将主机添加端口后再查询
	std::string host_with_port;
	size_t pos = szHost.find(":");
	if(pos != std::string::npos) {
		host_with_port = szHost;
	} else {
		host_with_port = szHost + ":80";
	}

	//! 查找主机对应的Cookies
	HOST_COOKIES_MAP::iterator it = m_cookies.find(host_with_port);
	if ( it == m_cookies.end() ) {
		if(bCreateIfNotExists) {
			pCookies = new Cookies;
			m_cookies[host_with_port] = pCookies;
		}
	} else {
		pCookies = it->second;
	}	

// 	Cookies::COOKIES::const_iterator cit = items.begin();
// 	while(cit != items.end()) {
// 		pCookies->Set(items.first, items.second);
// 		cit++;
// 	}
	return pCookies;
}

void CookieManager::Clear( const std::string& szHost )
{
	//! 返回的主机Cookies指针
	Cookies* pCookies = 0; 

	//! 将主机去端口后再查询
	//! 将主机去端口后再查询
	std::string host_with_port;
	size_t pos = szHost.find(":");
	if(pos != std::string::npos) {
		host_with_port = szHost;
	} else {
		host_with_port = szHost + ":80";
	}

	//! 查找主机对应的Cookies
	HOST_COOKIES_MAP::iterator it = m_cookies.find(host_with_port);
	if ( it != m_cookies.end() ) {
		pCookies = it->second;
		m_cookies.erase(it);
	}

	delete pCookies;
	pCookies = NULL;
}

void CookieManager::ClearAll() {
	HOST_COOKIES_MAP::const_iterator cit = m_cookies.begin();
	for(;cit != m_cookies.end();cit++) {		
		delete cit->second;
		
	}
	m_cookies.clear();
}


} // end namespace q
