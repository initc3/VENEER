#ifndef __CONTEXT_H
#define __CONTEXT_H

#include <cstring>
#include <string>
#include <vector>
#include "HashTree.h"

struct Context {
	std :: vector<HashTree *> os;
	Context& operator << (HashTree *rhs);
	static Context context;
private:
	Context() {}
};

#endif

