
#ifndef __FILESYS_H__
#define __FILESYS_H__
#pragma once

namespace base {
	// һЩ�����ļ����ļ��۲���
	namespace utils {
		class filehelper {
		public:
			// ���Ŀ¼�Ƿ����
			static bool IsFolderExists(tstring sFolder);

			// ����Ŀ¼�����Զ�㴴����
			// �����ɹ������Ѵ��ڣ�����true; ���򷵻�false
			static bool CreateFolder(tstring sPath, bool bFile);

			// ����ļ��Ƿ����
			static bool IsFileExists(const tstring sFileName);

		};
	} // end namespace untils

} // namespace TuduBase

#endif
