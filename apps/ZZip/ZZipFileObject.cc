
#include "stdafx.h"
#include "ZZipFileObject.h"
#include <atlbase.h>
#include <algorithm>

ZZipFileObject::ZZipFileObject()
: sPath_(_T(""))
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
		sPath_.clear();
		// 检测目录,是否是"/"开头
		if(sPath[0] == _T('/') || sPath[0] == _T('\\')) {
			sPath_ = sPath;
		} else {
			sPath_.append(1, _T('/'));
			sPath_.append(sPath);
		}
		// '\' -> '/'
		std::replace(sPath_.begin(), sPath_.end(), _T('\\'), _T('/'));
		return true;
	}
	
	return false;
}

bool ZZipFileObject::isfolder()
{
	return (sPath_.find_last_of(_T('/')) == (sPath_.size()-1));
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
	return sPath_;
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