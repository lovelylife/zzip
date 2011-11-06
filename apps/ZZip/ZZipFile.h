
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
	bool AddFolder(const tstring& sZZipPath, const tstring& sLocalFolder);

// 添加文件
	bool AddFile(const tstring& sZZipPath, const tstring& sLocalFileName);

// 删除文件夹
	refptr<ZZipFileObject> RemoveFile(const tstring& sZZipPath);

// 重命名
	bool RenameFile(const tstring& sOldPath, const tstring& sNewPath);

// 通过路径查找并提取文件
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
