
#include "ZZipFileObject.h"
#include <atlbase.h>
#include <algorithm>

ZZipFileObject::ZZipFileObject()
: sParentPath_(_T(""))
, sName_(_T(""))
, sLocalPath_(_T(""))
{
	memset(&FileItem_, 0, sizeof(ZZipFileItem));


}

ZZipFileObject::~ZZipFileObject( void )
{

}

void ZZipFileObject::ZZipPathFromPath( const tstring& sPath) {

	// check
	if(sPath.empty()) { return; }

	// 解析路径和名称
	tstring sTempPath = sPath;
	std::replace(sTempPath.begin(), sTempPath.end(), '\\', '/');
	bool bFolder = (sTempPath[sTempPath.size()-1] == '/');
	if(bFolder) {
		size_t pos = sTempPath.size()-2;
		while(pos >= 0 ) {
			if(sTempPath[pos]==_T('/')) break;
			pos--;
		}
		if(pos <= 0) {
			// bValid_ = false;
		} else {
			sParentPath_ = (sTempPath.substr(0, pos).c_str());
			sName_ = (sTempPath.substr(pos+1, sTempPath.size()-1).c_str());
		}
	} else {
		size_t pos = sTempPath.find_last_of('/');
		if(pos == std::string::npos) {
			// bValid_ = false;
		} else {
			sParentPath_ = (sTempPath.substr(0, pos).c_str());
			sName_ = (sTempPath.substr(pos+1, sTempPath.size()-1).c_str());
		}
	}
}
