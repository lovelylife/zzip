/************************************************** 
���� ����CBrowseDir���ඨ���ļ� BrowseDir.h 

/**************************************************/

#include "typedefs.h"

class DirHelper {
public: 
	//ȱʡ������ 
	DirHelper(const tstring& sPath, bool bSubFolder = false); 
	//���ó�ʼĿ¼Ϊdir���������false����ʾĿ¼������ 
	bool SetInitDir(const tstring& sInitDir); 

	//��ʼ������ʼĿ¼������Ŀ¼����filespecָ�����͵��ļ� 
	//filespec����ʹ��ͨ��� * ?�����ܰ���·���� 
	//�������false����ʾ�������̱��û���ֹ 
	bool BeginBrowse(const tstring& sFilespec); 

protected: 
	//��ų�ʼĿ¼�ľ���·������'\'��β
	tstring sFolderPath_; 
	bool bSubFolder_;
protected: 
	//����Ŀ¼dir����filespecָ�����ļ� 
	//������Ŀ¼,���õ����ķ��� 
	//�������false,��ʾ��ֹ�����ļ� 
	bool BrowseDir(const tstring& sDir, const tstring& sFilespec); 

	//����BrowseDirÿ�ҵ�һ���ļ�,�͵���ProcessFile 
	//�����ļ�����Ϊ�������ݹ�ȥ 
	//�������false,��ʾ��ֹ�����ļ� 
	//�û����Ը�д�ú���,�����Լ��Ĵ������ 
	virtual bool ProcessFile(const tstring& sFileName); 

	//����BrowseDirÿ����һ��Ŀ¼,�͵���ProcessDir 
	//�������ڴ����Ŀ¼������һ��Ŀ¼����Ϊ�������ݹ�ȥ 
	//������ڴ�����ǳ�ʼĿ¼,��parentdir=NULL 
	//�û����Ը�д�ú���,�����Լ��Ĵ������ 
	//�����û�����������ͳ����Ŀ¼�ĸ��� 
	virtual void ProcessDir(const tstring& sCurrentDir, const tstring& sParentDir); 
}; 