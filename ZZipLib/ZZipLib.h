
#ifndef __ZZIPLIB_H__
#define __ZZIPLIB_H__

#pragma once

#include <../base/typedefs.h>

// �ļ�ͷ����
typedef struct  {
	uint8 sig[4];			// ͷ����ʶ
	uint32 version;			// �ļ��汾
	uint32 options;			// �ļ�ѡ���ʶ
	uint64 offset;			// �ļ�Ŀ¼�ṹƫ����������λ��Ϊ�ļ�ĩβ
	uint32 extsize;			// ��չ��С
	uint32 extdata[1];		// ��չ����
} ZZipHeader;

typedef struct {
	uint64 size;			// �ṹ���С
	uint64 md5code;			// md5У����
	uint64 offset;			// �ļ�ƫ����
	uint64 filesize;		// �ļ���С
	uint32 createtime;		// ����ʱ��
	uint32 lastmodify;		// �޸�ʱ��
	uint32 namelen;			// ���Ƴ���
	// char data[1];			// ·��������/��Կ�ȵ�
} ZZipFileItem;

#endif
