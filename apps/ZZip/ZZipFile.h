
#ifndef __ZZIP_H__
#define __ZZIP_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
// ZZip文件压缩
// 文件数据结构示意图
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
// 处理命令行模式
//	uint32 Command(int argc, TCHAR* argv[]);

// 打开ZZip文件
	bool Open(const tstring& lpszFileName );

// 保存ZZip文件
	bool Save();

// 关闭ZZip文件
	void Close();

// 添加文件夹
	bool AddFolder(tstring sZZipPath, tstring sLocalFolder);

// 添加文件
	bool AddFile(const tstring& sZZipPath, const tstring& sLocalFileName);

// 删除文件夹
	refptr<ZZipFileObject> RemoveFile(const tstring& sZZipPath);

// 重命名
	bool RenameFile(const tstring& sOldPath, const tstring& sNewPath);

// 通过路径查找文件
	const ZZipFileObject* Find(const tstring& lpszPath);

// 读取指定ZZipFileObject的数据， 并返回读取数据的大小。提供这个方法的
// 目的在于读取大文件（尽管这种情况出现的不是很平凡，但是不可避免）；
// 因为无法申请足够的内存用于存储文件的内容，所以提供读写数据块的方式，
// 解决大文件的存储。如果仅仅只是读取几百k或者几十k的文件，你可以直接
// 申请相应的文件大小的内存(zzipobject.filesize()来获得)
// 示例读取小文件(1-1024K不等)// 
// 读取文件"/4.gif" 保存到 "C:\\tt.gif"
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
