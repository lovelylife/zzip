
#include "typedefs.h"
#include "ref_counted.h"
#include <iostream>
#include <fstream>
#include <atlbase.h>
#include <atlconv.h>
#include <algorithm>
#include <io.h>
#include "ZZipFile.h"
#include "ZZipCommand.h"

ZZipFile::ZZipFile(void)
{
	memset(&ZZipFileHeader_, 0, sizeof(ZZipFileHeader));
	strncpy(ZZipFileHeader_.sig, ZZIP_SIG, ZZIP_SIG_LEN);
}

ZZipFile::~ZZipFile(void)
{

	// 	ZZipFileObjects::iterator it = FileObjects_.begin();
	// 	for(; it != FileObjects_.end(); it++) {
	// 		(*it)->Release();
	// 	}
	// 
	// 	FileObjects_.clear();
}

// public
// int ZZip::Command( int argc, _TCHAR* argv[] ) {
// 	ZZipCommand cmd(argc, argv);
// 	return 0;
// }

bool ZZipFile::Open( const tstring& sFileName ) {
	sZZipFileName_ = sFileName;
	if(sFileName.empty() ) return false;
	if(StreamReader_.is_open()) { Close(); }
	StreamReader_.open(sFileName.c_str(), std::ios::in|std::ios::binary);
	if(!StreamReader_.good()) {
		std::cout << "Open file " << CT2A(sFileName.c_str()) << " error." << std::endl;
		return false;
	} 

	std::cout << "Open file " << CT2A(sFileName.c_str()) << " success." << std::endl;

	StreamReader_.seekg(0, std::ios::end);
	uint64 filesize = StreamReader_.tellg();
	
	// 文件不为空且头部大小小于sizeof(ZZipFileHeader则是个无效的文件
	if(filesize <= 0) {
		// 空文件，不需要解析
		return true;
	}
	ZZipFileHeader hdr = {0};
	if(filesize < sizeof(ZZipFileHeader)) {
		std::cout << "Invalid file data!" << std::endl;
		return false;
	}		

	// 读取文件头部分
	StreamReader_.seekg(0, std::ios::beg);
	StreamReader_.read((char*)&hdr, sizeof(ZZipFileHeader));

	// 读取目录结构
	StreamReader_.seekg(hdr.offset, std::ios::beg);
	{
		char buffer[1024] = {0};
		std::streamsize size = 0;
		std::streamsize restsize = filesize - hdr.offset;
		while((!StreamReader_.eof()) && restsize > 0) {
			refptr<ZZipFileObject> FileObjectPtr = new ZZipFileObject;
			StreamReader_.read((char*)&FileObjectPtr->FileItem_, std::streamsize(sizeof(ZZipFileItem)));
			restsize -= sizeof(ZZipFileItem);
			// 读取路径
			std::string sTemp;
			size = FileObjectPtr->FileItem_.namelength;

			while((size > 0) && (!StreamReader_.eof())) {
				if(size > sizeof(buffer)) {
					StreamReader_.read(buffer, sizeof(buffer));
				} else {
					StreamReader_.read(buffer, size);
				}
				if(!StreamReader_.gcount()) break;
				sTemp.append(buffer, StreamReader_.gcount());
				size -= StreamReader_.gcount();
				restsize -= StreamReader_.gcount(); 
			}
			
			// std::cout << "file position: " << StreamReader_.tellg() << std::endl;
			if(!sTemp.empty()) {
				FileObjectPtr->ZZipPathFromPath(tstring(CA2T(sTemp.c_str())));
				FileObjectPtr->AddRef();
				FileObjects_.push_back(FileObjectPtr);
			}
			
			if(StreamReader_.eof() || (restsize <= 0)) {
				break;
			}
		}
	}
	return true;
}

