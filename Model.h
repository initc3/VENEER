#ifndef __MODEL_H
#define __MODEL_H

#include <vector>
#include <string>
#include <map>
#include "Layers.h"
#include "NDArray.h"

struct Model {
	std::vector<Layer *> layers;
	Model(const std::vector<std::pair<std::string, std::string> > &layers, std::vector<int> input_shape, std::map<std::string, NDArray> &weights);
	~Model();
	int predict(NDArray feed);
};

#endif

