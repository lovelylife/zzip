
#include "stdafx.h"
#include "typedefs.h"
#include "ref_counted.h"
#include <iostream>
#include <fstream>
#include <atlbase.h>
#include <atlconv.h>
#include <algorithm>
#include <io.h>
#include <strstream>
#include "ZZipFile.h"
#include "ZZipCommand.h"

#ifdef _WIN32
#include <shlwapi.h>

#endif

static const TCHAR ZZipTmpFileName[]	= _T(".__zzip__");
static const TCHAR ZZipTmpFolderName[]  = _T("\\__zzip_cache\\");


ZZipFile::ZZipFile(void) 
: StreamPtr_(NULL)
, StreamWriterPtr_(NULL)
, Type_(TypeUnknow)
, OpenMode_(OpenMode_|OpenModeWrite)
{
	memset(&ZZipFileHeader_, 0, sizeof(ZZipFileHeader));
	strncpy(ZZipFileHeader_.sig, ZZIP_SIG, ZZIP_SIG_LEN);
}

ZZipFile::~ZZipFile(void) {
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
	HRSRC hRsrc = FindResource(hInstance, MAKEINTRESOURCE(pszResourceName), sType.c_str());
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
			refptr<ZZipFileObject> FileObjectPtr = new ZZipFileObject;
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

			// std::cout << "file position: " << StreamReaderPtr_->tellg() << std::endl;
			if(!sTemp.empty()) {
				FileObjectPtr->ZZipPathFromPath(tstring(CA2T(sTemp.c_str())));
				FileObjectPtr->AddRef();
				FileObjects_.push_back(FileObjectPtr);
			}

			if(pStream->eof() || (restsize <= 0)) {
				break;
			}
		}
	}

	return true;
}


// �����ļ�
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

	// д���ļ�����
	char buffer[2048];
	ZZipFileObjects::iterator it = FileObjects_.begin();
	for(; it != FileObjects_.end(); it++) {
		refptr<ZZipFileObject> zzipfile = (*it);
		// ��¼�ļ�ƫ����
		
		// ��ȡ�����ļ�
		std::ifstream f(zzipfile->sLocalPath_.c_str(), std::ios::in|std::ios::binary);
		if(!f.good()) { 
			zzipfile->FileItem_.filesize = 0;
			zzipfile->FileItem_.namelength = 0;
			continue; 
		}

		zzipfile->FileItem_.offset = StreamWriterPtr_->tellp();
		
		// �����ļ���С
		std::streamsize filesize = 0;
		f.seekg(std::ios::beg, std::ios::end);
		zzipfile->FileItem_.filesize = f.tellg();
		f.seekg(0,std::ios::beg);
		std::streamsize filecount = zzipfile->FileItem_.filesize;
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

	// �����ļ�Ŀ¼�ṹλ��
	ZZipFileHeader_.offset  = StreamWriterPtr_->tellp();
	it = FileObjects_.begin();
	char szPath[_MAX_PATH] = {0};
	std::streamsize sizecount = 0;
//	std::string sBuffer;
	// д��Ŀ¼�ṹ
	for(; it != FileObjects_.end(); it++) {
		refptr<ZZipFileObject> zzipfile=(*it); 
		//zzipfile->FileItem_.namelen = zzipfile->sPath_.size();
		zzipfile->FileItem_.namelength = WideCharToMultiByte(CP_ACP, 0, zzipfile->sPath_.c_str(), wcslen(zzipfile->sPath_.c_str()), szPath, _MAX_PATH, NULL, NULL);
		if(zzipfile->FileItem_.namelength < 1) { 
			continue; 
		}
		sizecount += zzipfile->FileItem_.namelength;
		StreamWriterPtr_->write((const char*)&zzipfile->FileItem_, sizeof(ZZipFileItem));
		StreamWriterPtr_->write(szPath, zzipfile->FileItem_.namelength );
	}
//	std::cout << "archives size: " << sizecount << std::endl;
	// д�ļ�ͷ
	StreamWriterPtr_->seekp(0, std::ios::beg);
	StreamWriterPtr_->write((char*)&ZZipFileHeader_, sizeof(ZZipFileHeader));
	StreamWriterPtr_->close();

	remove(CT2A(sZZipFileName_.c_str()));

	tstring sTempFileName = sZZipFileName_;
	sTempFileName += _T(".__zzip__");
	// ʹ�õ�ǰ����
	int nReturnCode = _trename(sTempFileName.c_str(), sZZipFileName_.c_str());

	if(nReturnCode != 0) {
		std::cout << "rename: error(code:" << nReturnCode << ")" <<  std::endl;
		return false;
	}

	return true;
}

refptr<ZZipFileObject> ZZipFile::RemoveFile( const tstring& sZZipPath )
{
	refptr<ZZipFileObject> object;
	ZZipFileObjects::iterator it = FileObjects_.begin();
	for(; it != FileObjects_.end(); it++) {
		tstring sTemp = (*it)->sPath_;
		if(sZZipPath.compare(sTemp)) {
			object = (ZZipFileObject*)(*it);
			FileObjects_.erase(it);
			break;
		}
	}
	return object;
}

