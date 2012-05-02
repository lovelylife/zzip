
#ifndef __ZZIPLIB_H__
#define __ZZIPLIB_H__

#pragma once

#include "typedefs.h"

#define ZZIP_SIG "ZZip"
#define ZZIP_SIG_LEN  4

// 文件头部分
typedef struct  {
	char	sig[ZZIP_SIG_LEN];	// 头部标识
	uint32	version;			// 文件版本
	int64	options;			// 文件选项标识
	int64	offset;				// 文件目录结构偏移量，结束位置为文件末尾
	int32	extendsize;			// 扩展部分长度(扩展部分数据位置offset-extendsize)
} ZZipFileHeader;

typedef struct {
	int64 md5code;			// md5校验码
	int64 offset;			// 文件偏移量
	int64 filesize;			// 文件大小
	int32 options;			// 选项
	int32 createtime;		// 创建时间
	int32 lastmodify;		// 修改时间
	int32 namelength;		// 名称长度
	// char data[1];		// 路径和名称/密钥等等
} ZZipFileItem;

#endif
