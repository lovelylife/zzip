
#include "UrlParser.h"
#include <iostream>
#include <sstream>

//! 常量数组
static const char* gURLParamName[] = {
	"SCHEME"  ,
	"USER"	  ,
	"PASSWORD",
	"DOMAIN"  ,
	"PORT"	  ,
	"PATH"	  ,
	"QUERY"	  ,
	"ANCHOR"  ,
};


// public
base::utils::UrlParser::UrlParser(const char* sUrl)
: sUrl_() {
	{
		// '\' -> '/'
		char ch;
		int i=0;
		while((ch = sUrl[i++]) != '\0') {
			if(ch == '\\') {  
				sUrl_.append(1, '/'); 
			} else {
				sUrl_.append(1, ch);
			}
		}
	}
	parse();
}

// public
base::utils::UrlParser::~UrlParser() {}


//! protected 获取相应的解析参数
bool base::utils::UrlParser::parse() {

	//! 当前位置
	size_t curPos = 0;

	// 解析SCHEME
	size_t schemePos = sUrl_.find("://", curPos);
	if(schemePos != std::string::npos) {
		UrlParams_[SCHEME] = sUrl_.substr(0,schemePos);
		curPos = schemePos + 3;
	}

	// 分割域名和路径 通过 "/" 符合主机和路径
	// 然后分别解析主机和路径
	size_t pathPos = sUrl_.find("/", curPos);
	if(pathPos != std::string::npos) {
		parsePath(sUrl_.substr(pathPos, sUrl_.size() - pathPos).c_str());
	} 
	parseDomain(sUrl_.substr(curPos, pathPos - curPos).c_str());

	return true;
}

// protected
bool base::utils::UrlParser::parseDomain(const char* szDomain) {

	if(szDomain==0) { return false; } 

	//! 当前位置
	size_t curPos = 0;

	// user:password @ host:port
	std::string sDomain(szDomain);

	// user:password
	size_t loginPos = sDomain.find("@", curPos);
	if(loginPos != std::string::npos) {
		size_t userPos = sDomain.find(":", curPos);
		if(userPos != std::string::npos) {
			UrlParams_[USER] = sDomain.substr(curPos,userPos-curPos);
			UrlParams_[PASSWORD] = sDomain.substr(userPos+1,loginPos-(userPos+1));
		} else {
			UrlParams_[USER] = sDomain.substr(curPos,loginPos-curPos);
		}
		curPos = loginPos+1;
	}
	
	//! host:port
	size_t portPos = sDomain.find(":", curPos);
	if(portPos != std::string::npos) {
		UrlParams_[DOMAIN] = sDomain.substr(curPos,portPos-curPos);
		UrlParams_[PORT] = sDomain.substr(portPos+1,sDomain.size()-(portPos+1));
	} else {
		UrlParams_[DOMAIN] = sDomain.substr(curPos,sDomain.size()-curPos);
	}

	return true;
}

// protected
bool base::utils::UrlParser::parsePath(const char* szPath) {

	if(szPath==0) { return false; } 

	//! 当前位置
	size_t curPos = 0;

	// /path?querystring#anchor
	std::string sPath(szPath);

	// ?
	size_t queryPos = sPath.find("?", curPos);
	if(queryPos != std::string::npos) {
		UrlParams_[PATH] = sPath.substr(curPos,queryPos);
		curPos = queryPos+1;

		// #
		size_t anPos = sPath.find("#", curPos);
		if(anPos != std::string::npos) {
			UrlParams_[QUERY] = sPath.substr(curPos, anPos - curPos);
			UrlParams_[ANCHOR] = sPath.substr(anPos+1, sPath.size() - (anPos+1));
		} else {
			UrlParams_[QUERY] = sPath.substr(curPos, sPath.size() - curPos);
		}
	} else {
		UrlParams_[PATH] = sPath;
	}

	return true;
}


#ifdef USING_DEBUG
// public
void base::utils::UrlParser::dump() {
	std::ostringstream output;
	output << "The raw URL: "  << sUrl_ << "\n\rparsed result:\n\r";
	URLParams::const_iterator i = UrlParams_.begin();
	while(i!=UrlParams_.end()) {
		output << gURLParamName[i->first] << " = " << i->second << "\n\r";
		i++;
	}

	std::cout << output.str();
}
#endif
