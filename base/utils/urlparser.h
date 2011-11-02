
#ifndef __URLPARSER_H__
#define __URLPARSER_H__

#include "typedefs.h"
#include <map>


namespace base {
	namespace utils {

		// UrlParser 顾名思义 url解析器，根据传入的sUrl解析出相应的参数，如：
		// 协议类型（HTTP|HTTPS|FTP等等）,默认为HTTP
		// 域名
		// 端口
		// uri路径不包括查询字符串
		// 查询字符串 "#"号或者"?"后面的，包括"#"或者"?"
		// 
		// 可以解析的格式包括,例如： 
		// (http|https|ftp://)(user:password@domain:3306)(/xxx)(?name=111)(#anchor)
		
		class UrlParser
		{
		public:
			// 标准构造函数
			UrlParser(const char* sUrl);
			~UrlParser();
#ifdef USING_DEBUG
			//! 打印输出参数名-参数值
			void dump();
#endif
			// URL的组成元素的标识
			typedef enum {SCHEME=0,USER,PASSWORD,DOMAINS,PORT,PATH,QUERY,ANCHOR,} URL_;

		protected:

			// 解析函数，先解析scheme，再根据 斜杠('/')解析出域名主机
			// 和请求的资源路径，然后分别解析详细的信息
			bool parse();
			bool parsePath(const char* szPath);
			bool parseDomain(const char* szDomain);

		private:
			typedef std::map<URL_, std::string> URLParams;

			// 保存解析之后的参数和值
			URLParams UrlParams_;

			// 完整的url地址，同时已经将反斜杠('\\')转化成斜杠('/')
			std::string sUrl_;

			DISALLOW_COPY_AND_ASSIGN(UrlParser);
		};
	} // utils namespace
} // base namespace

#endif
