
#include "ZZipFileObject.h"
#include <algorithm>

ZZipFileObject::ZZipFileObject( const ZZipFileItem& FileItem, std::string& sName)
: FileItem_(FileItem)
, bFolder_(false)
, bValid_(false)
{
	// 解析路径和名称 
	if(!sName.empty()) {
		std::replace(sName.begin(), sName.end(), '\\', '/');
		bFolder_ = (sName[sName.size()-1] == '/');
		if(bFolder_) {
			size_t pos = sName.size()-2;
			while(pos >= 0 ) {
				if(sName[pos]=='/') break;
				pos--;
			}
			if(pos <= 0) {
				bValid_ = false;
			} else {
				sParentPath_ = sName.substr(0, pos);
				sName_ = sName.substr(pos+1, sName.size()-1);
			}
		} else {
			size_t pos = sName.find_last_of('/');
			if(pos == std::string::npos) {
				bValid_ = false;
			} else {
				sParentPath_ = sName.substr(0, pos);
				sName_ = sName.substr(pos+1, sName.size()-1);
			}
		}
	}

}

ZZipFileObject::~ZZipFileObject( void )
{

}