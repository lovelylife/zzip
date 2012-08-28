
#ifndef __COOKIE_MANAGER_H__
#define __COOKIE_MANAGER_H__

#pragma once
#include <map>
#include <string>

// 禁止拷贝函数和赋值操作，必须声明为private类型
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);               \
	void operator=(const TypeName&)

namespace q {

class CookieManager {
public:
	CookieManager(void);
	~CookieManager(void);

	// 管理主机的cookies信息
	class Cookies {
	public:
		friend class CookieManager;
		typedef std::map<std::string, std::string> COOKIES;

		bool Set(const std::string& sCkName, const std::string& sCkValue);
		bool Get(const std::string& sCkName, std::string& sCkValue)const;
		void Remove(const std::string& sCkName);
		void Clear() { cookies_data_.clear(); }

		// 将Cookies数据组装成形如（cname1=value1;cname2=value2;...）
		// 格式的字符串, 同时可以根据szFilterNames(cname1,cname2,...)
		// 过滤掉不需要的字段（暂未实现）, 调用时输入 NULL或者0。
		void toString(const char* szFilterNames, std::string& sCookies);

	protected:
		//! 防止外部构造和外部删除, 只允许在CCookieManager中创建和销毁
		Cookies();
		~Cookies();

	private:
		// HOSTCOOKIES 用于保存主机的cookies信息， 保存格式：
		// Cookie名称 -> Cookie值
		COOKIES cookies_data_;

		DISALLOW_COPY_AND_ASSIGN(Cookies);
	};

	// 获取Cookies对象，如果指定主机的Cookies对象不存在，则
	// 根据bCreateIfNotExists决定是否创建新的Cookies对象，
	// 所有主机的cookies操作均通过Cookies来操作
	Cookies* GetCookies(std::string&szHost, bool bCreateIfNotExists = false);
	void Clear(const std::string& szHost);
	void ClearAll();
private:
	// HOST_COOKIES_MAP 用于保存所有主机的Cookies信息，保存格式：
	// 主机域 -> Cookies
	typedef std::map< std::string, Cookies* > HOST_COOKIES_MAP;
	HOST_COOKIES_MAP m_cookies;

	DISALLOW_COPY_AND_ASSIGN(CookieManager);
};

extern CookieManager COOKIEMGR;


}

#endif
