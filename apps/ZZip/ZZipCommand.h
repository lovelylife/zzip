
#ifndef __COMMAND_H__
#define __COMMAND_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
// �����н�����
//////////////////////////////////////////////////////////////////////////

#include <../base/typedefs.h>

class ZZipCommand {

	typedef enum {
		zz,			// ѹ��
		uz,			// ��ѹ
		app,		// ����ļ�
		rmv,		// ɾ���ļ�
		help,		// ����
		
	} ACTION;

public:
	ZZipCommand(int argc, _TCHAR* argv[]);
	~ZZipCommand(void);

public:
	ACTION action;
	tstring file; 

};


#endif
