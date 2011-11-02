// study.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include "base/application.h"
#include <fstream>

using namespace std;

#define UsingApp_ZZip

#ifdef UsingApp_ZZip
	#include "apps/ZZip/ZZipApp.h"
	typedef ZZipApp CAppClass;
#endif

CAppClass theApp;

int _tmain(int argc, TCHAR* argv[])
{
	theApp.run(argc, argv);
	theApp.stop();

#ifdef USING_DEBUG
	system("pause");
#endif
	return 0;
}
