#pragma once

namespace base {
	
//! ��
  namespace stream {
	// stream �ӿ�
	class istream {
		size_t read(void* sData, size_t size);
		size_t write(void* sData, size_t size)
	};
  } // namespace stream
} // namespace base
