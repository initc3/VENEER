
#ifndef __FLATTEN_H
#define __FLATTEN_H

#include <string>
#include <vector>
#include <map>
#include "NDArray.h"

struct Flatten: public Layer {
	Flatten(const std::string &name, const std::vector<int> &input_shape);
	NDArray forward(const NDArray &input);
};

#endif
