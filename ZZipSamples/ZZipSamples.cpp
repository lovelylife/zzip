// ZZipSamples.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
//#include "Color.h"
#include <atlconv.h>
#include "QHttp.h"
#include <assert.h>


// 枚举文件
class EnumItems {
public:
	EnumItems(ZZipFile& zzip, const tstring& sPath, bool bRescuire = false) {
		ZZipFile::ZZipFileTree::ValueTravEvent evt;
		evt += std::make_pair(this, &EnumItems::EnumFile);
		zzip.EnumItem(sPath, evt, bRescuire);
		evt -= std::make_pair(this, &EnumItems::EnumFile);
	}

public:
	bool EnumFile(const ZZipFile::ZZipFileTree::PathType& path, const ZZipFile::ZZipFileTree::ValueType&) {
		tstring sPath;
		ZZipFile::ZZipFileTree::Path2String(path, sPath);
		_tprintf(_T("EnumFile() file: %s\n"), sPath.c_str());
		return true;
	}
};

struct tests {
	bool a1 : 1;
	bool a2 : 1;
	bool a3 : 1;
	bool a4 : 1;
	bool a5 : 1;
	bool a6 : 1;
	bool a7 : 1;
	bool a8 : 1;
};

class DownloadController : public q::IDownloadController {
public:
	void OnAttach(q::IDownloadObject* p) {
		printf("DownloadController attached.\r\n");
		object_ = p;
	}

	void OnDettach() {
		printf("DownloadController deattached.\r\n");
	}

	void OnStatusChanged(int) {
		printf("DownloadController OnStatusChanged.\r\n");
	}

	size_t OnRecvData(void* ptr, size_t size) {
		return size;
	}

	bool IsCompleted() {
		assert(object_);
		return (object_->downloaded_size() == object_->file_size() && object_->downloaded_size() > 0);
	}

	uint64 get_download() {
		assert(object_);
		return object_->downloaded_size();
	}

	uint64 get_total_size() {
		assert(object_);
		return object_->file_size();
	}

	void Dump() {
		std::ostringstream os;
		os << "url:\t\t" << object_->url() << std::endl;
		os << "actual url:\t" << object_->actual_url() << std::endl;
		os << "size:\t\t" << object_->file_size() << " bytes" << std::endl;
		printf(os.str().c_str());
	}

private:
	RefPtr<q::IDownloadObject> object_;
};

class HttpClientRequest : public q::IRequestController {
public:
	void OnFinish(q::IRequestObject* p) {
		if(p->status() != 200) {
			std::cout << "error status code: " << p->status() << std::endl;
			return;
		}
		const char* sContent = p->content();
	}
};



class Interface1 {
public:
	virtual void fun() = 0;
	virtual ~Interface1() {}
};

class X : public q::Object , public Interface1{
public:
	X(){ std::cout << "X::X();" << std::endl; }
	~X() { std::cout << "X::~X();" << std::endl; }

	void fun() {
		std::cout << "X::fun();" << std::endl;
	}
};

class Z;

class Y : public q::Object{
public:
	Y(){ std::cout << "Y::Y();" << std::endl; }
	~Y();

	


	Interface1* x;
};

class Z : public q::Object {
public:
	Z(){
		std::cout << "Z::Z();" << std::endl;
		x = new X;
		y = new Y;
		
		y->x = x;
		 }
	~Z() { std::cout << "Z::~Z();" << std::endl; }

	void call() {
		a = 0;
		std::cout << "Z::call();" << std::endl;
	}


int a;
RefPtr<Y> y;
RefPtr<X> x;

};

Y::~Y() { std::cout << "Y::~Y();" << std::endl;
x->fun();
}

#define tostr(T) printf("class token"#T"\n")

template <class T>
class CC {
public:
	CC() {}
	~CC(){}

	void p() {
		tostr(T);
	}

};

#include <vector>


