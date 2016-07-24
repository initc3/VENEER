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
#include "ReLU.h"

using namespace std;


ReLU :: ReLU(const string &name, const vector<int> &input_shape) {
	this->name = name + "/relu";
	this->input_shape = input_shape;
	this->output_shape = input_shape;

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

NDArray ReLU :: forward(const NDArray &input) {
	assert(input.shape == input_shape);

	NDArray output = input;
	for (int i = 0; i < (int)output.array.size(); ++i) {
		output.array[i] = max(output.array[i], FP :: from(0.0f) );
	}
	return output;
}

