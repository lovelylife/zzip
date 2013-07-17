
#include "QCookieManager.h"
#include <sstream>
#include <stdio.h>
#include "utils/urlparser.h"

using namespace base::utils;

namespace q {

CookieManager COOKIEMGR;

// Cookies ��ʵ��

// protected ��׼����
CookieManager::Cookies::Cookies() {}

// protected ��������
CookieManager::Cookies::~Cookies() {}

// public ����cookies�ֶ�
bool CookieManager::Cookies::Set(
	const std::string& sCkName, 
	const std::string& sCkValue
)
{
	AutoLock<CriticalSectionEx> lock(locker_);
	if(sCkName.empty()) return false;
	cookies_data_[sCkName] = sCkValue;
	return true;
}

// public ��ȡcookies�ֶ�
bool CookieManager::Cookies::Get(const std::string& sCkName, std::string& sCkValue) 
{
	AutoLock<CriticalSectionEx> lock(locker_);
	COOKIES::const_iterator cit = cookies_data_.find(sCkName);
	if(cit != cookies_data_.end()) {
		sCkValue = cit->second;
		return true;
	} else {
		sCkValue.clear();
	}
	return false;
}

// public �Ƴ�cookies�ֶ�
void CookieManager::Cookies::Remove(const std::string& sCkName) {
	AutoLock<CriticalSectionEx> lock(locker_);
	COOKIES::iterator it = cookies_data_.find(sCkName);
	if(it != cookies_data_.end()) {
		cookies_data_.erase(it);
	}
}

// public ���Cookies
void CookieManager::Cookies::GetString(const char* szFilterNames, std::string& sCookies) {

	AutoLock<CriticalSectionEx> lock(locker_);
	sCookies.clear();
	std::ostringstream output;
	{
		AutoLock<CriticalSectionEx> lock(locker_);
		Cookies::COOKIES::const_iterator cit = cookies_data_.begin();
		while(cit != cookies_data_.end()) {
			output << cit->first << "=" << cit->second << ";";
			cit++;
		}
	}
	
	sCookies = output.str();
}

void CookieManager::Cookies::SetString( const char* szFilterNames, const std::string& sCookies )
{
	std::string s = sCookies;
	char* str = strtok((char*)s.c_str(), "; ");
	std::string temp_str;
	while(str) {
		temp_str = str;
		int pos = temp_str.find('=');
		Set(temp_str.substr(0, pos), temp_str.substr(pos+1, temp_str.size()-pos-1));
		str = strtok(NULL, "; ");
	}
}

// CookieManager ���ʵ��

// public ��׼���캯��
CookieManager::CookieManager(void)
{
	//printf("CookieManager::CookieManager(void)\n");
	cookies_.clear();
}

// public ��׼��������
CookieManager::~CookieManager(void)
{
	//printf("CookieManager::~CookieManager(void)\n");
	HOST_COOKIES::iterator it = cookies_.begin();
	while(it != cookies_.end()) {
		if(it->second) {
			delete it->second;
			cookies_.erase(it++);
		} else {
			it++;
		}
	}
}

// public 
CookieManager::Cookies* CookieManager::GetCookies(std::string& szHost, bool bCreateIfNotExists /* = false */)
{
	//! ���ص�����Cookiesָ��
	Cookies* pCookies = 0; 

	//! ��������Ӷ˿ں��ٲ�ѯ
	UrlParser urlparser(szHost.c_str());
	std::string host_with_port;
	host_with_port = urlparser[UrlParser::DOMAINS];
	std::string port = urlparser[UrlParser::PORT];
	if(!port.empty()) {
		host_with_port = szHost;
	} else {
		if(urlparser[UrlParser::SCHEME] == "https")
		{
			host_with_port += ":443";
		} else {
			host_with_port += ":80";
		}		
	}
	{
		AutoLock<CriticalSectionEx> lock(locker_);
		//! ����������Ӧ��Cookies
		HOST_COOKIES::iterator it = cookies_.find(host_with_port);
		if ( it == cookies_.end() ) {
			if(bCreateIfNotExists) {
				pCookies = new Cookies;
				cookies_[host_with_port] = pCookies;
			}
		} else {
			pCookies = it->second;
		}
	}

	return pCookies;
}

void CookieManager::Clear( const std::string& szHost )
{
	//! ���ص�����Cookiesָ��
	Cookies* pCookies = 0; 

	//! ��������Ӷ˿ں��ٲ�ѯ
	base::utils::UrlParser urlparser(szHost.c_str());
	std::string host_with_port = urlparser[UrlParser::DOMAINS];
	std::string port = urlparser[UrlParser::PORT];
	if(!port.empty()) {
		host_with_port = szHost;
	} else {
		if(urlparser[UrlParser::SCHEME] == "https" ) {
			host_with_port += ":443";
		} else {
			host_with_port += ":80";
		}		
	}

	{
		AutoLock<CriticalSectionEx> lock(locker_);
		//! ����������Ӧ��Cookies
		HOST_COOKIES::iterator it = cookies_.find(host_with_port);
		if ( it != cookies_.end() ) {
			pCookies = it->second;
			cookies_.erase(it);
		}
	}	

	delete pCookies;
	pCookies = NULL;
}

void CookieManager::ClearAll() {
	AutoLock<CriticalSectionEx> lock(locker_);
	HOST_COOKIES::const_iterator cit = cookies_.begin();
	for(;cit != cookies_.end();cit++) {		
		delete cit->second;
	}
	cookies_.clear();
}


} // end namespace q
