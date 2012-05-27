
#include "stdafx.h"
#include "ZZipFile.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <io.h>
#include <strstream>

#ifdef _WIN32
#include <atlbase.h>
#include <atlconv.h>
#include <shlwapi.h>
#include <shellapi.h>
#endif

static const TCHAR ZZipTmpFileName[]	= _T(".__zzip__");
static const TCHAR ZZipTmpFolderName[]  = _T(".cache\\");

bool ZZipFile::PathIsInValid(const tstring& sPath ) {
	return (_taccess(sPath.c_str(), 0) == -1);
}

ZZipFile::ZZipFile(void) 
: StreamPtr_(NULL)
, StreamWriterPtr_(NULL)
, Type_(TypeUnknow)
, OpenMode_(OpenMode_|OpenModeWrite)
, ErrorCode_(0)
{
	memset(&ZZipFileHeader_, 0, sizeof(ZZipFileHeader));
	strncpy(ZZipFileHeader_.sig, ZZIP_SIG, ZZIP_SIG_LEN);
	// ���¼�
	WriteFileObjectEvent += std::make_pair(this, &ZZipFile::WriteFileObject);
}

ZZipFile::~ZZipFile(void) {
	WriteFileObjectEvent += std::make_pair(this, &ZZipFile::WriteFileObject);
	if(StreamPtr_ != NULL) {
		delete StreamPtr_;
		StreamPtr_ = NULL;
	}
}

#ifdef _WIN32

bool ZZipFile::Open(HINSTANCE hInstance, const tstring& sType, unsigned int pszResourceName) {	
	
	ATLASSERT(StreamPtr_ == NULL);

	// ����Դ��ȡ�����ݣ�ֻ��ģʽ����Ϊ���ܽ���д����
	OpenMode_ = OpenModeRead;

    // �����ͣ����ļ�����
	Type_ = TypeStream;

	//��λ���ǵ��Զ�����Դ��������Ϊ�����Ǵӱ�ģ�鶨λ��Դ�����Խ�����򵥵���ΪNULL����
	HRSRC hRsrc = ::FindResource(hInstance, MAKEINTRESOURCE(pszResourceName), sType.c_str());
	if (NULL == hRsrc) return false;

	//��ȡ��Դ�Ĵ�С
	DWORD dwSize = SizeofResource(hInstance, hRsrc); 
	if (0 == dwSize) {
		dwSize = ::GetLastError();
		return false;
	}
	//������Դ
	HGLOBAL hGlobal = LoadResource(hInstance, hRsrc); 
	if (NULL == hGlobal)
		return false;
	//������Դ
	char* pBuffer = (char*)LockResource(hGlobal); 
	if (NULL == pBuffer) return false;

 	StreamPtr_ = new std::strstream();
 	StreamPtr_->write(pBuffer,dwSize);
	::FreeResource(hRsrc);
	return Parse(StreamPtr_);
}

#endif

bool ZZipFile::Open( const tstring& sFileName, int OpenMode)  {

	ATLASSERT(StreamPtr_ == NULL);
	if(sFileName.empty() ) return false;

	// ����ZZip�ļ���
	sZZipFileName_ = sFileName;
	
	// ��ģʽ
	OpenMode_ = OpenMode;
	std::fstream::openmode StreamOpenMode = std::fstream::binary;
	if(OpenMode_ & OpenModeRead) {
		StreamOpenMode = StreamOpenMode | std::fstream::in;
	}

	if(OpenMode_ & OpenModeWrite) {
		StreamOpenMode = StreamOpenMode | std::fstream::out;
	}

	StreamPtr_ = new std::fstream;
	std::fstream* ZZipFileStreamPtr = (std::fstream*)StreamPtr_;
	
	ZZipFileStreamPtr->open(sFileName.c_str(), StreamOpenMode);
	if(StreamPtr_->fail()) {
		std::cout << "[ZZip]Open file " << CT2A(sFileName.c_str()) << " error." << std::endl;
		return false;
	}

	// ����ļ���д�����ڵ�ǰ�ļ�����Ŀ¼������ʱ�ļ�
	if((OpenMode_ & OpenModeWrite)) {
		
		tstring sTempFileName = sZZipFileName_;
		sTempFileName += _T(".__zzip__");

		if(StreamWriterPtr_ == NULL) {
			StreamWriterPtr_ = new std::ofstream;
			StreamWriterPtr_->open(sTempFileName.c_str(), std::ostream::out|std::ostream::binary|std::ostream::trunc);
		}
		
		if(!StreamWriterPtr_->good()) { 
			return false;
		}

		StreamWriterPtr_->clear();
		StreamWriterPtr_->flush();
	}

	// ���浽��ʱ�ļ�
 	sCacheDir_ = sZZipFileName_;
	sCacheDir_ += ZZipTmpFolderName;
	std::replace(sCacheDir_.begin(), sCacheDir_.end(), _T('/'), _T('\\'));

	// �ж��ļ����Ƿ񲻴���
	if(PathIsInValid(sCacheDir_)) {
		if(0 != _tmkdir(sCacheDir_.c_str())){
			// �����ļ���ʧ��
			return NULL;
		}
	}

	std::cout << "[ZZip]Open file " << CT2A(sFileName.c_str()) << " success." << std::endl;

	OpenMode_ = OpenMode;
	Type_     = TypeFile;

	return Parse(StreamPtr_);
}

