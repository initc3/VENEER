
#ifndef __LAYER_H
#define __LAYER_H

#include <string>
#include <vector>
#include "NDArray.h"

struct Layer {
	std::string name;
	std::vector<int> input_shape;
	std::vector<int> output_shape;

	Layer(): name(), input_shape(), output_shape() {}
	virtual ~Layer() {}

	virtual NDArray forward(const NDArray &input) {
		return input;
	}

private:
	Layer operator = (const Layer &);
	Layer(const Layer &);
};

#endif

