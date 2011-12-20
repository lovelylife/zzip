#include "StdAfx.h"
#include "filehelper.h"
#include <io.h>
#include <direct.h>

bool base::utils::filehelper::CreateFolder(tstring sPath, bool bFile) {
	tstring pathTmp = sPath;
	//format path string to unix style.
	for (size_t i=0; i<pathTmp.size(); ++i)
	{
		if (pathTmp[i] == _T('\\'))
			pathTmp[i] = _T('/');
	}
	if (bFile)
		pathTmp = pathTmp.substr(0, pathTmp.find_last_of(_T("/"))+1);
	else
		pathTmp += _T('/');
	//mkdir begin
	TCHAR* t = const_cast<TCHAR*>(pathTmp.c_str());
	while (t = _tcschr(++t, _T('/'))) {
		*t = 0;
		if (_taccess(pathTmp.c_str(), 0) == -1)
		{
			_tmkdir(pathTmp.c_str());
		}
		
		*t = _T('/');
	}
	return true;
}

bool base::utils::filehelper::IsFileExists(const tstring sFileName) {
	return (_taccess(sFileName.c_str(), 0) != -1);
}