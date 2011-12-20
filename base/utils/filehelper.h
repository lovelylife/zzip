
#ifndef __FILESYS_H__
#define __FILESYS_H__
#pragma once

namespace base {
	// 一些常用文件和文件价操作
	namespace utils {
		class filehelper {
		public:
			// 检测目录是否存在
			static bool IsFolderExists(tstring sFolder);

			// 创建目录，可以多层创建；
			// 创建成功或者已存在，返回true; 否则返回false
			static bool CreateFolder(tstring sPath, bool bFile);

			// 检测文件是否存在
			static bool IsFileExists(const tstring sFileName);

		};
	} // end namespace untils

} // namespace TuduBase

#endif
