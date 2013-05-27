
#ifndef QHttp_h__
#define QHttp_h__

#include "QHttpController.h"

namespace q {

class Http : public Object {
public:
	virtual bool send(IRequestController*);
};

Http* create(int thread_number);

} // namespace q

#endif // QHttp_h__