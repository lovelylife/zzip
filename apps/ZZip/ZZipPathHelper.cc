/*********************************************
	����CBrowseDir����ʵ���ļ� BrowseDir.cpp 
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
	//�Ȱ�dirת��Ϊ����·�� 
	TCHAR szFullPath[_MAX_PATH] = {0};
	memset((void*)szFullPath, 0, sizeof(szFullPath));
	if (_tfullpath(szFullPath, sDir.c_str(),_MAX_PATH) == NULL) 
		return false; 

	sFolderPath_ = szFullPath;

	//�ж�Ŀ¼�Ƿ���� 
	if (_tchdir(sFolderPath_.c_str()) != 0) 
		return false; 

	//���Ŀ¼�����һ����ĸ����'\',����������һ��'\' 
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

	//���Ȳ���dir�з���Ҫ����ļ� 
	long hFile; 
	_tfinddata_t FileData; 
	if ((hFile=_tfindfirst(sFilespec.c_str(),&FileData)) != -1) { 
		do { 
			//����ǲ���Ŀ¼ 
			//�������,����д��� 
			if (!(FileData.attrib & _A_SUBDIR)) { 
				tstring sFileName;
				sFileName += sDir;
				sFileName += FileData.name;
				if (!ProcessFile(sFileName)) return false; 
			} 
		} while (_tfindnext(hFile, &FileData) == 0); 
		_findclose(hFile);
	} 

	//����dir�е���Ŀ¼ 
	//��Ϊ�ڴ���dir�е��ļ�ʱ���������ProcessFile�п��ܸı��� 
	//��ǰĿ¼����˻�Ҫ�������õ�ǰĿ¼Ϊdir�� 
	//ִ�й�_findfirst�󣬿���ϵͳ��¼���������Ϣ����˸ı�Ŀ¼ 
	//��_findnextû��Ӱ�졣 
	_tchdir(sDir.c_str()); 
	if ((hFile=_tfindfirst(_T("*.*"), &FileData)) != -1) { 
		do { 
			//����ǲ���Ŀ¼ 
			//�����,�ټ���ǲ��� . �� .. 
			//�������,���е��� 
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