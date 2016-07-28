
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
#include "MatMul.h"

using namespace std;


MatMul :: MatMul(const string &name, const vector<int> &input_shape, map<string, NDArray> &weights) {
	this->name = name + "/matmul";
	this->input_shape = input_shape;
	w = weights[name + "/w"];
	this->output_shape = vector<int>(1, w.shape[0]);

	assert(input_shape.size() == 1);

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

NDArray MatMul :: forward(const NDArray &input) {
	assert(input.shape == input_shape);
	NDArray output;
	output.shape = output_shape;
	int matrix_cnt = 0;
	for (int i = 0; i < w.shape[0]; ++i) {
		// multiply the i-th row with the input vector
		int input_cnt = 0;
		FP result = FP :: from(0.0f);
		for (int j = 0; j < w.shape[1]; ++j)
			result += w.array[matrix_cnt++] * input.array[input_cnt++];
		output.array.push_back(result);
	}
	if (Context :: context.level == 0)
		Context :: context << new HashTree(output.array) << output.array;
	return output;
}

