
#include "stdafx.h"
#include "ZZipFileObject.h"
#include <atlbase.h>
#include <algorithm>

ZZipFileObject::ZZipFileObject()
: sZZipPath_(_T(""))
, sLocalPath_(_T(""))
{
	memset(&FileItem_, 0, sizeof(ZZipFileItem));
}

ZZipFileObject::~ZZipFileObject( void )
{

}

bool ZZipFileObject::ZZipPathFromPath( const tstring& sPath) {

	// verify empty
	if(!sPath.empty()) {
		sZZipPath_.clear();
		// 检测目录,是否是"/"开头
		if(sPath[0] == _T('/') || sPath[0] == _T('\\')) {
			sZZipPath_ = sPath;
		} else {
			sZZipPath_.append(1, _T('/'));
			sZZipPath_.append(sPath);
		}
		// '\' -> '/'
		std::replace(sZZipPath_.begin(), sZZipPath_.end(), _T('\\'), _T('/'));
		return true;
	}
	
	return false;
}

bool ZZipFileObject::isfolder()
{
	return (sZZipPath_.find_last_of(_T('/')) == (sZZipPath_.size()-1));
}

int64 ZZipFileObject::offset() const
{
	return FileItem_.offset;
}

int64 ZZipFileObject::filesize() const
{
	return FileItem_.filesize;
}

tstring ZZipFileObject::path() const
{
	return sZZipPath_;
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