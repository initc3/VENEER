
#include "Context.h"

Context& Context::operator << (HashTree *rhs) {
	os.push_back(rhs);
	return *this;
}

