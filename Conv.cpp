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
#include "Complex.h"
#include "FFT.h"
#include "Conv.h"
#include "Context.h"
#include "HashTree.h"

using namespace std;


Conv :: ~Conv() {
	if (dft_of_filter)
		delete [] dft_of_filter;
	if (dft_of_input)
		delete [] dft_of_input;
	if (buffer)
		delete [] buffer;
}

Conv :: Conv(const string &name, const vector<int> &input_shape, map<string, NDArray> &weights) {
	this->name = name + "/conv";
	this->input_shape = input_shape;
	this->output_shape = input_shape;
	w = weights[name + "/w"];
	this->output_shape[0] = w.shape[1];
	this->dft_of_filter = NULL;
	this->dft_of_input = NULL;
	this->buffer = NULL;

	assert(input_shape.size() == 3);
	assert(w.shape.size() == 4);

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

	printf("\tw_shape = ");
	for (vector<int> :: iterator itr(w.shape.begin()); itr != w.shape.end(); ++itr)
		printf(" %d", *itr);
	printf("\n");
#endif

	preprocess();
}

void Conv :: preprocess() {
	int filter_w = w.shape[2];
	int filter_h = w.shape[3];
	int input_c = input_shape[0];
	int input_w = input_shape[1] + filter_w - 1;
	int input_h = input_shape[2] + filter_h - 1;
	int output_c = output_shape[0];
	int la = input_w * input_h;
	int lb = filter_w * input_h;
	int len = 1;
	for ( ; len < la + lb - 1; len <<= 1);

	dft_of_filter = new Complex[input_c * output_c * len];
	dft_of_input = new Complex[input_c * len];
	buffer = new Complex[len];

	for (int oc = 0; oc < output_c; ++oc) {
		for (int ic = 0; ic < input_c; ++ic) {
			Complex *B = dft_of_filter + (ic * output_c + oc) * len;
			for (int pos = 0, cnt = (ic * output_c + oc) * filter_w * filter_h; pos < len; ++pos)
				B[pos] = pos % input_h < filter_h && pos < lb ? w.array[cnt++] : FP :: from(0.0f);
			for (int pos_l = 0, pos_r = lb - 1; pos_l < pos_r; ++pos_l, --pos_r)
				swap(B[pos_l], B[pos_r]);
			assert(int(B + len - dft_of_filter) <= input_c * output_c * len);
			DFT(B, len, 1);
		}
	}
}

NDArray Conv :: pad(const NDArray &input) {
	int image_width   = input.shape[1];
	int image_height  = input.shape[2];
	int filter_width  = w.shape[2];
	int filter_height = w.shape[3];
	int pad_t = (filter_height - 1) / 2; // floor
	int pad_b = filter_height       / 2; // ceil
	int pad_l = (filter_width - 1)  / 2; // floor
	int pad_r = filter_width        / 2; // ceil

	NDArray output;
	output.shape = vector<int>(3, 0);
	output.shape[0] = input.shape[0];
	output.shape[1] = input.shape[1] + filter_width - 1;
	output.shape[2] = input.shape[2] + filter_height - 1;

	int cnt = 0;
	for (int c = 0; c < input.shape[0]; ++c)
		for (int i = -pad_l; i < image_width + pad_r; ++i)
			for (int j = -pad_t; j < image_height + pad_b; ++j)
				if (i < 0 || i >= image_width || j < 0 || j >= image_height)
					output.array.push_back(FP :: from(0.0));
				else
					output.array.push_back(input.array[cnt++]);
	return output;
}

NDArray Conv :: forward(const NDArray &_input) {
	assert(_input.shape == input_shape);
	NDArray input = pad(_input);
	NDArray output;
	output.shape = output_shape;

	int input_c = input.shape[0];
	int input_w = input.shape[1];
	int input_h = input.shape[2];
	int output_c = output.shape[0];
	int output_w = output.shape[1];
	int output_h = output.shape[2];
	int filter_w = w.shape[2];
	int filter_h = w.shape[3];
	int la = input_w * input_h;
	int lb = filter_w * input_h;
	int len = 1;
	for ( ; len < la + lb - 1; len <<= 1);

	output.array = vector<FP>(output_c * output_w * output_h, FP :: from(0.0f));

	for (int ic = 0; ic < input_c; ++ic) {
		Complex *A = dft_of_input + len * ic;
		for (int pos = 0, cnt = ic * input_w * input_h; pos < len; ++pos) {
			A[pos] = pos < la ? input.array[cnt++] : FP :: from(0.0f);
		}
		DFT(A, len, 1);
	}

	for (int oc = 0; oc < output_c; ++oc) {
		for (int ic = 0; ic < input_c; ++ic) {
			Complex *A = dft_of_input + len * ic;
			Complex *B = dft_of_filter + (ic * output_c + oc) * len;
			Complex *C = buffer;
			for (int i = 0; i < len; i++)
				C[i] = mul(A[i], B[i]);
			DFT(C, len, -1);
			for (int i = 0; i < la + lb - 1; ++i)
				C[i].x = C[i].x / len;
			int cnt = oc * output_w * output_h;
			for (int w0 = 0; w0 + filter_w <= input_w; ++w0)
				for (int h0 = 0; h0 + filter_h <= input_h; ++h0) {
					int p = w0 * input_h + h0;
					output.array[cnt++] += C[p + lb - 1].x;
				}
		}
	}
	if (Context :: context.level == 0)
		Context :: context << new HashTree(output.array) << output.array;
	return output;
}


