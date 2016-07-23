
#ifndef __CONV_H
#define __CONV_H

#include <string>
#include <vector>
#include <map>
#include "NDArray.h"
#include "Complex.h"

struct Conv: public Layer {
	NDArray w;
	Complex *dft_of_filter;
	Complex *dft_of_input;
	Complex *buffer;

	Conv(const std::string &name, const std::vector<int> &input_shape, std::map<std::string, NDArray> &weights);
	~Conv();

	void preprocess();
	NDArray pad(const NDArray &input);
	NDArray forward(const NDArray &);
};

#endif

