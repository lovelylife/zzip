// ZZipSamples.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
//#include "Color.h"
#include <atlconv.h>

// 枚举文件
class EnumItems {
public:
	EnumItems(ZZipFile& zzip, const tstring& sPath, bool bRescuire = false) {
		ZZipFile::ZZipFileTree::ValueTravEvent evt;
		evt += std::make_pair(this, &EnumItems::EnumFile);
		zzip.EnumItem(sPath, evt, bRescuire);
		evt -= std::make_pair(this, &EnumItems::EnumFile);
	}

public:
	bool EnumFile(const ZZipFile::ZZipFileTree::PathType& path, const ZZipFile::ZZipFileTree::ValueType&) {
		tstring sPath;
		ZZipFile::ZZipFileTree::Path2String(path, sPath);
		_tprintf(_T("EnumFile() file: %s\n"), sPath.c_str());
		return true;
	}
};

struct tests {
	bool a1 : 1;
	bool a2 : 1;
	bool a3 : 1;
	bool a4 : 1;
	bool a5 : 1;
	bool a6 : 1;
	bool a7 : 1;
	bool a8 : 1;
};

int _tmain(int argc, _TCHAR* argv[]) {

	struct tests ttt = {0};
	ttt.a7 = true;
	printf("size: %d, value=%d\n", sizeof(ttt), ttt);

	//return 0;
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

// 	tstring sPath = _T("a/b/c/d/e.htm");
// 	ZZipFile::ZZipFileTree::PathType path;
// 	ZZipFile::ZZipFileTree::String2Path(sPath, path);
// 	tstring sPathOutput;
// 	ZZipFile::ZZipFileTree::Path2String(path,sPathOutput);
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
	// if(argc <= 1) { return 0; }
	tstring sZZipFileName = _T("D:/Developing/VCCode/trunk/QUI/QUITest/res/skin.zzip");
	// if(zzip.Open(argv[1])) {
	if(zzip.Open(sZZipFileName)) {
		zzip.AddFolder(_T("/"), _T("D:/Developing/VCCode/trunk/QUI/QUITest/res/skin"));
// 		zzip.AddFile(_T("/1.txt"), _T("C:\\1.txt"));
// 		zzip.AddFile(_T("/2.txt"), _T("C:\\2.txt"));
// 		zzip.AddFile(_T("/3.txt"), _T("C:\\3.txt"));
// 		zzip.AddFile(_T("/4.gif"), _T("C:\\4.gif"));
		zzip.Save();
		zzip.Close();
	}

	ZZipFile zzip2;
	if(zzip2.Open(sZZipFileName)) {

		// 枚举文件
		EnumItems e(zzip2, _T("/"), true);
// 		ZZipFile::ZZipFileTree::ValueTravEvent evt;
// 		evt += std::make_pair(&e, &EnumItems::EnumFile);
// 		zzip.EnumItem(_T("/"), evt, false);
// 		evt -= std::make_pair(&e, &EnumItems::EnumFile);

//		ZZipEnum<EnumItems>(zzip2, _T("/drawables"), false);
// 
// 		// 读取文件"/4.gif" 保存到 "C:\\tt.gif"
// 		RefPtr<ZZipFileObject> p = zzip.FindFile(_T("/4.gif"));
// 		if(NULL != p) {
// 			std::ofstream of(_T("C:\\tt.gif"), std::ios::out|std::ios::binary);
// 			if(of.good()) {
// 				char buffer[1024] = {0};
// 				int64 filesize = p->filesize();
// 				int64 startpos = 0;
// 				int64 fcount = filesize;
// 				while(fcount > 0) {
// 					int64 ReadBytes = zzip.ReadData(p, startpos, (void*)buffer, 1024);
// 					if(ReadBytes > 0) {
// 						fcount -= ReadBytes;
// 						startpos += ReadBytes;
// 						of.write(buffer, ReadBytes);
// 					} else {
// 						break;
// 					}
// 				}
// 				of.close();
// 			}
// 		}
		
		zzip2.Close();
	}
	return 0;
}