bool ZZipFile::Parse( std::iostream* pStream )
{
	if(pStream == NULL || (!pStream->good())) return false;
	pStream->seekg(0, std::ios::end);
	int64 filesize = pStream->tellg();

	// �ļ���Ϊ����ͷ����СС��sizeof(ZZipFileHeader���Ǹ���Ч���ļ�
	if(filesize <= 0) {
		// ���ļ�������Ҫ����
		return true;
	}

	ZZipFileHeader hdr = {0};
	if(filesize < sizeof(ZZipFileHeader)) {
		std::cout << "Invalid file data!" << std::endl;
		return false;
	}		

	// ��ȡ�ļ�ͷ����
	pStream->seekg(0, std::ios::beg);
	pStream->read((char*)&hdr, sizeof(ZZipFileHeader));

	// ��ȡĿ¼�ṹ
	pStream->seekg(hdr.offset, std::ios::beg);
	{
		char buffer[1024] = {0};
		std::streamsize size = 0;
		std::streamsize restsize = filesize - hdr.offset;
		while((!pStream->eof()) && restsize > 0) {
			RefPtr<ZZipFileObject> FileObjectPtr = new ZZipFileObject;
			pStream->read((char*)&FileObjectPtr->FileItem_, std::streamsize(sizeof(ZZipFileItem)));
			restsize -= sizeof(ZZipFileItem);
			// ��ȡ·��
			std::string sTemp;
			size = FileObjectPtr->FileItem_.namelength;

			while((size > 0) && (!StreamPtr_->eof())) {
				if(size > sizeof(buffer)) {
					pStream->read(buffer, sizeof(buffer));
				} else {
					pStream->read(buffer, size);
				}
				if(!pStream->gcount()) break;
				sTemp.append(buffer, pStream->gcount());
				size -= pStream->gcount();
				restsize -= pStream->gcount(); 
			}

			if(!sTemp.empty()) {
				tstring sPath = CA2T(sTemp.c_str());;
				FileObjectPtr->setpath(sPath);
				//@todo
				ZZipFileTree::PathType pathtype;
				ZZipFileTree::String2Path(sPath, pathtype);
				if(FileObjectPtr->isfolder()) {
					FileObjectsTree_.kdir(pathtype);
				} else {
					FileObjectsTree_.insert(pathtype, FileObjectPtr);
				}
			}

			if(pStream->eof() || (restsize <= 0)) {
				break;
			}
		}
	}

	return true;
}


