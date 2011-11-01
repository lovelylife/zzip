
#ifndef __COMMAND_H__
#define __COMMAND_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
// 命令行解析器
//////////////////////////////////////////////////////////////////////////

#include <../base/typedefs.h>

class ZZipCommand {

	typedef enum {
		zz,			// 压缩
		uz,			// 解压
		app,		// 添加文件
		rmv,		// 删除文件
		help,		// 帮助
		
	} ACTION;

public:
	ZZipCommand(int argc, _TCHAR* argv[]);
	~ZZipCommand(void);

public:
	ACTION action;
	tstring file; 

};


#endif
