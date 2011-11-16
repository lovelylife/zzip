
#ifndef __ZZIPLIB_H__
#define __ZZIPLIB_H__

#pragma once

#include <typedefs.h>

#define ZZIP_SIG "ZZip"
#define ZZIP_SIG_LEN  4

// 文件头部分
typedef struct  {
	char	sig[ZZIP_SIG_LEN];	// 头部标识
	uint32	version;			// 文件版本
	uint64	options;			// 文件选项标识
	uint64	offset;			// 文件目录结构偏移量，结束位置为文件末尾
} ZZipFileHeader;

typedef struct {
	uint64 md5code;			// md5校验码
	uint64 offset;			// 文件偏移量
	uint64 filesize;		// 文件大小
	uint32 options;			// 选项
	uint32 createtime;		// 创建时间
	uint32 lastmodify;		// 修改时间
	uint32 namelength;		// 名称长度
	// char data[1];		// 路径和名称/密钥等等
} ZZipFileItem;

#endif
