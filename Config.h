#pragma once

#include "json/json.h"

class Config
{
public:
	Config(void);
	~Config(void);

	template<typename T>
	bool Set(const std::string& key, const T& value) {
		Json::Value v;
		Parse(key, v);
		return true;
	}
	
	template<typename T>
	Json::Value Get(const std::string& key, const T& default_value) const {
		Json::Value v;
		Parse(key, v);
		if(v.isNull()) {
			return Json::Value(default_value);
		}
		
		return v;
	}
	
	template<typename T>
	Json::Value& Get(const std::string key, const Json::Value& default_value) {
	
	}

private:
	 void Parse(const std::string& path, Json::Value& v) {
		std::string::size_type cur = 0;
		std::string::size_type pos = path.find('.', cur);
		std::string key;

		Json::Value& t = value_;
		do {
			key = path.substr(cur, pos-cur);
			if(!t[key].isObject()) {
				return;	
			}

			t = t[key];
			pos = path.find('.', cur);
			cur = pos+1;
		} while(pos != std::string::npos);

		v = t;
	}




// –Ú¡–ªØ
public:
	bool FromString(const std::string& in);
	bool ToString(std::string& out);

private:
	Json::Value value_;
};
