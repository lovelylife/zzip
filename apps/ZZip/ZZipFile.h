
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
	bool Open(const tstring& lpszFileName );
	
	bool AddFile(const tstring& lpszZZipPath, const tstring& lpszLocalFileName );
	refptr<ZZipFileObject> RemoveFile(const tstring& lpszZZipPath);
	// 通过路径查找并提取文件
	bool ExtractFile(const tstring& lpszPath);
	bool Save();
	void Close();
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