long CalculateResolutionHeight(const std::vector<POINT>& v) {

	int a = 119;
	std::cout << a/40 << ": " <<a%40 << std::endl;


	return 0;
	bool is_equal = false;
	long result_height = 480;
	long height_less(0), height_larger(0xff);
	for(std::vector<POINT>::const_iterator it=v.begin();
		it != v.end(); it++) 
	{
		POINT pt = *it;
		if(pt.y == 480) {
			is_equal = true;
			break;
		} else if(pt.y < 480) {
			if(pt.y > height_less) {
				height_less = pt.y;
			}
		} else {
			if(pt.y < height_larger) {
				height_larger = pt.y;
			}
		}
	}

	if(!is_equal) {
		if(height_less >= 480) {
			result_height = height_larger;
		} else {
			result_height = height_less;
		}
	}

	return result_height;
}

int _tmain(int argc, _TCHAR* argv[]) {
	
	/*
	long x(0), y(0);
	// 计算合适的分辨率 640*480
	std::vector<POINT> resolutions;
	POINT pt = {640, 480};
	//resolutions.push_back(pt);

	pt.x = 300; pt.y = 380;
	resolutions.push_back(pt);

	pt.x = 1100; pt.y = 200;
	resolutions.push_back(pt);

<<<<<<< .mine
	pt.x = 1280; pt.y = 1024;
	resolutions.push_back(pt);

	std::vector<POINT> vector_width_less;
	std::vector<POINT> vector_width_equal;
	std::vector<POINT> vector_width_larger;
	long width_larger(0xffff), width_less(0);
	for(std::vector<POINT>::const_iterator it=resolutions.begin();
		it != resolutions.end(); it++) 
=======
	RefPtr<HttpClientRequest> request_controller = new HttpClientRequest();
	// 
	if(0 != downloaders->request(
		"http://localhost/phpkind/?app=cdmusic&mod=service&inajax=true&action=top100", 
		request_controller
		)) 
>>>>>>> .r103
	{
		POINT pt = *it;
		if(pt.x == 640) {
			vector_width_equal.push_back(pt);
		} else if( pt.x < 640) {
			if(pt.x>width_less) {
				width_less = pt.x;
				vector_width_less.clear();
				vector_width_less.push_back(pt);
			} else if( pt.x == width_less) {
				vector_width_less.push_back(pt);
			}
		} else if( pt.x > 640) {
			if(pt.x < width_larger) {
				width_larger = pt.x;
				vector_width_larger.clear();
				vector_width_larger.push_back(pt);
			} else if(pt.x == width_larger) {
				vector_width_larger.push_back(pt);
			}
		}
	}

	if(vector_width_equal.size() > 0) {
		x = 640;
		y = CalculateResolutionHeight(vector_width_equal);
	} else if(vector_width_less.size() >0) {
		x = width_less;
		y = CalculateResolutionHeight(vector_width_less);
	} else if(vector_width_larger.size()) {
		x = width_larger;
		y = CalculateResolutionHeight(vector_width_larger);
	} else {
		// not get here
		assert(0);
	}

	std::cout << "width:" << x << " height:" << y << std::endl;
	
	*/
	//RefPtr<Z> z = new Z;
//	CC<int> c;
//	c.p();
	q::Http* downloaders = q::create(3);

 	RefPtr<DownloadController> controller = new DownloadController;
	controller->open("http://sq2.newhua.com/down/LeapFTP3.0.1.46_yfy.zips", "get");
 	if(0 != downloaders->send(controller)) 
 	{
 		printf("error create download task.");
 	}

//	RefPtr<RequestTop100> request_controller = new RequestTop100();
//	// 
//	if(0 != downloaders->request(
//		"http://192.168.10.106/trunk/?app=cdmusic&mod=service&inajax=true&action=top100", 
//		request_controller
//		)) 
//	{
//		printf("error create download task.");
//	}
//
//
	std::streambuf* old = std::cout.rdbuf();

	while(true) {
		Sleep(1000);
 		system("cls");
 		std::cout << "downloaded: " << controller->get_download() << " bytes";
 		
 		if(controller->IsCompleted()) {
 			printf("Download Completed.\r\n");
 			break;
 		}
	}

	controller->Dump();

	return 0;
// #ifdef USING_TestCode
// 	QUI::Color::doTest();
// #endif
//	std::cout << "value is A = " << (int)((unsigned char)(value >> 24)) << std::endl;
// 	unsigned long value = 0x01020304;
// 	std::cout << "value is R" << (int)((unsigned char)value) << std::endl;
// 	std::cout << "value is G" << (int)((unsigned char)(value >> 8)) << std::endl;
// 	std::cout << "value is B" << (int)((unsigned char)(value >> 16)) << std::endl;
// 	std::cout << "value is A" << (int)((unsigned char)(value >> 24)) << std::endl;
	
//	return 0;

// 	tstring sPath = _T("a/b/c/d/e.htm");
// 	ZZipFile::ZZipFileTree::PathType path;
// 	ZZipFile::ZZipFileTree::String2Path(sPath, path);
// 	tstring sPathOutput;
// 	ZZipFile::ZZipFileTree::Path2String(path,sPathOutput);
	bool bRet = true;
	// TODO:
	// do something
	std::cout << "ZZipApp is running" << std::endl;
	wchar_t WCharString[] = {0xff,0x22,0xad,0x33,0x00};
	tstring sString = _T("abcdefghijklmn");
	sString.append(1, _T('\0'));
	sString.append(1, _T('\0'));
	sString.append(1, _T('\0'));
	std::cout << wcslen(sString.c_str()) << std::endl;

	std::cout << sString.size() << std::endl;
	std::cout << sizeof(ZZipFileItem) << std::endl;
	ZZipFile zzip;
	// if(argc <= 1) { return 0; }
	tstring sZZipFileName = _T("D:/Developing/VCCode/trunk/QUI/QUITest/res/skin.zzip");
	// if(zzip.Open(argv[1])) {
	if(zzip.Open(sZZipFileName)) {
		zzip.AddFolder(_T("/"), _T("D:/Developing/VCCode/trunk/QUI/QUITest/res/skin"));
// 		zzip.AddFile(_T("/1.txt"), _T("C:\\1.txt"));
// 		zzip.AddFile(_T("/2.txt"), _T("C:\\2.txt"));
// 		zzip.AddFile(_T("/3.txt"), _T("C:\\3.txt"));
// 		zzip.AddFile(_T("/4.gif"), _T("C:\\4.gif"));
		zzip.Save();
		zzip.Close();
	}

	ZZipFile zzip2;
	if(zzip2.Open(sZZipFileName)) {

		// 枚举文件
		EnumItems e(zzip2, _T("/"), true);
// 		ZZipFile::ZZipFileTree::ValueTravEvent evt;
// 		evt += std::make_pair(&e, &EnumItems::EnumFile);
// 		zzip.EnumItem(_T("/"), evt, false);
// 		evt -= std::make_pair(&e, &EnumItems::EnumFile);

//		ZZipEnum<EnumItems>(zzip2, _T("/drawables"), false);
// 
// 		// 读取文件"/4.gif" 保存到 "C:\\tt.gif"
// 		RefPtr<ZZipFileObject> p = zzip.FindFile(_T("/4.gif"));
// 		if(NULL != p) {
// 			std::ofstream of(_T("C:\\tt.gif"), std::ios::out|std::ios::binary);
// 			if(of.good()) {
// 				char buffer[1024] = {0};
// 				int64 filesize = p->filesize();
// 				int64 startpos = 0;
// 				int64 fcount = filesize;
// 				while(fcount > 0) {
// 					int64 ReadBytes = zzip.ReadData(p, startpos, (void*)buffer, 1024);
// 					if(ReadBytes > 0) {
// 						fcount -= ReadBytes;
// 						startpos += ReadBytes;
// 						of.write(buffer, ReadBytes);
// 					} else {
// 						break;
// 					}
// 				}
// 				of.close();
// 			}
// 		}
		
		zzip2.Close();
	}
	return 0;
}