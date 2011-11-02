/************************************************** 
　　 这是CBrowseDir的类定义文件 BrowseDir.h 

/**************************************************/

#include "typedefs.h"

class DirHelper {
public: 
	//缺省构造器 
	DirHelper(const tstring& sPath, bool bSubFolder = false); 
	//设置初始目录为dir，如果返回false，表示目录不可用 
	bool SetInitDir(const tstring& sInitDir); 

	//开始遍历初始目录及其子目录下由filespec指定类型的文件 
	//filespec可以使用通配符 * ?，不能包含路径。 
	//如果返回false，表示遍历过程被用户中止 
	bool BeginBrowse(const tstring& sFilespec); 

protected: 
	//存放初始目录的绝对路径，以'\'结尾
	tstring sFolderPath_; 
	bool bSubFolder_;
protected: 
	//遍历目录dir下由filespec指定的文件 
	//对于子目录,采用迭代的方法 
	//如果返回false,表示中止遍历文件 
	bool BrowseDir(const tstring& sDir, const tstring& sFilespec); 

	//函数BrowseDir每找到一个文件,就调用ProcessFile 
	//并把文件名作为参数传递过去 
	//如果返回false,表示中止遍历文件 
	//用户可以覆写该函数,加入自己的处理代码 
	virtual bool ProcessFile(const tstring& sFileName); 

	//函数BrowseDir每进入一个目录,就调用ProcessDir 
	//并把正在处理的目录名及上一级目录名作为参数传递过去 
	//如果正在处理的是初始目录,则parentdir=NULL 
	//用户可以覆写该函数,加入自己的处理代码 
	//比如用户可以在这里统计子目录的个数 
	virtual void ProcessDir(const tstring& sCurrentDir, const tstring& sParentDir); 
}; 