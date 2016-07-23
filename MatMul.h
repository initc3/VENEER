
#ifndef __MATMUL_H
#define __MATMUL_H

#include <string>
#include <vector>
#include <map>
#include "NDArray.h"
#include "Layer.h"

struct MatMul: public Layer {
	NDArray w;

	MatMul(const std::string &name, const std::vector<int> &input_shape, std::map<std::string, NDArray> &weights);
	NDArray forward(const NDArray &input);
};

#endif

