
#ifndef __RELU_H
#define __RELU_H

#include <string>
#include <vector>
#include <map>
#include "NDArray.h"

struct ReLU: public Layer {
	ReLU(const std::string &name, const std::vector<int> &input_shape);
	NDArray forward(const NDArray &input);
};

#endif

