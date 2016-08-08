
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

#include "sha256.h"
#include "NDArray.h"
#include "Layers.h"
#include "Model.h"
#include "HashTree.h"
#include "FFT.h"

#define Show(x) cout << #x << " = " << (x) << endl

using namespace std;

map<string, NDArray> load_pretrained_model(const char *path) {
	map<string, NDArray> result;
	ifstream fin(path);
	int n;
	fin >> n;
	for (int i = 0; i < n; ++i) {
		int d, tmp, e = 1;
		string name;
		fin >> name >> d;
		NDArray array = NDArray();
		for (int j = 0; j < d; ++j) {
			fin >> tmp;
			array.shape.push_back(tmp);
			e *= tmp;
		}
		array.array = vector<FP>(e, FP());
		for (int j = 0; j < e; ++j) {
			fin >> array.array[j];
		}
		result[name] = array;
	}
	return result;
}


int main(int argc, char **argv) {
	Context :: context.parse(argc, argv);

	int _input_shape[] = {1, 28, 28};
	pair<string, string> _layers[] = {
		make_pair("conv", "conv1"),
		make_pair("bias", "conv1"),
		make_pair("relu", "conv1"),
		make_pair("pool", "pool1"),

		make_pair("conv", "conv2"),
		make_pair("bias", "conv2"),
		make_pair("relu", "conv2"),
		make_pair("pool", "pool2"),

		make_pair("flatten", "flatten"),

		make_pair("matmul", "fc1"),
		make_pair("bias", "fc1"),
		make_pair("relu", "fc1"),

		make_pair("matmul", "fc2"),
		make_pair("bias", "fc2"),
		make_pair("relu", "fc2")
	};

	vector<pair<string, string> > layers(_layers, _layers + 15);
	vector<int> input_shape(_input_shape, _input_shape + 3);
	map<string, NDArray> weights = load_pretrained_model("pretrained.txt");
	Model model(layers, input_shape, weights);

	ifstream fin("test_images.txt");

	int start_time = clock();

	int N, correct = 0;
	fin >> N;
	N = 1;
#ifdef MEASURE_COMPUTATION
	Show (HashTree :: allocated);
#endif
	for (int test_id = 0; test_id < N; ++test_id) {
#ifdef MEASURE_COMPUTATION
		FP :: cnt_add = 0;
		FP :: cnt_sub = 0;
		FP :: cnt_mul = 0;
		FP :: cnt_div = 0;
		FP :: cnt_cmp = 0;
		HashTree :: allocated = 0;
		cnt_FFT = 0;
#endif
		NDArray image;
		image.shape = input_shape;
		image.array = vector<FP>(784, FP :: from(0.0f));

		int label;
		fin >> label;
		for (int i = 0; i < 784; ++i)
			fin >> image.array[i];
		int predict = model.predict(image);
		if (Context :: context.mode == 0)
			Context :: context.makePrediction(predict);

		if (predict == label) {
			printf(".");
			++correct;
		}
		else {
			printf("x");
		}
		if (test_id % 5 == 4 || test_id == N - 1) {
			puts("");
		}

#ifdef MEASURE_COMPUTATION
		Show (FP :: cnt_add);
		Show (FP :: cnt_sub);
		Show (FP :: cnt_mul);
		Show (FP :: cnt_div);
		Show (FP :: cnt_cmp);
		Show (HashTree :: allocated);
		Show (cnt_FFT);
#endif
	}
	printf("%.3f\n", (float) correct / N);
	cout << "Time used:" << float(clock() - start_time) / CLOCKS_PER_SEC << endl;
	return 0;
}

