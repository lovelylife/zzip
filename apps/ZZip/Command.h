
#ifndef __COMMAND_H__
#define __COMMAND_H__

#pragma once

//////////////////////////////////////////////////////////////////////////
// �����н�����
//////////////////////////////////////////////////////////////////////////

#include <../base/typedefs.h>

class ZZipCommand {

	typedef enum {
		z,
		uz,
		
	} Action;

public:
	ZZipCommand(int argc, _TCHAR* argv[]);
	~ZZipCommand(void);

public:
	int action;
};


#endif
