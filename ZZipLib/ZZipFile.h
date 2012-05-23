
#ifndef __ZZIPFILE_H__
#define __ZZIPFILE_H__

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


#include "ZZipTree.h"
#include "ZZipFileObject.h"

#ifdef _WIN32
#include <objidl.h>
#endif

#include <fstream>
#include <sstream>


#ifndef ZZIP_Writer
#define ZZIP_Writer
#endif

#ifndef ZZIP_Reader
#define ZZIP_Reader
#endif


class _EXT_CLASS ZZipFile {
public:

  // 文件打开模式， OpenModeRead为只读，写函数则不会支持
  // OpenModeWrite即为写模式，读函数不支持， 可以使用‘|’
  // 或操作符进行组合
  static const int OpenModeRead			= 0x00000001;
  static const int OpenModeWrite		= 0x00000002;

  // zzip文件流的类型，默认为未知
  static const int TypeUnknow			= 0;
  static const int TypeStream			= 1;
  static const int TypeFile				= 2;

  static const int FilterModeFile		= 0x0001;
  static const int FilterModeFolder		= 0x0002;
  static const int FilterModeAll		= 0x0003;
  
  //typedef bool (*EnumFileFunction)(void* arg, const int ftype, const tstring& sItem);

  class ZZipFileTree 
	: public ZZipTree< tstring, RefPtr<ZZipFileObject> >
  {
  public:
	typedef ZZipTree< tstring, RefPtr<ZZipFileObject> > _MyBase;
	typedef _MyBase::PathType PathType;
	typedef _MyBase::ValueType ValueType;
	typedef _MyBase::ValueTravEvent ValueTravEvent;
  // 静态方法
  public:
	static bool String2Path(const tstring&, PathType&);
	static bool Path2String(const PathType&, tstring&);
  
  public:
    void DoTrav(const PathType&, const ValueTravEvent&, bool Recursive=true);
    bool ZZipTrav(NodePtr);

  // 构造析构
  public:
	ZZipFileTree();
    ~ZZipFileTree();
	
  private:
	  _MyBase::NodeTravEvent ZZipTravEvent;
	  ValueTravEvent ZZipValueTravEvent;
  };

public:
  // 标准构造函数
  ZZipFile(void);
  // 标准析构函数
  ~ZZipFile(void);

  friend class ZZipFileTree;

// 静态方法
public:
  // 删除本地文件夹
  static bool RemoveDir(const tstring& sLocalDir);

// 方法
public: 
  // 处理命令行模式
  // uint32 Command(int argc, TCHAR* argv[]);

  // 使用现有的ZZipFile文件流
  bool Attach(std::iostream* pStream);

  // 打开ZZip文件
  bool Open(const tstring& lpszFileName, int OpenMode = OpenModeRead|OpenModeWrite);

#ifdef _WIN32
  bool Open(HINSTANCE hInstance, const tstring& sType, unsigned int pszResourceName);
#endif

  // 关闭ZZip文件
  void Close();

  bool IsGood() const ;

  tstring FileName() const;

// 文件操作方法
public:
  // 判断文件夹是否不存在
  static bool PathIsInValid(const tstring& sPath);

  // 枚举文件或者文件夹
  void EnumItem(const tstring& sZZipFolderPath, const ZZipFileTree::ValueTravEvent& evt, bool Recursive = true);

  // 通过路径查找文件
  RefPtr<ZZipFileObject> FindFile(const tstring& lpszZZipPath);

  // 读取指定ZZipFileObject的数据， 并返回读取数据的大小。提供这个方法的
  // 目的在于读取大文件（尽管这种情况出现的不是很平凡，但是不可避免）；
  // 因为无法申请足够的内存用于存储文件的内容，所以提供读写数据块的方式，
  // 解决大文件的存储。如果仅仅只是读取几百k或者几十k的文件，你可以直接
  // 申请相应的文件大小的内存(zzipobject.filesize()来获得)
  // 示例读取小文件(1-1024K不等) 例如读取文件"/4.gif" 保存到 "C:\\tt.gif"
  // ZZipFile zzip;
  // zzip.Open(filename);
  // std::ofstream of(_T("C:\\tt.xml"), std::ios::out|std::ios::binary);
  // if(of.good()) {
  //		const ZZipFileObject* p = zzip.Find(_T("/3.txt"));
  // 		if(p) {
  // 			uint64 filesize = p->filesize();
  // 			char *filebuffer = new char[filesize];
  // 			uint64 ReadBytes = zzip.ReadData(p, 0, (void*)buffer, filesize);
  //			...
  //			delete filebuffer;
  // 		}
  // 		of.close();
  // 	}

  int64 ReadData(const ZZipFileObject* zzipfile, int64 offset, void* lpBuffer, int64 size);

  // 读取文件到内存
  bool ExtractFile(const tstring& sZZipPath,IStream** pStream);

  // 保存ZZip文件
  ZZIP_Writer bool Save();

  // 清空所有文件
  ZZIP_Writer void Clear();

  // 添加文件夹
  ZZIP_Writer bool AddFolder(tstring sZZipPath, tstring sLocalFolder);

  // 添加文件
  ZZIP_Writer RefPtr<ZZipFileObject> AddFile(const tstring& sZZipPath, const tstring& sLocalFileName, bool bOverwrite = true);

#ifdef _WIN32
  // 添加文件
  ZZIP_Writer bool AddFile(const tstring& sZZipPath, IStream* pStream, bool bOverwrite = true);

#endif 
  // 删除文件夹
  ZZIP_Writer void RemoveFile(const tstring& sZZipPath);

  // 重命名
  ZZIP_Writer bool RenameFile(const tstring& sOldPath, const tstring& sNewPath);

private:

  // 反序列化
  bool Parse(std::iostream* pStream);

  // 序列化ZZipFileObject到文件
  bool WriteFileObject(const ZZipFileTree::PathType& path, const ZZipFileTree::ValueType&);

//   void BeginSerialize();
//   void EndSerialize();

private:
  ZZipFileTree::ValueTravEvent WriteFileObjectEvent;

private:
  // ZZipFile流类型
  int Type_;

  // 读写类型
  int OpenMode_;

  // 错误代码
  int ErrorCode_;

  // 文件或者字符流
  std::iostream*  StreamPtr_;

  // 缓冲文件，最后将替换成ZZip文件，将原有的删除
  std::ofstream*  StreamWriterPtr_;

  // 存档结构缓冲，最后写入StreamWriterPtr_缓冲文件
  std::stringstream  ArchiveWriter_;

  // 如果为空说明是字符流，否则为文件流
  tstring sZZipFileName_;

  // 临时缓存文件夹
  tstring sCacheDir_;

  // ZZipFile文件头部，跟通常的文件一样，包含了文件的相关信息
  ZZipFileHeader ZZipFileHeader_;

  // 文件二叉树
  ZZipFileTree FileObjectsTree_;

};

// 用于枚举文件
// template<class _Traits>
// void ZZipEnum(ZZipFile& zzipfile, const tstring& sZZipPath, bool bRescuire = false) {
// 	_Traits e;
// 	ZZipFile::ZZipFileTree::ValueTravEvent evt;
// 	evt += std::make_pair(&e, &_Traits::EnumFile);
// 	zzipfile.EnumItem(sZZipPath, evt, bRescuire);
// 	evt -= std::make_pair(&e, &_Traits::EnumFile);
// }

#endif
