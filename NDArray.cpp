#include <vector>
#include <string>
#include <sstream>
#include <cassert>

#include "NDArray.h"

using namespace std;


NDArray :: NDArray(): shape() {
}

NDArray :: ~NDArray() {
}

string NDArray :: get_shape_str() const {
	ostringstream os;
	os << "[";
	for (int i = 0; i < (int) shape.size(); ++i) {
		if (i != 0)
			os << ", ";
		os << shape[i];
	}
	os << "]";
	return os.str();
}

void NDArray :: check_shape() const {
	int e = 1;
	for (int i = 0; i < (int) shape.size(); ++i)
		e = e * shape[i];
	assert (e == (int) array.size());
}

float& NDArray :: get(const vector<int> &pos) {
	int p = 0, mul = 1;
	for (int i = (int)pos.size() - 1; i >= 0; --i) {
		p += mul * pos[i];
		mul *= shape[i];
	}
	return array[p];
}

float NDArray :: get(const vector<int> &pos) const {
	int p = 0, mul = 1;
	for (int i = (int)pos.size() - 1; i >= 0; --i) {
		p += mul * pos[i];
		mul *= shape[i];
	}
	return array[p];
}