// �����ļ�(���л�)
bool ZZipFile::Save() {

	// ���ļ����Ͳ��ܽ����ļ��������
	ATLASSERT(
		(StreamPtr_ != NULL)
		&& (Type_ == TypeFile) 
		&& (OpenMode_ & OpenModeWrite)
	);

	// ��Ч�Ļ����ļ�
	if(StreamWriterPtr_ == NULL) return false;
	
	// ���ƶ��ļ�ͷ��Сλ��
	StreamWriterPtr_->seekp(sizeof(ZZipFileHeader), std::ios::beg);

	// ���Archive����
	ArchiveWriter_.flush();

	// ����Ŀ¼�������л�
	FileObjectsTree_.DoTrav(ZZipFileTree::PathType(),  WriteFileObjectEvent, true);

 	// �����ļ�Ŀ¼�ṹλ��
	ZZipFileHeader_.offset  = StreamWriterPtr_->tellp();

	// ������д��ZZip�ļ�
	{
		std::string sArchive = ArchiveWriter_.str();
		StreamWriterPtr_->write(sArchive.c_str(), sArchive.size());
		ArchiveWriter_.flush();
	}
	
	// д�ļ�ͷ
	StreamWriterPtr_->seekp(0, std::ios::beg);
	StreamWriterPtr_->write((char*)&ZZipFileHeader_, sizeof(ZZipFileHeader));
	StreamWriterPtr_->close();

	//������Ҫ�Ļ���Ŀ¼���ļ���
	//������ڻ����ļ�����Ӧ��ɾ����
	if(!PathIsInValid(sCacheDir_)) {
		RemoveDir(sCacheDir_);
	}

	// ɾ���޸�ǰ��ZZip�ļ�������ʱ�����ļ�����
	((std::fstream*)StreamPtr_)->close();
	::_tremove(sZZipFileName_.c_str());
	StreamPtr_ = NULL;
	// remove(CT2A(sZZipFileName_.c_str()));

	tstring sTempFileName = sZZipFileName_;
	sTempFileName += ZZipTmpFileName;	
	
	// ʹ�õ�ǰ����
	int ReturnCode = _trename(sTempFileName.c_str(), sZZipFileName_.c_str());

	if(ReturnCode != 0) {
		std::cout << "rename: error(code:" << ReturnCode << ")" <<  std::endl;
		return false;
	}

	return Open(sZZipFileName_, OpenMode_);
}

void ZZipFile::RemoveFile( const tstring& sZZipPath )
{
	RefPtr<ZZipFileObject> object;
	ZZipFileTree::PathType path;
	ZZipFileTree::String2Path(sZZipPath, path);
	FileObjectsTree_.erase(path, true);
}

RefPtr<ZZipFileObject> ZZipFile::AddFile( const tstring& sZZipPath, const tstring& sLocalFileName, bool bOverwrite )
{
	ZZipFileTree::PathType path;
	ZZipFileTree::String2Path(sZZipPath, path);
	RefPtr<ZZipFileObject> FileObjectPtr = new ZZipFileObject();
	FileObjectPtr->sLocalPath_ = sLocalFileName;
	FileObjectPtr->setpath(sZZipPath);
	FileObjectsTree_.insert(path, FileObjectPtr);

	return FileObjectPtr;
}


#ifdef _WIN32
bool ZZipFile::AddFile(const tstring& sZZipPath, IStream* pStream, bool bOverwrite)
{
	ATLASSERT((OpenMode_ & OpenModeWrite) && (Type_ == TypeFile));

	if(StreamWriterPtr_ == NULL) return false;

	// ����ļ��Ƿ����
 	RefPtr<ZZipFileObject> FileObjectPtr = FindFile(sZZipPath);
	if(FileObjectPtr == NULL) {
		FileObjectPtr = new ZZipFileObject;
		FileObjectPtr->setpath(sZZipPath);
		RefPtr<ZZipFileObject> p;
		ZZipFileTree::PathType path;
		ZZipFileTree::String2Path(sZZipPath, path);
		FileObjectsTree_.insert(path, FileObjectPtr);
	} else {
		if(!bOverwrite) {
			return true;
		}
	}

	// �ж��ļ����Ƿ����
	if(PathIsInValid(sCacheDir_)) {
		if(0 != _tmkdir(sCacheDir_.c_str())){
			// �����ļ���ʧ��
			return false;
		}
	}

	// ������ʱ�ļ�
	TCHAR sTmpFileName[_MAX_PATH];
	if(!GetTempFileName(sCacheDir_.c_str(), NULL, 0, sTmpFileName)) {
		// GetLastError()
		return false;
	}

	FileObjectPtr->setpath(sZZipPath);
	FileObjectPtr->sLocalPath_ = sTmpFileName;
	
	std::ofstream TempFileStream;
	TempFileStream.open(sTmpFileName, std::ofstream::binary);
	// �����ļ�ʧ��GetLastError
	if(!TempFileStream.good()) {
		return false;
	}

	//д�ļ�����
	HGLOBAL hGlobal = NULL;
	GetHGlobalFromStream(pStream, &hGlobal); // ��ȡ IStream ���ڴ���
	SIZE_T size = GlobalSize(hGlobal);
	
	char* pBuffer = (char*)GlobalLock(hGlobal); 	
	TempFileStream.write(pBuffer,static_cast<long>(size));
	GlobalUnlock(hGlobal);

	return true;
}

#endif

