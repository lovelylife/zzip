
#ifndef __ZZIP_H__
#define __ZZIP_H__

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
// 	|	    file indexs	 	 |
// 	|________________________|
// 	|						 |
// 	|		header			 |
// 	|________________________|
//
//////////////////////////////////////////////////////////////////////////

#include <list>
#include <fstream>
#include "ZZipFile.h"
#include "ZZipFileObject.h"


class ZZipFile {
public:
	typedef std::list<ZZipFileObject*>		ZZipFileObjects;

public:
	ZZipFile(void);
	~ZZipFile(void);

public: 
// ����������ģʽ
//	uint32 Command(int argc, TCHAR* argv[]);

// ��ZZip�ļ�
	bool Open(const tstring& lpszFileName );

// ����ZZip�ļ�
	bool Save();

// �ر�ZZip�ļ�
	void Close();

// ����ļ���
	bool AddFolder(const tstring& sZZipPath, const tstring& sLocalFolder);

// ����ļ�
	bool AddFile(const tstring& sZZipPath, const tstring& sLocalFileName);

// ɾ���ļ���
	refptr<ZZipFileObject> RemoveFile(const tstring& sZZipPath);

// ������
	bool RenameFile(const tstring& sOldPath, const tstring& sNewPath);

// ͨ��·�����Ҳ���ȡ�ļ�
	bool ExtractFile(const tstring& lpszPath);

protected:
	
	
private:
	bool ParseFileHeader();

private:
	ZZipFileHeader	ZZipFileHeader_;
	ZZipFileObjects FileObjects_;
	std::ifstream FileStreamReader_;
	tstring sZZipFileName_;
};


#endif
