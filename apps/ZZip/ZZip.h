
#ifndef __ZZIPFILE_H__
#define __ZZIPFILE_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
// ZZip�ļ�ѹ��
// �ļ����ݽṹʾ��ͼ
//
// 	 ________________________
// 	|						 |
// 	|						 |
// 	|	files list data		 |
// 	|						 |
// 	|________________________|
// 	|						 |
// 	|		XML	Archive 	 |
// 	|________________________|
// 	|						 |
// 	|		header			 |
// 	|________________________|
//
//////////////////////////////////////////////////////////////////////////

#include <../base/typedefs.h>
#include <list>
#include "ZZipLib.h"
#include "ZZipFileObject.h"

class ZZipFile {
public:
	typedef std::list<ZZipFileObject>		FILEOBJECTS;

public:
	ZZipFile(void);
	~ZZipFile(void);

public: 
	// ����������ģʽ
	uint32 Command(int argc, _TCHAR* argv[]);
	bool Open(const _TCHAR* lpszFileName );
	bool Save();
	bool AddFile(const _TCHAR* lpszZZipPath, const _TCHAR* lpszLocalFileName );
	bool RemoveFile(const _TCHAR* lpszZZipPath);
	// ͨ��·�����Ҳ���ȡ�ļ�
	bool ExtractFile(const _TCHAR* lpszPath);
protected:
	
	
private:
	bool ParseFileHeader();

private:
	ZZipHeader	ZZipHeader_;
	FILEOBJECTS FileObjects_;
};


#endif
