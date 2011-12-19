
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
// 	|	    file indexs	 	 |
// 	|________________________|
// 	|						 |
// 	|		header			 |
// 	|________________________|
//
//////////////////////////////////////////////////////////////////////////

#include <list>
#include <fstream>
#include "ZZipFileObject.h"

#ifdef _WIN32
#include <objidl.h>
#endif

#ifndef ZZIP_Writer
#define ZZIP_Writer
#endif

#ifndef ZZIP_Reader
#define ZZIP_Reader
#endif


class _EXT_CLASS ZZipFile {
public:

  // �ļ���ģʽ�� OpenModeReadΪֻ����д�����򲻻�֧��
  // OpenModeWrite��Ϊдģʽ����������֧�֣� ����ʹ�á�|��
  // ��������������
  static const int OpenModeRead			= 0x00000001;
  static const int OpenModeWrite		= 0x00000002;

  // zzip�ļ��������ͣ�Ĭ��Ϊδ֪
  static const int TypeUnknow			= 0;
  static const int TypeStream			= 1;
  static const int TypeFile				= 2;

  typedef std::list<ZZipFileObject*>		ZZipFileObjects;

public:
  // ��׼���캯��
  ZZipFile(void);
  // ��׼��������
  ~ZZipFile(void);

// ����
public: 
  // ����������ģʽ
  // uint32 Command(int argc, TCHAR* argv[]);

  // ʹ�����е�ZZipFile�ļ���
  bool Attach(std::iostream* pStream);

  // ��ZZip�ļ�
  bool Open(const tstring& lpszFileName, int OpenMode = OpenModeRead|OpenModeWrite);

#ifdef _WIN32
  bool Open(HINSTANCE hInstance, const tstring& sType, unsigned int pszResourceName);
#endif

  // �ر�ZZip�ļ�
  void Close();

// �ļ���������
public:
  // �ж��ļ����Ƿ񲻴���
  static bool PathIsInValid(const tstring& sPath);
  // ͨ��·�������ļ�
  refptr<ZZipFileObject> FindFile(const tstring& lpszZZipPath);

  // ��ȡָ��ZZipFileObject�����ݣ� �����ض�ȡ���ݵĴ�С���ṩ���������
  // Ŀ�����ڶ�ȡ���ļ�����������������ֵĲ��Ǻ�ƽ�������ǲ��ɱ��⣩��
  // ��Ϊ�޷������㹻���ڴ����ڴ洢�ļ������ݣ������ṩ��д���ݿ�ķ�ʽ��
  // ������ļ��Ĵ洢���������ֻ�Ƕ�ȡ����k���߼�ʮk���ļ��������ֱ��
  // ������Ӧ���ļ���С���ڴ�(zzipobject.filesize()�����)
  // ʾ����ȡС�ļ�(1-1024K����) �����ȡ�ļ�"/4.gif" ���浽 "C:\\tt.gif"
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

  // ��ȡ�ļ����ڴ�
  bool ExtractFile(const tstring& sZZipPath,IStream** pStream);

  // ����ZZip�ļ�
  ZZIP_Writer bool Save();

  // ��������ļ�
  ZZIP_Writer void Clear();

  // ����ļ���
  ZZIP_Writer bool AddFolder(tstring sZZipPath, tstring sLocalFolder);

  // ����ļ�
  ZZIP_Writer refptr<ZZipFileObject> AddFile(const tstring& sZZipPath, const tstring& sLocalFileName, bool bOverwrite = true);

#ifdef _WIN32
  // ����ļ�
  ZZIP_Writer refptr<ZZipFileObject> AddFile(const tstring& sZZipPath, IStream* pStream, bool bOverwrite = true);

#endif 
  // ɾ���ļ���
  ZZIP_Writer refptr<ZZipFileObject> RemoveFile(const tstring& sZZipPath);

  // ������
  ZZIP_Writer bool RenameFile(const tstring& sOldPath, const tstring& sNewPath);

private:

  bool Parse(std::iostream* pStream);
	
  // ɾ�������ļ���
  bool RemoveDir(tstring sLocalDir);
private:

  // ZZipFile������
  int Type_;

  // ��д����
  int OpenMode_;

  // �������
  int ErrorCode_;

  // �ļ������ַ���
  std::iostream*  StreamPtr_;

  // �����ļ�������滻��ZZip�ļ�����ԭ�е�ɾ��
  std::ofstream*  StreamWriterPtr_;

  // ���Ϊ��˵�����ַ���������Ϊ�ļ���
  tstring sZZipFileName_;

  // ��ʱ�����ļ���
  tstring sCacheDir_;

  // ZZipFile�ļ�ͷ������ͨ�����ļ�һ�����������ļ��������Ϣ
  ZZipFileHeader ZZipFileHeader_;

  // �ļ������б�
  ZZipFileObjects FileObjects_;





};


#endif
