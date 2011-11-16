
#ifndef __ZZIPLIB_H__
#define __ZZIPLIB_H__

#pragma once

#include <typedefs.h>

#define ZZIP_SIG "ZZip"
#define ZZIP_SIG_LEN  4

// �ļ�ͷ����
typedef struct  {
	char	sig[ZZIP_SIG_LEN];	// ͷ����ʶ
	uint32	version;			// �ļ��汾
	uint64	options;			// �ļ�ѡ���ʶ
	uint64	offset;			// �ļ�Ŀ¼�ṹƫ����������λ��Ϊ�ļ�ĩβ
} ZZipFileHeader;

typedef struct {
	uint64 md5code;			// md5У����
	uint64 offset;			// �ļ�ƫ����
	uint64 filesize;		// �ļ���С
	uint32 options;			// ѡ��
	uint32 createtime;		// ����ʱ��
	uint32 lastmodify;		// �޸�ʱ��
	uint32 namelength;		// ���Ƴ���
	// char data[1];		// ·��������/��Կ�ȵ�
} ZZipFileItem;

#endif
