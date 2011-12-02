
#ifndef __ZZIPAPP_H__
#define __ZZIPAPP_H__	

#include "application.h"

struct event_base;

class ZZipApp : public base::framework::Application<ZZipApp> {
public:
	ZZipApp();
	~ZZipApp();

public:
	virtual bool run(int argc, _TCHAR* argv[]);
	virtual bool stop();

private:
	struct event_base *basePtr_;
};

#endif


