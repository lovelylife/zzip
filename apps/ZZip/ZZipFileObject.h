
#ifndef __ZZIPFILEOBJECT_H__
#define __ZZIPFILEOBJECT_H__

#pragma once

#include "ref_counted.h"
#include "ZZip.h"

class ZZipFileObject : public base::RefCountedThreadSafe<ZZipFileObject> {
public:
	ZZipFileObject();
	~ZZipFileObject(void);

	void ZZipPathFromPath( const tstring& sPath);
	friend class ZZipFile;
private: 
	ZZipFileItem FileItem_;
	tstring sParentPath_;	// end with character '/'
	tstring sName_;			// 
	tstring sLocalPath_;	// 本地路径，如果为空则为ZZip文件文具

};

#endif
