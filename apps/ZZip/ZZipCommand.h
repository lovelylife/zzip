
#ifndef __COMMAND_H__
#define __COMMAND_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
// 命令行解析器
//////////////////////////////////////////////////////////////////////////

#include <typedefs.h>
#include "ZZip.h"

class ZZipCommand {

	typedef enum {
		zz,			// 压缩
		uz,			// 解压
		add,		// 添加文件
		rmv,		// 删除文件
		help,		// 帮助
		
	} ACTION;

public:
	ZZipCommand(int argc, TCHAR* argv[]);
	~ZZipCommand(void);

// methods
public:
	void PrintHelpInfo();
public:
	ACTION action;
	tstring file; 
};


#endif