void ZZipFile::Close() {
	if(StreamPtr_ == NULL) return;

	// �����ļ����͵Ĺرգ���Ҫ����һЩ��ʱ�����ļ���Ŀ¼
	if(Type_ == TypeFile) {

		//������Ҫ�Ļ���Ŀ¼���ļ���
		//������ڻ����ļ�����Ӧ��ɾ����
		if(!PathIsInValid(sCacheDir_)) {
			RemoveDir(sCacheDir_);
			sCacheDir_.clear();
		}

		// ɾ���޸�ǰ��ZZip�ļ�������ʱ�����ļ�����
		if(StreamWriterPtr_ && StreamWriterPtr_->is_open()) {
			StreamWriterPtr_->close();
			delete StreamWriterPtr_;
			StreamWriterPtr_ = NULL;
		}

		tstring sTempFileName = sZZipFileName_;
		sTempFileName += ZZipTmpFileName;

		if(!PathIsInValid(sTempFileName)) {
			_tremove(sTempFileName.c_str());
		}

		if(((std::fstream*)StreamPtr_)->is_open()) {
			StreamPtr_->clear();
			((std::fstream*)StreamPtr_)->close();
		}
	}
	
	if(StreamPtr_) {
		delete StreamPtr_;
		StreamPtr_ = NULL;
	}

	FileObjectsTree_.clear();
}

bool ZZipFile::AddFolder( tstring sZZipPath, tstring sLocalFolder )
{
	// ���sZZipPath�Ƿ�("/") 
	if(sZZipPath.empty()) {
		sZZipPath.append(1, _T('/'));
	} else {
		std::replace(sZZipPath.begin(), sZZipPath.end(), _T('\\'), _T('/'));
		if(sZZipPath[sZZipPath.size()-1] != _T('/')) {
			sZZipPath.append(1, _T('/'));
		}
	}
	//����dir�е���Ŀ¼ 
	long hFile; 
	_tfinddata_t FileData = {0}; 
	if(_tchdir(sLocalFolder.c_str())!=0) return false;

	// ���("/") 
	std::replace(sLocalFolder.begin(), sLocalFolder.end(), _T('\\'), _T('/'));
 	if(sLocalFolder[sLocalFolder.size()-1] != _T('/')) {
 		sLocalFolder.append(1, _T('/'));
 	}

	if ((hFile=_tfindfirst(_T("*.*"), &FileData)) != -1) {
		do { 
			// ����ǲ���Ŀ¼ 
			// �����,�ټ���ǲ��� . �� .. 
			// �������,���е���
			if ((FileData.attrib & _A_SUBDIR)) {
				if ((_tcscmp(FileData.name, _T(".")) != 0 )	
					&& (_tcscmp(FileData.name, _T("..")) != 0)) 
				{
					tstring sSubDir=sLocalFolder;
					sSubDir += FileData.name;
					sSubDir.append(1, _T('/'));

					tstring sSubZZipPath = sZZipPath;
					sSubZZipPath += FileData.name;
					sSubZZipPath.append(1, _T('/'));

					// ���Ŀ¼
					if(!AddFolder(sSubZZipPath, sSubDir)) return false;
				}
			} else {
				tstring sLocalPath;
				sLocalPath = sLocalFolder;
				sLocalPath += FileData.name;
				tstring sPath;
				sPath = sZZipPath;
				sPath += FileData.name;
				// ����ļ�
				AddFile(sPath, sLocalPath);
			}
		} while (_tfindnext(hFile,&FileData) == 0); 
		_findclose(hFile); 
	} 
	return true;
}

RefPtr<ZZipFileObject> ZZipFile::FindFile( const tstring& sZZipPath ) {
	RefPtr<ZZipFileObject> p;
	ZZipFileTree::PathType path;
	ZZipFileTree::String2Path(sZZipPath, path);
	FileObjectsTree_.Find(path, p);
	return p;
}

int64 ZZipFile::ReadData( const ZZipFileObject* zzipfile, int64 offset, void* lpBuffer, int64 size )
{
	ATLASSERT(StreamPtr_ != NULL && (zzipfile != NULL)&&(lpBuffer != NULL) && (size > 0) );
	// ATLASSERT((zzipfile != NULL)&&(lpBuffer != NULL) && (size > 0) );
	if(StreamPtr_->good()) {
		if((zzipfile->FileItem_.offset>0) && ((offset >= 0) && (offset < zzipfile->FileItem_.filesize) )) {
			std::streamsize fileoffset = zzipfile->FileItem_.offset+offset;
			std::streamsize readsize = size;
			if((readsize + offset) > zzipfile->FileItem_.filesize) {
				readsize = static_cast<std::streamsize>(zzipfile->FileItem_.filesize - offset);
			}

			StreamPtr_->seekg(fileoffset, std::ios::beg);
			StreamPtr_->read((char*)lpBuffer, readsize);
			return StreamPtr_->gcount();
		}
	}

	return 0;
}

