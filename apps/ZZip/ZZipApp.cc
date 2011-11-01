
#include "../../base/application.h"
#include <iostream>
#include "ZZipApp.h"

// public
ZZipApp::ZZipApp() {

}

// public
ZZipApp::~ZZipApp() {}

// public
bool ZZipApp::run() {
	bool bRet = true; // base::framework::IApplication::run();
	// TODO:
	// do something
	std::cout << "ZZipApp is running" << std::endl;
	return bRet;
}

// public
bool ZZipApp::stop() {
	bool bRet = true; // base::framework::IApplication::stop();
	// TODO:
	// do something
	std::cout << "ZZipApp is stoped" << std::endl;
	return bRet;
}
