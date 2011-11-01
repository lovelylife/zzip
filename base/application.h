

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "thread.h"

namespace base {
  namespace framework {

	template<class T> class MainThread;

	template<class T>
	class Application {
	public:
		Application() {};
		~Application(){};
		virtual bool run() {
			return true;
		}
		virtual bool stop() {
			return true;
		}
	private:
		MainThread<T> MainThreadLoop;
	};

	template<class T>
	class MainThread : public Thread {
		bool ThreadLoop() { return true; }
	};

  } // namespace framework
} // namespace base




#endif