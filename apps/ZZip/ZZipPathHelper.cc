/*********************************************
	这是CBrowseDir的类实现文件 BrowseDir.cpp 
***********************************************/ 

#include <direct.h>
#include <string.h>
#include <io.h>
#include <algorithm>

#include "ZZipPathHelper.h" 

ZZipPathHelper::ZZipPathHelper(const tstring& sPath, bool bSubFolder) 
: sFolderPath_(sPath)
, bSubFolder_(bSubFolder)
{
} 

bool ZZipPathHelper::SetInitDir(const tstring& sDir) 
{ 
	//先把dir转换为绝对路径 
	TCHAR szFullPath[_MAX_PATH] = {0};
	memset((void*)szFullPath, 0, sizeof(szFullPath));
	if (_tfullpath(szFullPath, sDir.c_str(),_MAX_PATH) == NULL) 
		return false; 

	sFolderPath_ = szFullPath;

	//判断目录是否存在 
	if (_tchdir(sFolderPath_.c_str()) != 0) 
		return false; 

	//如果目录的最后一个字母不是'\',则在最后加上一个'\' 
	std::replace(sFolderPath_.begin(), sFolderPath_.end(), _T('\\'), _T('/'));
	if(sFolderPath_[sFolderPath_.size()-1] != _T('/')) {
		sFolderPath_.append(1, _T('/'));
	}

	return true; 
} 

bool ZZipPathHelper::BeginBrowse(const tstring& sFilespec) 
{ 
	ProcessDir(sFolderPath_,NULL); 
	return BrowseDir(sFolderPath_, sFilespec); 
} 

bool ZZipPathHelper::BrowseDir (const tstring& sDir, const tstring& sFilespec) 
{ 
	_tchdir(sDir.c_str()); 

	//首先查找dir中符合要求的文件 
	long hFile; 
	_tfinddata_t FileData; 
	if ((hFile=_tfindfirst(sFilespec.c_str(),&FileData)) != -1) { 
		do { 
			//检查是不是目录 
			//如果不是,则进行处理 
			if (!(FileData.attrib & _A_SUBDIR)) { 
				tstring sFileName;
				sFileName += sDir;
				sFileName += FileData.name;
				if (!ProcessFile(sFileName)) return false; 
			} 
		} while (_tfindnext(hFile, &FileData) == 0); 
		_findclose(hFile);
	} 

	//查找dir中的子目录 
	//因为在处理dir中的文件时，派生类的ProcessFile有可能改变了 
	//当前目录，因此还要重新设置当前目录为dir。 
	//执行过_findfirst后，可能系统记录下了相关信息，因此改变目录 
	//对_findnext没有影响。 
	_tchdir(sDir.c_str()); 
	if ((hFile=_tfindfirst(_T("*.*"), &FileData)) != -1) { 
		do { 
			//检查是不是目录 
			//如果是,再检查是不是 . 或 .. 
			//如果不是,进行迭代 
			if ((FileData.attrib & _A_SUBDIR)) { 
				if (_tcscmp(FileData.name, _T(".")) != 0 
					&& _tcscmp(FileData.name, _T("..")) != 0) 
				{ 
					tstring sSubDir;
					sSubDir.append(sDir.c_str());
					sSubDir.append(FileData.name);
					sSubDir.append(_T("/"));
					ProcessDir(sSubDir, sDir); 
					if (!BrowseDir(sSubDir, sFilespec)) return false; 
				} 
			} 
		} while (_tfindnext(hFile,&FileData) == 0); 
		_findclose(hFile); 
	} 
	return true; 
} 

bool ZZipPathHelper::ProcessFile(const tstring& sFileName) 
{
	return true; 
} 

void ZZipPathHelper::ProcessDir(const tstring& sCurrentDir, const tstring& sParentDir) {}