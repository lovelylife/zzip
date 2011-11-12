
#include "../../base/application.h"
#include <iostream>
#include "ZZipApp.h"
#include "ZZipFile.h"

// public
ZZipApp::ZZipApp() {

}

// public
ZZipApp::~ZZipApp() {}

// public
bool ZZipApp::run(int argc, TCHAR* argv[]) {
	bool bRet = true; // base::framework::IApplication::run();
	// TODO:
	// do something
	std::cout << "ZZipApp is running" << std::endl;
	wchar_t WCharString[] = {0xff,0x22,0xad,0x33,0x00};
	tstring sString = _T("abcdefghijklmn");
	std::cout << wcslen(WCharString) << std::endl;
	std::cout << sString.size() << std::endl;

	ZZipFile zzip;
	if(argc <= 1) { return 0; }
	if(zzip.Open(argv[1])) {
//		zzip.AddFolder(_T("/"), _T("E:\\test"));
		zzip.AddFile(_T("/1.txt"), _T("C:\\1.txt"));
		zzip.AddFile(_T("/2.txt"), _T("C:\\2.txt"));
		zzip.AddFile(_T("/3.txt"), _T("C:\\3.txt"));
		zzip.AddFile(_T("/4.gif"), _T("C:\\4.gif"));
		zzip.Save();
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
