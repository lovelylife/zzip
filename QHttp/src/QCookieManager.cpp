
#include "QCookieManager.h"
#include <sstream>
#include <stdio.h>

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
){
	if(sCkName.empty()) return false;
	cookies_data_[sCkName] = sCkValue;
	return true;
}

// public ��ȡcookies�ֶ�
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

// public �Ƴ�cookies�ֶ�
void CookieManager::Cookies::Remove(const std::string& sCkName) {
	COOKIES::iterator it = cookies_data_.find(sCkName);
	if(it != cookies_data_.end()) {
		cookies_data_.erase(it);
	}
}

// public ���Cookies
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

// CookieManager ���ʵ��

// public ��׼���캯��
CookieManager::CookieManager(void)
{
	//printf("CookieManager::CookieManager(void)\n");
	m_cookies.clear();
}

// public ��׼��������
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
	//! ���ص�����Cookiesָ��
	Cookies* pCookies = 0; 

	//! ��������Ӷ˿ں��ٲ�ѯ
	std::string host_with_port;
	size_t pos = szHost.find(":");
	if(pos != std::string::npos) {
		host_with_port = szHost;
	} else {
		host_with_port = szHost + ":80";
	}

	//! ����������Ӧ��Cookies
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
	//! ���ص�����Cookiesָ��
	Cookies* pCookies = 0; 

	//! ������ȥ�˿ں��ٲ�ѯ
	//! ������ȥ�˿ں��ٲ�ѯ
	std::string host_with_port;
	size_t pos = szHost.find(":");
	if(pos != std::string::npos) {
		host_with_port = szHost;
	} else {
		host_with_port = szHost + ":80";
	}

	//! ����������Ӧ��Cookies
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
