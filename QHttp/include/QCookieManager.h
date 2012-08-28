
#ifndef __COOKIE_MANAGER_H__
#define __COOKIE_MANAGER_H__

#pragma once
#include <map>
#include <string>

// ��ֹ���������͸�ֵ��������������Ϊprivate����
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);               \
	void operator=(const TypeName&)

namespace q {

class CookieManager {
public:
	CookieManager(void);
	~CookieManager(void);

	// ����������cookies��Ϣ
	class Cookies {
	public:
		friend class CookieManager;
		typedef std::map<std::string, std::string> COOKIES;

		bool Set(const std::string& sCkName, const std::string& sCkValue);
		bool Get(const std::string& sCkName, std::string& sCkValue)const;
		void Remove(const std::string& sCkName);
		void Clear() { cookies_data_.clear(); }

		// ��Cookies������װ�����磨cname1=value1;cname2=value2;...��
		// ��ʽ���ַ���, ͬʱ���Ը���szFilterNames(cname1,cname2,...)
		// ���˵�����Ҫ���ֶΣ���δʵ�֣�, ����ʱ���� NULL����0��
		void toString(const char* szFilterNames, std::string& sCookies);

	protected:
		//! ��ֹ�ⲿ������ⲿɾ��, ֻ������CCookieManager�д���������
		Cookies();
		~Cookies();

	private:
		// HOSTCOOKIES ���ڱ���������cookies��Ϣ�� �����ʽ��
		// Cookie���� -> Cookieֵ
		COOKIES cookies_data_;

		DISALLOW_COPY_AND_ASSIGN(Cookies);
	};

	// ��ȡCookies�������ָ��������Cookies���󲻴��ڣ���
	// ����bCreateIfNotExists�����Ƿ񴴽��µ�Cookies����
	// ����������cookies������ͨ��Cookies������
	Cookies* GetCookies(std::string&szHost, bool bCreateIfNotExists = false);
	void Clear(const std::string& szHost);
	void ClearAll();
private:
	// HOST_COOKIES_MAP ���ڱ�������������Cookies��Ϣ�������ʽ��
	// ������ -> Cookies
	typedef std::map< std::string, Cookies* > HOST_COOKIES_MAP;
	HOST_COOKIES_MAP m_cookies;

	DISALLOW_COPY_AND_ASSIGN(CookieManager);
};

extern CookieManager COOKIEMGR;


}

#endif
