
#ifndef __BIAS_H
#define __BIAS_H

#include <string>
#include <vector>
#include <map>
#include "NDArray.h"

struct Bias: public Layer {
	NDArray b;

	Bias(const std::string &name, const std::vector<int> &input_shape, std::map<std::string, NDArray> &weights);
	NDArray forward(const NDArray &input);
};

#endif

