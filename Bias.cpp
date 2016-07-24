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
#include "Bias.h"

using namespace std;


Bias :: Bias(const string &name, const vector<int> &input_shape, map<string, NDArray> &weights) {
	this->name = name + "/bias";
	this->input_shape = input_shape;
	this->output_shape = input_shape;

	b = weights[name + "/b"];

	assert(b.shape.size() == 1);
	assert(input_shape.front() == b.shape[0]);

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
	printf("\tb_shape = ");
	for (vector<int> :: iterator itr(b.shape.begin()); itr != b.shape.end(); ++itr)
		printf(" %d", *itr);
	printf("\n");
#endif
}

NDArray Bias :: forward(const NDArray &input) {
	assert(input.shape == input_shape);

	int alter = 1;
	for (int i = 1; i < (int)input_shape.size(); ++i)
		alter *= input_shape[i];

	NDArray output = input;
	int cnt = 0;
	for (int i = 0; i < (int)b.shape.size(); ++i) {
		FP bias = b.array[i];
		for (int j = 0; j < alter; ++j) {
			output.array[cnt++] += bias;
		}
	}
	return output;
}

