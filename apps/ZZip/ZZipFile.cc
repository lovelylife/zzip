
#include "typedefs.h"
#include "ref_counted.h"
#include <iostream>
#include <fstream>
#include <atlbase.h>
#include <atlconv.h>
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
	if(FileStreamReader_.is_open()) { return true; }
	FileStreamReader_.open(sFileName.c_str(), std::ios::binary);
	if(!FileStreamReader_.is_open()) {
		std::cout << "Open file " << CT2A(sFileName.c_str()) << " error." << std::endl;
		return false;
	} 

	std::cout << "Open file " << CT2A(sFileName.c_str()) << " success." << std::endl;

	FileStreamReader_.seekg(0, std::ios::end);
	size_t filesize = FileStreamReader_.tellg();
	
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
	FileStreamReader_.seekg(std::ios::beg);
	FileStreamReader_.read((char*)&hdr, sizeof(ZZipFileHeader));

	// 读取目录结构
	FileStreamReader_.seekg(hdr.offset);
	{
		char buffer[1024] = {0};
		std::streamsize size = 0;
		while(!FileStreamReader_.eof()) {
			refptr<ZZipFileObject> FileObjectPtr = new ZZipFileObject;
			FileStreamReader_.read((char*)&FileObjectPtr->FileItem_, std::streamsize(sizeof(ZZipFileItem))-1);

			// 读取路径
			std::string sTemp;
			size = FileObjectPtr->FileItem_.namelen;

			while(size > 0) {
				if(size > sizeof(buffer)) {
					FileStreamReader_.read(buffer, sizeof(buffer));
				} else {
					FileStreamReader_.read(buffer, size);
				}				
				sTemp.append(buffer, FileStreamReader_.gcount());
				size -= FileStreamReader_.gcount();
			}

			FileObjectPtr->ZZipPathFromPath(tstring(CA2T(sTemp.c_str())));
			FileObjectPtr->AddRef();
			FileObjects_.push_back(FileObjectPtr);
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
	writer.open(sTempFileName.c_str());
	if(!writer.good()) {
		return false;
	}

	// 先移动文件头大小位置
	writer.seekp(writer.beg);
	writer.seekp(sizeof(ZZipFileHeader));

	// 写入文件数据
	char buffer[2048];
	ZZipFileObjects::iterator it = FileObjects_.begin();
	for(; it != FileObjects_.end(); it++) {
		refptr<ZZipFileObject> zzipfile = (*it);
		std::ifstream f(zzipfile->sLocalPath_.c_str(), std::ios::binary);

		std::streamsize filesize = 0;
		if(f.is_open()) {
			while(!f.eof()) {
				memset(buffer, 0, sizeof(buffer));
				f.read(buffer, sizeof(buffer));
				std::streamsize ReadBytes = f.gcount();
				writer.write(buffer, ReadBytes);
				filesize += ReadBytes;
			}
			f.close();
			zzipfile->FileItem_.offset = writer.tellp();
			zzipfile->FileItem_.filesize = filesize;
		}		
	}

	ZZipFileHeader_.offset  = writer.tellp();
	it = FileObjects_.begin();
	// 写入目录结构
	for(; it != FileObjects_.end(); it++) {
		refptr<ZZipFileObject> zzipfile=(*it); 
		zzipfile->FileItem_.namelen = zzipfile->sParentPath_.size() + zzipfile->sName_.size();
		writer.write((const char*)&zzipfile->FileItem_, sizeof(ZZipFileItem)-1);
		writer.write(CT2A(zzipfile->sParentPath_.c_str()), zzipfile->sParentPath_.size());
		writer.write(CT2A(zzipfile->sName_.c_str()), zzipfile->sName_.size());
	}

	// 写文件头
	writer.seekp(writer.beg);
	writer.write((char*)&ZZipFileHeader_, sizeof(ZZipFileHeader));

	writer.close();
	FileStreamReader_.close();

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
		tstring sTemp = (*it)->sParentPath_;
		sTemp += (*it)->sName_;
		if(sZZipPath.compare(sTemp)) {
			object = (ZZipFileObject*)(*it);
			FileObjects_.erase(it);
			return object;
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
		if((object->sParentPath_ == new_object->sParentPath_)
			&& (object->sName_ == new_object->sName_)) 
		{
			// 文件已经存在
			FileObjects_.insert(it, 1, new_object);
			object->Release();
			FileObjects_.erase(it);
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
	if(FileStreamReader_.is_open()) {
		FileStreamReader_.close();
	}
}

bool ZZipFile::AddFolder( const tstring& sZZipPath, const tstring& sLocalFolder )
{
	return true;
}
