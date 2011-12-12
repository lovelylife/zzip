
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

class _EXT_CLASS ZZipFile {
public:
  typedef std::list<ZZipFileObject*>		ZZipFileObjects;

  // �ļ���ģʽ�� OpenModeReadΪֻ����д�����򲻻�֧��
  // OpenModeWrite��Ϊдģʽ����������֧�֣� ����ʹ�á�|��
  // ��������������
  static const int OpenModeRead			= 0x00000001;
  static const int OpenModeWrite		= 0x00000002;

  // zzip�ļ��������ͣ�Ĭ��Ϊδ֪
  static const int ZZipTypeUnknow		= 0;
  static const int ZZipTypeStream		= 1;
  static const int ZZipTypeFile			= 2;

public:
  // ��׼���캯��
  ZZipFile(void);
  // ��׼��������
  ~ZZipFile(void);

public: 
  // ����������ģʽ
  // uint32 Command(int argc, TCHAR* argv[]);

  // ʹ�����е�ZZipFile�ļ���
  bool Attach(std::iostream* pStream);

  // ��ZZip�ļ�
  bool Open(const tstring& lpszFileName );

#ifdef _WIN32
  bool Open(HINSTANCE hInstance, const tstring& sType, unsigned int pszResourceName);
#endif

  // ����ZZip�ļ�
  bool Save();

  // �ر�ZZip�ļ�
  void Close();

  // ��������ļ�
  void Clear();

  // ����ļ���
  bool AddFolder(tstring sZZipPath, tstring sLocalFolder);

  // ����ļ�
  bool AddFile(const tstring& sZZipPath, const tstring& sLocalFileName);

  // ɾ���ļ���
  refptr<ZZipFileObject> RemoveFile(const tstring& sZZipPath);

  // ������
  bool RenameFile(const tstring& sOldPath, const tstring& sNewPath);

  // ͨ��·�������ļ�
  const ZZipFileObject* FindFile(const tstring& lpszPath);

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

  int64 ReadData(const ZZipFileObject* zzipfile, uint64 offset, void* lpBuffer, uint64 size);

  // ��ȡ�ļ����ڴ�
  bool ExtractFile(const tstring& sZZipPath,IStream** pStream);

private:
  bool Parse(std::iostream* pStream);
	
private:
  // ZZipFile�ļ�ͷ������ͨ�����ļ�һ�����������ļ��������Ϣ
  ZZipFileHeader	ZZipFileHeader_;

  // �ļ������б�
  ZZipFileObjects FileObjects_;

  // �ļ������ַ���
  std::iostream*  StreamPtr_;

  // ���Ϊ��˵�����ַ���������Ϊ�ļ���
  tstring sZZipFileName_;

};


#endif
