
#include "ZZipCommand.h"


static const _TCHAR* CommandHelpInfo[] = {
	_T("aaaa"),

};

ZZipCommand::~ZZipCommand(void){}

ZZipCommand::ZZipCommand( int argc, _TCHAR* argv[] ) {
	if(argc < 2) {
		// printf help info
		
	} else {
		// ½âÎö²Ù×÷
		tstring sAction = argv[1];
		if(sAction.compare(_T("zz")) == 0) {
			action = zz;
		} else if(sAction.compare(_T("uz")) == 0) {
			action = uz;
		} else if(sAction.compare(_T("add")) == 0) {
			action = add;
		} else if(sAction.compare(_T("rmv")) == 0) {
			action = rmv;
		} else if(sAction.compare(_T("help")) == 0) {
			action = help;
		} else {
			// printf help info
		}
	}
}
