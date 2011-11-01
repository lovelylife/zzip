
#ifndef __ZZIPLIB_H__
#define __ZZIPLIB_H__

#pragma once

#include <../base/typedefs.h>

// 文件头部分
typedef struct  {
	uint8 sig[4];			// 头部标识
	uint32 version;			// 文件版本
	uint32 options;			// 文件选项标识
	uint64 offset;			// 文件目录结构偏移量，结束位置为文件末尾
	uint32 extsize;			// 扩展大小
	uint32 extdata[1];		// 扩展数据
} ZZipHeader;

typedef struct {
	uint64 size;			// 结构体大小
	uint64 md5code;			// md5校验码
	uint64 offset;			// 文件偏移量
	uint64 filesize;		// 文件大小
	uint32 createtime;		// 创建时间
	uint32 lastmodify;		// 修改时间
	uint32 namelen;			// 名称长度
	// char data[1];			// 路径和名称/密钥等等
} ZZipFileItem;

#endif
