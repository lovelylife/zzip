
#ifndef Object_h__
#define Object_h__

#include <RefCounted.h>
namespace q {

class Object : public base::RefCountedThreadSafe<Object> {
public:
	Object() {}
	virtual ~Object() {}
};

} // namespace q

#endif