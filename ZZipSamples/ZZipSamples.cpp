// ZZipSamples.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
//#include "Color.h"
#include <atlconv.h>

bool EnumFileProc(void*, const tstring&);

int _tmain(int argc, _TCHAR* argv[]) {

// #ifdef USING_TestCode
// 	QUI::Color::doTest();
// #endif
//	std::cout << "value is A = " << (int)((unsigned char)(value >> 24)) << std::endl;
// 	unsigned long value = 0x01020304;
// 	std::cout << "value is R" << (int)((unsigned char)value) << std::endl;
// 	std::cout << "value is G" << (int)((unsigned char)(value >> 8)) << std::endl;
// 	std::cout << "value is B" << (int)((unsigned char)(value >> 16)) << std::endl;
// 	std::cout << "value is A" << (int)((unsigned char)(value >> 24)) << std::endl;
	
//	return 0;

	tstring sPath = _T("a/b/c/d/e.htm");
	ZZipFile::ZZipFileTree::PathType path;
	ZZipFile::ZZipFileTree::String2Path(sPath, path);
	tstring sPathOutput;
	ZZipFile::ZZipFileTree::Path2String(path,sPathOutput);

	bool bRet = true;
	// TODO:
	// do something
	std::cout << "ZZipApp is running" << std::endl;
	wchar_t WCharString[] = {0xff,0x22,0xad,0x33,0x00};
	tstring sString = _T("abcdefghijklmn");
	sString.append(1, _T('\0'));
	sString.append(1, _T('\0'));
	sString.append(1, _T('\0'));
	std::cout << wcslen(sString.c_str()) << std::endl;

	std::cout << sString.size() << std::endl;
	std::cout << sizeof(ZZipFileItem) << std::endl;
	ZZipFile zzip;
	if(argc <= 1) { return 0; }
	if(zzip.Open(argv[1])) {
		zzip.AddFolder(_T("/"), _T("D:\\test"));
		zzip.AddFile(_T("/1.txt"), _T("C:\\1.txt"));
		zzip.AddFile(_T("/2.txt"), _T("C:\\2.txt"));
		zzip.AddFile(_T("/3.txt"), _T("C:\\3.txt"));
		zzip.AddFile(_T("/4.gif"), _T("C:\\4.gif"));
		zzip.Save();
		zzip.Close();
	}

	if(zzip.Open(argv[1])) {

		// 枚举文件
//		zzip.EnumFile(_T("/"), 0, EnumFileProc);

		// 读取文件"/4.gif" 保存到 "C:\\tt.gif"
// 		std::ofstream of(_T("C:\\tt.gif"), std::ios::out|std::ios::binary);
// 		if(of.good()) {
// 			const ZZipFileObject* p = zzip.FindFile(_T("/4.gif"));
// 			char buffer[1024] = {0};
// 			int64 filesize = p->filesize();
// 			int64 startpos = 0;
// 			int64 fcount = filesize;
// 			while(fcount > 0) {
// 				int64 ReadBytes = zzip.ReadData(p, startpos, (void*)buffer, 1024);
// 				if(ReadBytes > 0) {
// 					fcount -= ReadBytes;
// 					startpos += ReadBytes;
// 					of.write(buffer, ReadBytes);
// 				} else {
// 					break;
// 				}
// 			}
// 			of.close();
// 		}
		zzip.Close();
	}
	return 0;
}

bool EnumFileProc(void*, const tstring& sFileName) {

	std::wcout << sFileName.c_str() << std::endl;
	return true;
}