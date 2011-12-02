// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#ifdef _STATIC_LINK
#define _EXT_CLASS
#else
#define _EXT_CLASS _declspec(dllimport)
#endif


#include "ZZipFile.h"

// TODO: 在此处引用程序需要的其他头文件
