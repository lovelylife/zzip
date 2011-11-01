
#ifndef __COMMAND_H__
#define __COMMAND_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
// �����н�����
//////////////////////////////////////////////////////////////////////////

#include <typedefs.h>
#include "ZZip.h"

class ZZipCommand {

	typedef enum {
		zz,			// ѹ��
		uz,			// ��ѹ
		add,		// ����ļ�
		rmv,		// ɾ���ļ�
		help,		// ����
		
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
