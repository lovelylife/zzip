
#include "ZZip.h"
#include <iostream>
#include <fstream>
#include <atlbase.h>
#include <atlconv.h>
#include "ZZipCommand.h"


#define ZZIP_SIG "ZZip"


ZZipFile::ZZipFile(void)
{
}

ZZipFile::~ZZipFile(void)
{
}

// public
// int ZZip::Command( int argc, _TCHAR* argv[] ) {
// 	ZZipCommand cmd(argc, argv);
// 	return 0;
// }

bool ZZipFile::Open( const _TCHAR* lpszFileName ) {
	if(NULL == lpszFileName ) return false;
	std::string sFileName = CT2A(lpszFileName);
	std::fstream f;
	f.open(lpszFileName);
	if(!f.is_open()) {
		std::cout << "Open file " << sFileName.c_str() << " error." << std::endl;
		return false;
	} 
	
	std::cout << "Open file " << sFileName.c_str() << " success." << std::endl;

	f.seekg(f.beg);
	f.seekg(f.end);
	size_t filesize = f.tellg();
	ZZipHeader hdr = {0};
	if(filesize < sizeof(ZZipHeader)) {
		std::cout << "Invalid file data!" << std::endl;
		return false;
	}
	
	// ��ȡ�ļ�ͷ����
	f.seekg(f.beg);
	f.read((char*)&hdr, sizeof(ZZipHeader));

	// ��ȡĿ¼�ṹ, ��������XML�ĵ�����
	f.seekg(hdr.offset);
	{
		ZZipFileItem item = {0};
		f.read((char*)&item, std::streamsize(sizeof(ZZipFileItem))-1);
		// ��ȡ·��
		char* BufferPtr = new char(item.namelen+1);
		memset(BufferPtr, 0, item.namelen);
		f.read(BufferPtr, item.namelen);
		ZZipFileObject FileObject();
//		FileObject.FileItem_ = item;
//		FileObjects_.push_back()
	}
	return true;
}

// �����ļ�
bool ZZipFile::Save()
{
	std::fstream f;
	// ������ʱ�ļ�
	
	// ��д�ļ�ͷ

	// д���ļ�����

	// д��Ŀ¼�ṹ

	return true;
}

bool ZZipFile::RemoveFile( const _TCHAR* lpszZZipPath )
{
	return true;
}

bool ZZipFile::AddFile( const _TCHAR* lpszZZipPath, const _TCHAR* lpszLocalFileName )
{
	return true;
}
