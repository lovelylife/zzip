
#ifndef __ZZIPFILEOBJECT_H__
#define __ZZIPFILEOBJECT_H__

#pragma once

#include "ZZipLib.h"

class ZZipFileObject {
public:
	ZZipFileObject(const ZZipFileItem& FileItem, std::string& sName);
	~ZZipFileObject(void);

public: 

	
	
private:
	ZZipFileItem FileItem_;
	std::string sParentPath_;	// end with character '/'
	std::string sName_;			// 
	bool bFolder_;				// false => file, true : folder
	bool bValid_;				//  «∑Ò”––ß
};


#endif
