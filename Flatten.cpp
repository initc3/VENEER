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
#include "Flatten.h"

using namespace std;

Flatten :: Flatten(const string &name, const vector<int> &input_shape) {
	this->name = name;
	this->input_shape = input_shape;
	int _output_shape = 1;
	for (int i = 0; i < (int) input_shape.size(); ++i)
		_output_shape *= input_shape[i];
	this->output_shape = vector<int>(1, _output_shape);

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

NDArray Flatten :: forward(const NDArray &input) {
	assert(input.shape == input_shape);

	NDArray output;
	output.shape = output_shape;
	vector<int> pos(3, 0);
	for (int &i = pos[1] = 0; i < (int) input.shape[1]; ++i)	
		for (int &j = pos[2] = 0; j < (int) input.shape[2]; ++j)
			for (int &c = pos[0] = 0; c < (int) input.shape[0]; ++c)
				output.array.push_back(input.get(pos));
	return output;
}

