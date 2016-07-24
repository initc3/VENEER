#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>
#include <ctime>

#include "Layer.h"
#include "NDArray.h"
#include "Pool.h"

using namespace std;

Pool :: Pool(const string &name, const vector<int> &input_shape) {
	this->name = name + "/pool";
	this->input_shape = input_shape;
	this->output_shape = input_shape;
	this->output_shape[1] /= 2;
	this->output_shape[2] /= 2;

	assert(input_shape.size() == 3);

#ifdef DEBUG
	printf("layer %s\n", this->name.c_str());
	printf("\tinput_shape = ");
	for (vector<int> :: const_iterator itr(input_shape.begin()); itr != input_shape.end(); ++itr)
		printf(" %d", *itr);
	printf("\n");

	printf("\toutput_shape = ");
	for (vector<int> :: const_iterator itr(output_shape.begin()); itr != output_shape.end(); ++itr)
		printf(" %d", *itr);
	printf("\n");
#endif
}

NDArray Pool :: forward(const NDArray &input) {
	assert(input.shape == input_shape);

	NDArray output;
	output.shape = output_shape;
	for (int c = 0; c < input.shape[0]; ++c)
		for (int i = 0; i < input.shape[1]; i += 2)
			for (int j = 0; j < input.shape[2]; j += 2) {
				int _pos0[] = {c, i, j};
				int _pos1[] = {c, i + 1, j};
				int _pos2[] = {c, i, j + 1};
				int _pos3[] = {c, i + 1, j + 1};
				vector<int> pos0(_pos0, _pos0 + 3);
				vector<int> pos1(_pos1, _pos1 + 3);
				vector<int> pos2(_pos2, _pos2 + 3);
				vector<int> pos3(_pos3, _pos3 + 3);
				FP result = max( max( input.get(pos0), input.get(pos1) ), max( input.get(pos2), input.get(pos3) ) );
				output.array.push_back(result);
			}
	return output;
}