bool ZZipFile::Attach( std::iostream* pStream )
{
	ATLASSERT(StreamPtr_ == NULL);
	StreamPtr_ = pStream;
	return Parse(StreamPtr_);
}

void ZZipFile::Clear()
{
	FileObjectsTree_.clear();
}

bool ZZipFile::ExtractFile( const tstring& sZZipPath,IStream** pStream ) {
	// �����ļ�
	RefPtr<ZZipFileObject> p = FindFile(sZZipPath);
	if((NULL!=p) && (p->filesize() > 0)) {
		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, p->filesize());
		void* pData = GlobalLock(hGlobal);
		StreamPtr_->seekg(p->offset(), std::ios::beg);
		StreamPtr_->read((char*)pData, p->filesize());				
		GlobalUnlock(hGlobal);
		return (CreateStreamOnHGlobal(hGlobal, TRUE, pStream) == S_OK);
	}

	return false;
}

bool ZZipFile::ExtractFile( const tstring& sZZipPath, std::stringstream& sstream ) {
	// ��ȡ�ļ�sZZipPath
	RefPtr<ZZipFileObject> p = FindFile(sZZipPath);
	if(NULL != p) {
		char buffer[1024] = {0};
		int64 filesize = p->filesize();
		int64 startpos = 0;
		int64 fcount = filesize;
		while(fcount > 0) {
			int64 ReadBytes = ReadData(p, startpos, (void*)buffer, 1024);
			if(ReadBytes > 0) {
				fcount -= ReadBytes;
				startpos += ReadBytes;
				sstream.write(buffer, ReadBytes);
			} else {
				break;
			}
		}
		return true;
	}

	return false;
}

bool ZZipFile::RemoveDir( const tstring& sLocalDir )
{
	SHFILEOPSTRUCT sop = {0}; 
	sop.wFunc = FO_DELETE;
	TCHAR szDirNameBuffer[_MAX_PATH] = {0};
	ZeroMemory(szDirNameBuffer, _MAX_PATH);
	_tcsncpy_s(szDirNameBuffer, sLocalDir.c_str(), min(_tcslen(sLocalDir.c_str())-1, _MAX_PATH-2));
	sop.pFrom	= szDirNameBuffer;
	sop.pTo		= NULL; 
	sop.fFlags	= FOF_ALLOWUNDO|FOF_NOCONFIRMATION|FOF_SILENT|FOF_NOERRORUI;
	sop.fAnyOperationsAborted=FALSE;
	return !SHFileOperation(&sop);
}

bool ZZipFile::IsGood() const
{
	return StreamPtr_ && StreamPtr_->good();
}

tstring ZZipFile::FileName() const
{
	return sZZipFileName_;
}

