
#ifndef __ZZIPFILEOBJECT_H__
#define __ZZIPFILEOBJECT_H__

#pragma once

#include "ref_counted.h"
#include "ZZip.h"

class _EXT_CLASS ZZipFileObject : public base::RefCountedThreadSafe<ZZipFileObject> {
public:
	ZZipFileObject();
	~ZZipFileObject(void);

	bool ZZipPathFromPath( const tstring& sPath);
	bool isfolder();
	bool isstream();
	int64 offset() const;
	int64 filesize() const;
	tstring path() const;
	tstring localpath() const;
	int32 createtime() const;
	int32 lastmodify() const;

	friend class ZZipFile;
private: 
	ZZipFileItem FileItem_;
	tstring sPath_;			// 路径
	tstring sLocalPath_;	// 本地路径，如果为空则为ZZip文件文具

};

#endif
