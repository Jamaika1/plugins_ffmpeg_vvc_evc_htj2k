#include "except.h"
#include "libgraphengine/types.h"
#include "graphengine_except.h"

namespace zimg::graph {

void rethrow_graphengine_exception(const graphengine::Exception &e)
{
	switch (e.code) {
	case graphengine::Exception::OUT_OF_MEMORY: error::throw_<error::OutOfMemory>();
	case graphengine::Exception::USER_CALLBACK: error::throw_<error::UserCallbackFailed>(e.msg);
	default: error::throw_<error::InternalError>(e.msg);
	}
}

} // namespace zimg::graph
