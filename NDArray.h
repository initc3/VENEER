#ifndef __NDARRAY_H
#define __NDARRAY_H

#include <vector>
#include <string>
#include <sstream>
#include "FP.h"

struct NDArray {
	std::vector<int> shape;
	std::vector<FP> array;

	NDArray();
	~NDArray();
	std::string get_shape_str() const;
	void check_shape() const;
	FP& get(const std::vector<int> &pos);
	FP get(const std::vector<int> &pos) const;
};

#endif