bool ZZipFile::WriteFileObject( const ZZipFileTree::PathType& path, const ZZipFileTree::ValueType& v) {
	RefPtr<ZZipFileObject> zzipfile = v.second;
	if(NULL == zzipfile)  return false;
	if(!zzipfile->isfolder()) {
		char buffer[2048];
		std::streamsize  offset = StreamWriterPtr_->tellp();
		if(zzipfile->sLocalPath_.empty()) {
			// ��ȡ�ļ�����д�뻺��
			int64 filesize = zzipfile->filesize();
			int64 startpos = 0;
			int64 fcount = filesize;
			while(fcount > 0) {
				int64 ReadBytes = ReadData(zzipfile, startpos, (void*)buffer, 1024);
				if(ReadBytes > 0) {
					fcount -= ReadBytes;
					startpos += ReadBytes;
					StreamWriterPtr_->write(buffer, ReadBytes);
				} else {
					break;
				}
			}
			zzipfile->FileItem_.offset = offset;
		} else {
			// ��ȡ�����ļ�
			std::ifstream f(zzipfile->sLocalPath_.c_str(), std::ios::in|std::ios::binary);
			if(!f.good()) { 
				zzipfile->FileItem_.filesize = 0;
				zzipfile->FileItem_.namelength = 0;
				zzipfile->FileItem_.offset = 0;
				return true; 
			}

			// ��¼�ļ�ƫ��
			zzipfile->FileItem_.offset = offset;

			// �����ļ���С
			std::streamsize filesize = 0;
			f.seekg(std::ios::beg, std::ios::end);
			std::streamsize filecount = f.tellg();
			zzipfile->FileItem_.filesize = filecount;
			f.seekg(0,std::ios::beg);
			// ��ȡ�ļ����ݲ�д��Writer
			while((!f.eof()) && (filecount > 0)) {
				memset(buffer, 0, sizeof(buffer));
				f.read(buffer, sizeof(buffer));
				std::streamsize ReadBytes = f.gcount();
				StreamWriterPtr_->write(buffer, ReadBytes);
				filecount -= ReadBytes;
			}
			f.close();
		}	
	}
	
	//char szPath[_MAX_PATH] = {0};
	std::streamsize sizecount = 0;
	tstring sZZipPath;
	ZZipFileTree::Path2String(path, sZZipPath);
	// д��Ŀ¼�ṹ
	std::string sConvString;
	sConvString = CT2A(sZZipPath.c_str());

//	printf("write file object: %s\n", sConvString.c_str());
	zzipfile->FileItem_.namelength = strlen(sConvString.c_str());
	if(zzipfile->FileItem_.namelength > 1) { 
		sizecount += zzipfile->FileItem_.namelength;
		ArchiveWriter_.write((const char*)&zzipfile->FileItem_, sizeof(ZZipFileItem));
		ArchiveWriter_.write(sConvString.c_str(), zzipfile->FileItem_.namelength );
	}
	return true;
}


void ZZipFile::EnumItem( const tstring& sZZipFolderPath, const ZZipFileTree::ValueTravEvent& evt, bool Recursive /*= true*/ )
{
	ZZipFileTree::PathType path;
	ZZipFileTree::String2Path(sZZipFolderPath, path);
	FileObjectsTree_.DoTrav(path, evt, Recursive);
}

//////////////////////////////////////////////////////////////////////////
// class ZZipFileTree

ZZipFile::ZZipFileTree::ZZipFileTree()
{
	ZZipTravEvent += std::make_pair(this, &ZZipFileTree::ZZipTrav);
}

ZZipFile::ZZipFileTree::~ZZipFileTree()
{
	ZZipTravEvent -= std::make_pair(this, &ZZipFileTree::ZZipTrav);
}

bool ZZipFile::ZZipFileTree::String2Path( const tstring& sInput, ZZipFileTree::PathType& path) {
	path.clear();
	size_t offset = 0;
	size_t pos = sInput.find(_T('/'), offset);

	// ��һ������ǡ�/��������
	if(pos == 0) {
		offset = 1;
		pos = sInput.find(_T('/'), offset);
	}

	while(pos != std::string::npos) {
		path.push_back(sInput.substr(offset, pos-offset));
		offset = pos+1;
		pos = sInput.find('/', offset);
	}

	if(offset < (sInput.size())) {
		path.push_back(sInput.substr(offset,sInput.size()-offset));
	}
	return false;
}

bool ZZipFile::ZZipFileTree::Path2String( const ZZipFileTree::PathType& path, tstring& sOutput) {
	sOutput.clear();
	ZZipFileTree::PathType::const_iterator cit = path.begin();
	for(; cit != path.end(); cit++) {
		sOutput += _T("/");
		sOutput += (*cit);
	}
	return true;
}

bool ZZipFile::ZZipFileTree::ZZipTrav(NodePtr node)
{
 	if(_IsLeaf(node) || ( !_IsLeaf(node) && (NULL==_Left(node)))) {
 		// ���û���ӽڵ��Ŀ¼����Ҫ���� 
 		const ValueType& v = _Value(node);
 		tstring sPath;
 		//Path2String(_Path(node), sPath);
 		//_tprintf(_T("file: %s\n\r"), sPath.c_str());
 		ZZipValueTravEvent( _Path(node), v);
 	}
 
 	return true;
}

void ZZipFile::ZZipFileTree::DoTrav( const PathType& path, const ValueTravEvent& evt, bool Recursive /*=true*/)
{
	ValueTravEvent old = ZZipValueTravEvent;
	ZZipValueTravEvent = evt;
	_MyBase::NodeTrav(path, ZZipTravEvent, Recursive);
	ZZipValueTravEvent = old;
}