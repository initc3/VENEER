#ifndef __NDARRAY_H
#define __NDARRAY_H

#include <vector>
#include <string>
#include <sstream>

struct NDArray {
	std::vector<int> shape;
	std::vector<float> array;

	NDArray();
	~NDArray();
	std::string get_shape_str() const;
	void check_shape() const;
	float& get(const std::vector<int> &pos);
	float get(const std::vector<int> &pos) const;
};

#endif
