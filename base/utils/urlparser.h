
#ifndef __URLPARSER_H__
#define __URLPARSER_H__

#include "typedefs.h"
#include <map>


namespace base {
	namespace utils {

		// UrlParser ����˼�� url�����������ݴ����sUrl��������Ӧ�Ĳ������磺
		// Э�����ͣ�HTTP|HTTPS|FTP�ȵȣ�,Ĭ��ΪHTTP
		// ����
		// �˿�
		// uri·����������ѯ�ַ���
		// ��ѯ�ַ��� "#"�Ż���"?"����ģ�����"#"����"?"
		// 
		// ���Խ����ĸ�ʽ����,���磺 
		// (http|https|ftp://)(user:password@domain:3306)(/xxx)(?name=111)(#anchor)
		
		class UrlParser
		{
		public:
			// ��׼���캯��
			UrlParser(const char* sUrl);
			~UrlParser();
#ifdef USING_DEBUG
			//! ��ӡ���������-����ֵ
			void dump();
#endif
			// URL�����Ԫ�صı�ʶ
			typedef enum {SCHEME=0,USER,PASSWORD,DOMAINS,PORT,PATH,QUERY,ANCHOR,} URL_;

			std::string operator[](URL_ t) {
				return UrlParams_[t];
			}

		protected:

			// �����������Ƚ���scheme���ٸ��� б��('/')��������������
			// ���������Դ·����Ȼ��ֱ������ϸ����Ϣ
			bool parse();
			bool parsePath(const char* szPath);
			bool parseDomain(const char* szDomain);

		private:
			typedef std::map<URL_, std::string> URLParams;

			// �������֮��Ĳ�����ֵ
			URLParams UrlParams_;

			// ������url��ַ��ͬʱ�Ѿ�����б��('\\')ת����б��('/')
			std::string sUrl_;

			DISALLOW_COPY_AND_ASSIGN(UrlParser);
		};
	} // utils namespace
} // base namespace

#endif
