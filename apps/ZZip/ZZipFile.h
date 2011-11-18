
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
	bool AddFolder(tstring sZZipPath, tstring sLocalFolder);

// ����ļ�
	bool AddFile(const tstring& sZZipPath, const tstring& sLocalFileName);

// ɾ���ļ���
	refptr<ZZipFileObject> RemoveFile(const tstring& sZZipPath);

// ������
	bool RenameFile(const tstring& sOldPath, const tstring& sNewPath);

// ͨ��·�������ļ�
	const ZZipFileObject* Find(const tstring& lpszPath);

// ��ȡָ��ZZipFileObject�����ݣ� �����ض�ȡ���ݵĴ�С���ṩ���������
// Ŀ�����ڶ�ȡ���ļ�����������������ֵĲ��Ǻ�ƽ�������ǲ��ɱ��⣩��
// ��Ϊ�޷������㹻���ڴ����ڴ洢�ļ������ݣ������ṩ��д���ݿ�ķ�ʽ��
// ������ļ��Ĵ洢���������ֻ�Ƕ�ȡ����k���߼�ʮk���ļ��������ֱ��
// ������Ӧ���ļ���С���ڴ�(zzipobject.filesize()�����)
// ʾ����ȡС�ļ�(1-1024K����)// 
// ��ȡ�ļ�"/4.gif" ���浽 "C:\\tt.gif"
// 	ZZipFile zzip;
// 	zzip.Open(filename);
// 	std::ofstream of(_T("C:\\tt.xml"), std::ios::out|std::ios::binary);
// 	if(of.good()) {
// 		const ZZipFileObject* p = zzip.Find(_T("/3.txt"));
// 		if(p) {
// 			uint64 filesize = p->filesize();
// 			char *filebuffer = new char[filesize];
// 			uint64 ReadBytes = zzip.ReadData(p, 0, (void*)buffer, filesize);
// 		}
// 		of.close();
// 	}

	int64 ReadData(const ZZipFileObject* zzipfile, uint64 offset, void* lpBuffer, uint64 size);

protected:
	
	
private:
	ZZipFileHeader	ZZipFileHeader_;
	ZZipFileObjects FileObjects_;
	std::ifstream StreamReader_;
	tstring sZZipFileName_;
};


#endif
