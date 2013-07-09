//////////////////////////////////////////////////////////////////////////
// Config.cpp
// Author: Q
// Date: 2013-07-07
// 使用的jsoncpp从这里下载http://sourceforge.net/projects/jsoncpp/ 
// 博客地址：http://www.cnblogs.com/lovelylife

#include "Config.h"
#include <fstream>


Config::Config(void) :
value_(Json::objectValue)
{

}


Config::~Config(void)
{

}

bool Config::FromFile( const std::string& sFile )
{
	if(sFile.empty()) 
		return false;

	// 获取文件大小
	std::ifstream f(sFile.c_str());
	f.seekg(std::ios::beg, std::ios::end);
	std::streamsize file_size = f.tellg();
	if(file_size <= 0)
		return false;

	// 从文件读取数据
	f.seekg(0, std::ios::end);
	char* buffer = new char[file_size];
	f.read(buffer, file_size);
	f.close();

	// 初始化
	bool b = FromString(buffer);
	delete buffer;
	buffer = NULL;

	return b;
}

bool Config::FromString( const std::string& sBuffer )
{
	Json::Reader reader;
	if(!reader.parse(sBuffer, value_)) {
		return false;
	}

	return true;
}

std::string Config::ToString()
{
	Json::StyledWriter writer;
	return writer.write(value_);
}

bool Config::ToFile( const std::string& sSaveFile )
{
	if(sSaveFile.empty()) 
		return false;

	std::string sOut = ToString();

	std::ofstream f(sSaveFile.c_str());
	if(!f.good()) {
		return false;
	}

	f.write(sOut.c_str(), sOut.size());
	f.close();

	return true;
}


#ifdef USING_TestCode
class TestCode_Config {
public:
	TestCode_Config() {
		Config config;
		config.FromString("{}");
		config.Set("test.base.savepath", "D:/test");

		Json::Value v = config.Get("base.savepath", "");
		std::string s = v.asString();
		OutputDebugStringA(s.c_str());
		std::string sJsonString;
		sJsonString = config.ToString();
		OutputDebugStringA(sJsonString.c_str());
	}

	~TestCode_Config() {
		
	}
} TestCode_Config_c;
#endif