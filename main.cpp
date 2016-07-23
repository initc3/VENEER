
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

using namespace std;

const int HASH_BYTES = 32;

//#define DEBUG

/****************** The butterfly tree stuff ****************************************/

struct node {
	char hash[HASH_BYTES];
};

/**
 * Calculate the sha2 of given string s
 * @s pointer to input string
 * @n length of input string
 * @o output string, HASH_BYTES byte
 */
void sha2_digest(char *s, int n, char *o) {
	string input = string(s, s + n);
	string output = sha256(input);
	strcpy(o, output.c_str());
}

/**
 * Calculate the hash function for a given float x
 * @x the given floating point number
 * @c the output string, HASH_BYTES byte
 */
void float_to_byte_array(float x, char *o) {
	union C {
		float x;
		int y;
	};
	C result;
	result.x = x;
	for (int i = 0; i < HASH_BYTES; ++i) {
		// TODO: Warning: this kind of hash function will result in bunch of zeros in c
		o[i] = (char)(result.y & 255);
		result.y >>= 8;
	}
}

/**
 * Build hash tree from array
 * @array the array to build hash tree from
 * @l array[l ... r] (inclusive) is to be built
 * @r as above
 * @id the id of the current tree node
 * @tree the resulting tree
 */
void build_butterfly(float *array, int l, int r, int id, node *tree) {
	if (l == r) {
		float_to_byte_array(array[l], tree[id].hash);
		return;
	}
	int mid((l + r) >> 1);
	int left_id = id << 1;
	int right_id = left_id + 1;
	build_butterfly(array, l, mid, left_id, tree);
	build_butterfly(array, mid + 1, r, right_id, tree);
	static char concat[HASH_BYTES << 1];
	memcpy(concat, tree[left_id].hash, HASH_BYTES);
	memcpy(concat + HASH_BYTES, tree[right_id].hash, HASH_BYTES);
	sha2_digest(concat, HASH_BYTES << 1, tree[id].hash);
}

/**
 * @array input array
 * @length length of the array, should be 2 ^ d
 */
void build_butterfly(float *array, int length) {
	node *tree = new node[length * 2];
	build_butterfly(array, 0, length - 1, 0, tree);
}

/**
 * Assume the hash value in root of the subtree tree1[id], tree2[id] are different,
 * find out the leftmost position that differs.
 * @tree1 the array representing the first tree
 * @tree2 the array representing the second tree
 * @l the left bound
 * @r the right bound
 * @id the position in the tree
 */
int compare_butterfly(node *tree1, node *tree2, int l, int r, int id) {
	if (l == r)
		return l;
	int mid = (l + r) >> 1;
	int left_id = id << 1;
	int right_id = left_id + 1;
	if (memcmp(&tree1[left_id], &tree2[left_id], HASH_BYTES) != 0)
		return compare_butterfly(tree1, tree2, l, mid, left_id);
	if (memcmp(&tree1[right_id], &tree2[right_id], HASH_BYTES) != 0)
		return compare_butterfly(tree1, tree2, mid + 1, r, right_id);
	assert(0); // this should not happen
	return -1;
}

/**
 * Compare two trees, find the leftmost position that differs. If two trees are the same, return -1
 * @tree1 the first tree
 * @tree2 the second tree
 * @length the length of the sequence, should be 2 ^ d
 */
int compare_butterfly(node *tree1, node *tree2, int length) {
	if (memcmp(tree1, tree2, HASH_BYTES) == 0)
		return -1;
	return compare_butterfly(tree1, tree2, 0, length - 1, 0);
}

/****************** Loading pretrained model ****************************************/

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
		array.array = vector<float>(e, 0.0);
		for (int j = 0; j < e; ++j) {
			fin >> array.array[j];
		}
		result[name] = array;
	}
	return result;
}


int main() {
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
	
	int N, correct = 0;
	fin >> N;
	for (int test_id = 0; test_id < N; ++test_id) {
		NDArray image;
		image.shape = input_shape;
		image.array = vector<float>(784, 0.0f);

		int label;
		fin >> label;
		for (int i = 0; i < 784; ++i)
			fin >> image.array[i];
		int predict = model.predict(image);

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
	}
	printf("%.3f\n", (float) correct / N);
	cout << "Time used:" << float(clock()) / CLOCKS_PER_SEC << endl;
	return 0;
}