refptr<ZZipFileObject> ZZipFile::AddFile( const tstring& sZZipPath, const tstring& sLocalFileName, bool bOverwrite )
{
	refptr<ZZipFileObject> FileObjectPtr = FindFile(sZZipPath);
	if(FileObjectPtr == NULL) {
		FileObjectPtr = new ZZipFileObject();

		FileObjectPtr->sLocalPath_ = sLocalFileName;
		FileObjectPtr->ZZipPathFromPath(sZZipPath);
		FileObjectPtr->AddRef();
		FileObjects_.push_back(FileObjectPtr.get());
	} else {
		if(bOverwrite) {
			FileObjectPtr->ZZipPathFromPath(sZZipPath);
			FileObjectPtr->sLocalPath_ = sLocalFileName;
		}
	}

	return FileObjectPtr;
}


#ifdef _WIN32
refptr<ZZipFileObject> ZZipFile::AddFile(const tstring& sZZipPath, IStream* pStream, bool bOverwrite)
{
	ATLASSERT((OpenMode_ & OpenModeWrite) && (Type_ == TypeFile));

	if(StreamWriterPtr_ == NULL) return NULL;

	// ����ļ��Ƿ����
	refptr<ZZipFileObject> FileObjectPtr = FindFile(sZZipPath);
	if(FileObjectPtr == NULL) {
		FileObjectPtr = new ZZipFileObject;
		FileObjectPtr->ZZipPathFromPath(sZZipPath);
		FileObjectPtr->AddRef();
		FileObjects_.push_back(FileObjectPtr.get());
	} else {
		if(!bOverwrite) {
			return FileObjectPtr;
		}
	}



	// ���浽��ʱ�ļ�
	tstring sDir = sZZipFileName_;
	PathRemoveFileSpec((LPTSTR)sDir.c_str());
	sDir.insert(_tcslen(sDir.c_str()), ZZipTmpFolderName);
	std::replace(sDir.begin(), sDir.end(), _T('/'), _T('\\'));
	if(_taccess(sDir.c_str(), 0) == -1) {
		if(0 != _tmkdir(sDir.c_str())){
			// �����ļ���ʧ��
			return NULL;
		}
	}
	// ������ʱ�ļ�
	TCHAR sTmpFileName[_MAX_PATH];
	if(!GetTempFileName(sDir.c_str(), NULL, 0, sTmpFileName)) {
		// GetLastError()
		return NULL;
	}

	FileObjectPtr->ZZipPathFromPath(sZZipPath);
	FileObjectPtr->sLocalPath_ = sTmpFileName;
	
	std::ofstream TempFileStream;
	TempFileStream.open(sTmpFileName, std::ofstream::binary);
	// �����ļ�ʧ��GetLastError
	if(!TempFileStream.good()) {
		return NULL;
	}

	//д�ļ�����
	HGLOBAL hGlobal = NULL;
	GetHGlobalFromStream(pStream, &hGlobal); // ��ȡ IStream ���ڴ���
	SIZE_T size = GlobalSize(hGlobal);
	
	char* pBuffer = (char*)GlobalLock(hGlobal); 	
	TempFileStream.write(pBuffer,static_cast<long>(size));
	GlobalUnlock(hGlobal);

	return FileObjectPtr;
}

#endif

void ZZipFile::Close()
{
	ATLASSERT(StreamPtr_ != NULL);
	ZZipFileObjects::iterator it = FileObjects_.begin();
	for(; it != FileObjects_.end(); it++) {
		(*it)->Release();
	}
	FileObjects_.clear();
	StreamPtr_->clear();
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

refptr<ZZipFileObject> ZZipFile::FindFile( const tstring& lpszPath )
{
	refptr<ZZipFileObject> p;
	ZZipFileObjects::iterator it = FileObjects_.begin();
	for(; it != FileObjects_.end(); it++) {
		refptr<ZZipFileObject> object = (*it);
		if(object->sPath_ == lpszPath) {
			p = object;
			break;
		}
	}

	return p;
}

int64 ZZipFile::ReadData( const ZZipFileObject* zzipfile, uint64 offset, void* lpBuffer, uint64 size )
{
	ATLASSERT(StreamPtr_ != NULL && (zzipfile != NULL)&&(lpBuffer != NULL) && (size > 0) );
	// ATLASSERT((zzipfile != NULL)&&(lpBuffer != NULL) && (size > 0) );
	if(StreamPtr_->good()) {
		if((zzipfile->FileItem_.offset>0) && ((offset >= 0) && (offset < zzipfile->FileItem_.filesize) )) {
			int64 fileoffset = zzipfile->FileItem_.offset+offset;
			int64 readsize = size;
			if((readsize + offset) > zzipfile->FileItem_.filesize) {
				readsize = zzipfile->FileItem_.filesize - offset;
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
	ZZipFileObjects::iterator it = FileObjects_.begin();
	for(; it != FileObjects_.end(); it++) {
		(*it)->Release();
	}
	FileObjects_.clear();
}

bool ZZipFile::ExtractFile( const tstring& sZZipPath,IStream** pStream ) {
	// �����ļ�
	const ZZipFileObject* p = FindFile(sZZipPath);
	if(p && (p->filesize() > 0)) {
		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, p->filesize());
		void* pData = GlobalLock(hGlobal);
		StreamPtr_->seekg(p->offset(), std::ios::beg);
		StreamPtr_->read((char*)pData, p->filesize());				
		GlobalUnlock(hGlobal);
		return (CreateStreamOnHGlobal(hGlobal, TRUE, pStream) == S_OK);
	}

	return false;
}

