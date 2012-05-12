
#include "stdafx.h"
#include "ZZipFileObject.h"
#include <atlbase.h>
#include <algorithm>

ZZipFileObject::ZZipFileObject()
: bFolder_(false)
, sLocalPath_(_T(""))
{
	memset(&FileItem_, 0, sizeof(ZZipFileItem));
}

ZZipFileObject::~ZZipFileObject( void )
{

}

bool ZZipFileObject::setpath( const tstring& sPath) {

	// verify empty
	if(!sPath.empty()) {
		tstring sZZipPath;
		// 检测目录,是否是"/"开头
		if(sPath[0] == _T('/') || sPath[0] == _T('\\')) {
			sZZipPath = sPath;
		} else {
			sZZipPath.append(1, _T('/'));
			sZZipPath.append(sPath);
		}
		// '\' -> '/'
		std::replace(sZZipPath.begin(), sZZipPath.end(), _T('\\'), _T('/'));
		bFolder_ = (sZZipPath.find_last_of(_T('/')) == (sZZipPath.size()-1));
		return true;
	}
	
	return false;
}

bool ZZipFileObject::isfolder() const
{
	return bFolder_;
}

int64 ZZipFileObject::offset() const
{
	return FileItem_.offset;
}

int64 ZZipFileObject::filesize() const
{
	return FileItem_.filesize;
}

tstring ZZipFileObject::localpath() const
{
	return sLocalPath_;
}

int32 ZZipFileObject::createtime() const
{
	return FileItem_.createtime;
}

int32 ZZipFileObject::lastmodify() const
{
	return FileItem_.lastmodify;
}