// 保存文件
bool ZZipFile::Save()
{
	// 在当前文件所在目录创建临时文件
	tstring sTempFileName = sZZipFileName_;
	sTempFileName += _T(".__zzip__");
	std::ofstream writer;
	writer.open(sTempFileName.c_str(), std::ios::out|std::ios::binary);
	if(!writer.good()) { return false; }

	// 先移动文件头大小位置
	writer.seekp(sizeof(ZZipFileHeader), std::ios::beg);
//	writer.seekp(sizeof(ZZipFileHeader));

	// 写入文件数据
	char buffer[2048];
	ZZipFileObjects::iterator it = FileObjects_.begin();
	for(; it != FileObjects_.end(); it++) {
		refptr<ZZipFileObject> zzipfile = (*it);
		// 记录文件偏移量
		
		// 读取本地文件
		std::ifstream f(zzipfile->sLocalPath_.c_str(), std::ios::in|std::ios::binary);
		if(!f.good()) { 
			zzipfile->FileItem_.filesize = 0;
			zzipfile->FileItem_.namelength = 0;
			continue; 
		}

		zzipfile->FileItem_.offset = writer.tellp();
		
		// 计算文件大小
		std::streamsize filesize = 0;
		f.seekg(std::ios::beg, std::ios::end);
		zzipfile->FileItem_.filesize = f.tellg();
		f.seekg(0,std::ios::beg);
		std::streamsize filecount = zzipfile->FileItem_.filesize;
		// 读取文件内容并写入Writer
		while((!f.eof()) && (filecount > 0)) {
			memset(buffer, 0, sizeof(buffer));
			f.read(buffer, sizeof(buffer));
			std::streamsize ReadBytes = f.gcount();
			writer.write(buffer, ReadBytes);
			filecount -= ReadBytes;
		}
		f.close();
	}

	// 设置文件目录结构位置
	ZZipFileHeader_.offset  = writer.tellp();
	it = FileObjects_.begin();
	char szPath[_MAX_PATH] = {0};
	std::streamsize sizecount = 0;
	std::string sBuffer;
	// 写入目录结构
	for(; it != FileObjects_.end(); it++) {
		refptr<ZZipFileObject> zzipfile=(*it); 
		//zzipfile->FileItem_.namelen = zzipfile->sPath_.size();
		zzipfile->FileItem_.namelength = WideCharToMultiByte(CP_ACP, 0, zzipfile->sPath_.c_str(), wcslen(zzipfile->sPath_.c_str()), szPath, _MAX_PATH, NULL, NULL);
		if(zzipfile->FileItem_.namelength < 1) { 
			continue; 
		}
		sizecount += zzipfile->FileItem_.namelength;
		writer.write((const char*)&zzipfile->FileItem_, sizeof(ZZipFileItem));
		writer.write(szPath, zzipfile->FileItem_.namelength );
		//sBuffer.append((const char*)&zzipfile->FileItem_, sizeof(ZZipFileItem));
		sBuffer.append(szPath, zzipfile->FileItem_.namelength );
	}
	std::cout << "archives size: " << sizecount << std::endl;
	// 写文件头
	writer.seekp(0, std::ios::beg);
	writer.write((char*)&ZZipFileHeader_, sizeof(ZZipFileHeader));

	writer.close();
	StreamReader_.close();

	remove(CT2A(sZZipFileName_.c_str()));
	// 使用当前名称
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

bool ZZipFile::AddFile( const tstring& sZZipPath, const tstring& sLocalFileName )
{
	refptr<ZZipFileObject> new_object = new ZZipFileObject();
	new_object->sLocalPath_ = sLocalFileName;
	new_object->ZZipPathFromPath(sZZipPath);

	// 检测是否存在该文件
	bool bFileExists = false;
	ZZipFileObjects::iterator it = FileObjects_.begin();
	for(; it != FileObjects_.end(); it++) {
		refptr<ZZipFileObject> object = (*it);
		if(object->sPath_ == new_object->sPath_) {
			// 文件已经存在
			object->Release();
			new_object->AddRef();
			*it = new_object;
			bFileExists = true;
			break;
		}
	}
	
	if(!bFileExists) {
		new_object->AddRef();
		FileObjects_.push_back(new_object);
	}	
	return true;
}

void ZZipFile::Close()
{
	ZZipFileObjects::iterator it = FileObjects_.begin();
	for(; it != FileObjects_.end(); it++) {
		(*it)->Release();
	}
	FileObjects_.clear();
	StreamReader_.clear();
	if(StreamReader_.is_open()) {
		StreamReader_.close();		
	}
}

bool ZZipFile::AddFolder( tstring sZZipPath, tstring sLocalFolder )
{
	// 检查sZZipPath是否("/") 
	if(sZZipPath.empty()) {
		sZZipPath.append(1, _T('/'));
	} else {
		std::replace(sZZipPath.begin(), sZZipPath.end(), _T('\\'), _T('/'));
		if(sZZipPath[sZZipPath.size()-1] != _T('/')) {
			sZZipPath.append(1, _T('/'));
		}
	}
	//查找dir中的子目录 
	long hFile; 
	_tfinddata_t FileData = {0}; 
	if(_tchdir(sLocalFolder.c_str())!=0) return false;

	// 检查("/") 
	std::replace(sLocalFolder.begin(), sLocalFolder.end(), _T('\\'), _T('/'));
 	if(sLocalFolder[sLocalFolder.size()-1] != _T('/')) {
 		sLocalFolder.append(1, _T('/'));
 	}

	if ((hFile=_tfindfirst(_T("*.*"), &FileData)) != -1) {
		do { 
			//检查是不是目录 
			//如果是,再检查是不是 . 或 .. 
			//如果不是,进行迭代
			if ((FileData.attrib & _A_SUBDIR)) { 
				if ((_tcscmp(FileData.name, _T(".")) != 0 )	
					&& (_tcscmp(FileData.name, _T("..")) != 0)
				) {
					tstring sSubDir=sLocalFolder;
					sSubDir += FileData.name;
					sSubDir.append(1, _T('/'));

					tstring sSubZZipPath = sZZipPath;
					sSubZZipPath += FileData.name;
					sSubZZipPath.append(1, _T('/'));

					// 添加目录
					if(!AddFolder(sSubZZipPath, sSubDir)) return false;
				}
			} else {
				tstring sLocalPath;
				sLocalPath = sLocalFolder;
				sLocalPath += FileData.name;
				tstring sPath;
				sPath = sZZipPath;
				sPath += FileData.name;
				// 添加文件
				AddFile(sPath, sLocalPath);
			}
		} while (_tfindnext(hFile,&FileData) == 0); 
		_findclose(hFile); 
	} 
	return true;
}

const ZZipFileObject* ZZipFile::Find( const tstring& lpszPath )
{
	ZZipFileObject* p = NULL;
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
	ATLASSERT((zzipfile != NULL)&&(lpBuffer != NULL) && (size > 0) );
	if(StreamReader_.good()) {
		if((zzipfile->FileItem_.offset>0) && ((offset >= 0) && (offset < zzipfile->FileItem_.namelength) )) {
			uint64 fileoffset = zzipfile->FileItem_.offset+offset;
			uint64 readsize = size;
			if(readsize + offset > zzipfile->FileItem_.namelength) {
				readsize = zzipfile->FileItem_.namelength - offset;
			}

			StreamReader_.seekg(fileoffset, std::ios::beg);
			StreamReader_.read((char*)lpBuffer, readsize);
			return StreamReader_.gcount();
		}
	}

	return 0;
}
