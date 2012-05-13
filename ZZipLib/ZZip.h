
#ifndef __ZZIPLIB_H__
#define __ZZIPLIB_H__

#pragma once

#include "TypeDefs.h"

#define ZZIP_SIG "ZZip"
#define ZZIP_SIG_LEN  4

// �ļ�ͷ����
typedef struct  {
	char	sig[ZZIP_SIG_LEN];	// ͷ����ʶ
	uint32	version;			// �ļ��汾
	int64	options;			// �ļ�ѡ���ʶ
	int64	offset;				// �ļ�Ŀ¼�ṹƫ����������λ��Ϊ�ļ�ĩβ
	int32	extendsize;			// ��չ���ֳ���(��չ��������λ��offset-extendsize)
} ZZipFileHeader;

typedef struct {
	int64 md5code;			// md5У����
	int64 offset;			// �ļ�ƫ����
	int64 filesize;			// �ļ���С
	int32 options;			// ѡ��
	int32 createtime;		// ����ʱ��
	int32 lastmodify;		// �޸�ʱ��
	int32 namelength;		// ���Ƴ���
	// char data[1];		// ·��������/��Կ�ȵ�
} ZZipFileItem;

#endif
