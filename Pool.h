
#ifndef __POOL_H
#define __POOL_H

#include <string>
#include <vector>
#include <map>
#include "NDArray.h"

struct Pool: public Layer {
	Pool(const std::string &name, const std::vector<int> &input_shape);
	NDArray forward(const NDArray &input);
};

#endif

