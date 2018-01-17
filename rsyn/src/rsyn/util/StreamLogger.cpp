#include "StreamLogger.h"

namespace Rsyn {

StreamLogger * StreamLogger::instance = nullptr;

// -----------------------------------------------------------------------------

void StreamLogger::capture(std::ostream &out) {
	streams.push_back(std::make_tuple(&out, out.rdbuf()));
	out.rdbuf(this);
} // end method

} // end namespace
