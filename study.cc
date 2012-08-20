// study.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "base/application.h"
#include <fstream>
#include "thread.h"

using namespace std;

#define UsingApp_ZZip

#ifdef UsingApp_ZZip
	#include "apps/ZZip/ZZipApp.h"

	typedef ZZipApp CAppClass;
#endif

CAppClass theApp;

class WorkThread 
	: public Thread 
{
	
};

int _tmain(int argc, TCHAR* argv[])
{
	theApp.run(argc, argv);
	theApp.stop();



#ifdef USING_DEBUG
	system("pause");
#endif
	return 0;
}
