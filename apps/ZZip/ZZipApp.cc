
#include "stdafx.h"
#include "application.h"
#include <iostream>
#include "ZZipFile.h"
#include "ZZipApp.h"
#include <vector>
#include <algorithm>
#include <string>

// public
ZZipApp::ZZipApp() {

}

// public
ZZipApp::~ZZipApp() {}

// public
bool ZZipApp::run(int argc, TCHAR* argv[]) {

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
		// 读取文件"/4.gif" 保存到 "C:\\tt.gif"
		std::ofstream of(_T("C:\\tt.xml"), std::ios::out|std::ios::binary);
		if(of.good()) {
			refptr<ZZipFileObject> FileObjectPtr = zzip.FindFile(_T("/3.txt"));
			if(FileObjectPtr) {
				char buffer[1024] = {0};
				uint64 filesize = FileObjectPtr->filesize();
				uint64 startpos = 0;
				uint64 fcount = filesize;
				while(fcount > 0) {
					std::streamsize ReadBytes = zzip.ReadData(FileObjectPtr, startpos, (void*)buffer, 1024);
					if(ReadBytes > 0) {
						fcount -= ReadBytes;
						startpos += ReadBytes;
						of.write(buffer, ReadBytes);
					} else {
						break;
					}
				}
				of.close();
			}
			
		}
		zzip.Close();
	}
	
	return bRet;
}

// public
bool ZZipApp::stop() {
	bool bRet = true; // base::framework::IApplication::stop();
	// TODO:
	// do something
	std::cout << "ZZipApp is stoped" << std::endl;
	return bRet;
}
