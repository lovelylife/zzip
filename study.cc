// study.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "base/ref_counted.h"
#include "base/application.h"
#include <fstream>

using namespace std;

#define UsingApp_ZZip

#ifdef UsingApp_ZZip
	#include "apps/ZZip/ZZipApp.h"
	typedef ZZipApp CAppClass;
#endif

CAppClass theApp;

int main(int argc, _TCHAR* argv[])
{
	theApp.run();
	theApp.stop();

#ifdef USING_DEBUG
	system("pause");
#endif
	return 0;
}
