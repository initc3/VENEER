
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <vector>
#include <map>

using namespace std;

const int HASH_BYTES = 32;
const double PI = 3.14159265358979323846264338327950288;

/****************** The butterfly tree stuff ****************************************/

struct node {
	char hash[HASH_BYTES];
};

/**
 * Calculate the sha2 of given string s
 * TODO: currently, it is not sha2, pretend to be sha2.
 * @s pointer to input string
 * @n length of input string
 * @o output string, HASH_BYTES byte
 */
void sha2_digest(char *s, int n, char *o) {
	unsigned long long a[4] = {17, 19, 23, 29};
	unsigned long long result[4] = {0, 0, 0, 0};
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < 4; ++j)
			result[j] = result[j] * a[j] + s[i];
	int p = 0;
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 8; ++j) {
			o[p++] = (char)(result[j] & 255);
			result[j] >>= 8;
		}
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
	char *concat = new char[HASH_BYTES << 1]; // TODO: actually do not have to allocate every time
	memcpy(concat, tree[left_id].hash, HASH_BYTES);
	memcpy(concat + HASH_BYTES, tree[right_id].hash, HASH_BYTES);
	sha2_digest(concat, HASH_BYTES << 1, tree[id].hash);
	delete [] concat;
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

struct NDArray {
	vector<int> shape;
	vector<float> array;

	NDArray(): shape() {
	}

	~NDArray() {
	}

	float& get(const vector<int> &pos) {
		int p = 0, mul = 1;
		for (int i = (int)pos.size() - 1; i >= 0; --i) {
			p += mul * pos[i];
			mul *= shape[i];
		}
		return array[p];
	}

	float get(const vector<int> &pos) const {
		int p = 0, mul = 1;
		for (int i = (int)pos.size() - 1; i >= 0; --i) {
			p += mul * pos[i];
			mul *= shape[i];
		}
		return array[p];
	}
};

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
			cout << tmp << " ";
		}
		cout << endl;
		array.array = vector<float>(e, 0.0);
		for (int j = 0; j < e; ++j) {
			fin >> array.array[j];
		}
		result[name] = array;
	}
	return result;
}

/************************ FFT ***************************/

// handwritten complex number class to reduce overhead
struct Complex {
	float x;
	float y;

	Complex() : x(0.0), y(0.0) {
	}

	Complex(float x, float y): x(x), y(y) {
	}
};

#define mul(a, b) (Complex(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x))
void FFT(Complex P[], int n, int oper) {
	for (int i = 1, j = 0; i < n - 1; i++) {
		for (int s = n; j ^= s >>= 1, ~j & s; );
		if (i < j) swap(P[i], P[j]);
	}
	for (int d = 0; (1 << d) < n; d++) {
		int m = 1 << d, m2 = m * 2;
		double p0 = PI / m * oper;
		Complex unit_p0(cos(p0), sin(p0));
		for (int i = 0; i < n; i += m2) {
			Complex unit(1.0, 0.0);
			for (int j = 0; j < m; j++) {
				Complex &P1 = P[i + j + m], &P2 = P[i + j];
				Complex t = mul(unit, P1);
				P1 = Complex(P2.x - t.x, P2.y - t.y);
				P2 = Complex(P2.x + t.x, P2.y + t.y);
				unit = mul(unit, unit_p0);
			}
		}
	}
}
#undef mul

/******************** LAYERS ********************/

struct Layer {
//	virtual NDArray forward(const NDArray &input) = 0;
	string name;
	vector<int> input_shape, output_shape;
};

struct Conv: public Layer {

	Conv(const string &name, const vector<int> &input_shape, map<string, NDArray> &weights) {
		this->name = name + "/conv";
		this->input_shape = input_shape;
		this->output_shape = input_shape;
		NDArray w(weights[name + "/w"]);
		this->output_shape[0] = w.shape[1];

		assert(input_shape.size() == 3);
		assert(w.shape.size() == 4);

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
	}

};

struct Bias: public Layer {

	NDArray b;

	Bias(const string &name, const vector<int> &input_shape, map<string, NDArray> &weights) {
		this->name = name + "/bias";
		this->input_shape = input_shape;
		this->output_shape = input_shape;

		b = weights[name + "/b"];

		assert(b.shape.size() == 1);
		assert(input_shape.front() == b.shape[0]);

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
	}

	NDArray forward(const NDArray &input) {
		int alter = 1;
		for (int i = 1; i < (int)input_shape.size(); ++i)
			alter *= input_shape[i];

		NDArray output = input;
		int cnt = 0;
		for (int i = 0; i < (int)b.shape.size(); ++i) {
			float bias = b.array[i];
			for (int j = 0; j < alter; ++j) {
				output.array[cnt++] += bias;
			}
		}
		return output;
	}
};

struct ReLU: public Layer {

	ReLU(const string &name, const vector<int> &input_shape, map<string, NDArray> &weights) {
		this->name = name + "/relu";
		this->input_shape = input_shape;
		this->output_shape = input_shape;

		printf("layer %s\n", this->name.c_str());
		printf("\tinput_shape = ");
		for (vector<int> :: const_iterator itr(input_shape.begin()); itr != input_shape.end(); ++itr)
			printf(" %d", *itr);
		printf("\n");

			printf("\toutput_shape = ");
		for (vector<int> :: const_iterator itr(output_shape.begin()); itr != output_shape.end(); ++itr)
			printf(" %d", *itr);
		printf("\n");
	}

	NDArray forward(const NDArray &input) {
		NDArray output = input;
		for (int i = 0; i < (int)output.array.size(); ++i)
			output.array[i] = max(output.array[i], 0.0f);
		return output;
	}

};

struct Pool: public Layer {
	Pool(const string &name, const vector<int> &input_shape, map<string, NDArray> &weights) {
		this->name = name + "/pool";
		this->input_shape = input_shape;
		this->output_shape = input_shape;
		this->output_shape[1] /= 2;
		this->output_shape[2] /= 2;

		assert(input_shape.size() == 3);
		printf("layer %s\n", this->name.c_str());
		printf("\tinput_shape = ");
		for (vector<int> :: const_iterator itr(input_shape.begin()); itr != input_shape.end(); ++itr)
			printf(" %d", *itr);
		printf("\n");

		printf("\toutput_shape = ");
		for (vector<int> :: const_iterator itr(output_shape.begin()); itr != output_shape.end(); ++itr)
			printf(" %d", *itr);
		printf("\n");
	}

	NDArray forward(const NDArray &input) {
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
					float result = max( max( input.get(pos0), input.get(pos1) ), max( input.get(pos2), input.get(pos3) ) );
					output.array.push_back(result);
				}
		return output;
	}

};

struct Flatten: public Layer {
	Flatten(const string &name, const vector<int> &input_shape, map<string, NDArray> &weights) {
		this->name = name;
		this->input_shape = input_shape;
		int _output_shape = 1;
		for (int i = 0; i < (int) input_shape.size(); ++i)
			_output_shape *= input_shape[i];
		this->output_shape = vector<int>(1, _output_shape);

		assert(input_shape.size() == 3);
		printf("layer %s\n", this->name.c_str());
		printf("\tinput_shape = ");
		for (vector<int> :: const_iterator itr(input_shape.begin()); itr != input_shape.end(); ++itr)
			printf(" %d", *itr);
		printf("\n");

		printf("\toutput_shape = ");
		for (vector<int> :: const_iterator itr(output_shape.begin()); itr != output_shape.end(); ++itr)
			printf(" %d", *itr);
		printf("\n");
	}

	NDArray forward(const NDArray &input) {
		NDArray output;
		output.shape = output_shape;
		vector<int> pos(3, 0);
		for (int &i = pos[1] = 0; i < (int) input.shape[1]; ++i)	
			for (int &j = pos[2] = 0; j < (int) input.shape[2]; ++j)
				for (int &c = pos[0] = 0; c < (int) input.shape[0]; ++c)
					output.array.push_back(input.get(pos));
		return output;
	}
};

struct MatMul: public Layer {
	MatMul(const string &name, const vector<int> &input_shape, map<string, NDArray> &weights) {
		this->name = name + "/matmul";
		this->input_shape = input_shape;
		NDArray w(weights[name + "/w"]);
		this->output_shape = vector<int>(1, w.shape[1]);

		assert(input_shape.size() == 1);
		printf("layer %s\n", this->name.c_str());
		printf("\tinput_shape = ");
		for (vector<int> :: const_iterator itr(input_shape.begin()); itr != input_shape.end(); ++itr)
			printf(" %d", *itr);
		printf("\n");

		printf("\toutput_shape = ");
		for (vector<int> :: const_iterator itr(output_shape.begin()); itr != output_shape.end(); ++itr)
			printf(" %d", *itr);
		printf("\n");
	}
};



int input_shape[][3] = {
	{1, 28, 28},  //  0: conv1/Conv
	{32, 28, 28}, //  1: conv1/Bias
	{32, 28, 28}, //  2: conv1/ReLU

	{32, 28, 28}, //  3: pool1/Pool

	{32, 14, 14}, //  4: conv2/Conv
	{64, 14, 14}, //  5: conv2/Bias
	{64, 14, 14}, //  6: conv2/ReLU

	{64, 14, 14}, //  7: pool2/Pool

	{64, 7, 7},   //  8: Flatten
	{3136},       //  9: fc1/MatMul
	{512},        // 10: fc1/Bias
	{512},        // 11: fc1/ReLU

	{10},         // 12: fc2/MatMul
	{10},         // 13: fc2/Bias
	{10}          // 14: fc2/ReLU
};

int main() {
	map<string, NDArray> weights = load_pretrained_model("pretrained.txt");

	Layer conv1_conv = Conv("conv1", vector<int>(input_shape[0], input_shape[0] + 3), weights);
	Layer conv1_bias = Bias("conv1", vector<int>(input_shape[1], input_shape[1] + 3), weights);
	Layer conv1_relu = Bias("conv1", vector<int>(input_shape[2], input_shape[2] + 3), weights);
	Layer pool1      = Pool("pool1", vector<int>(input_shape[3], input_shape[3] + 3), weights);

	Layer conv2_conv = Conv("conv2", vector<int>(input_shape[4], input_shape[4] + 3), weights);
	Layer conv2_bias = Bias("conv2", vector<int>(input_shape[5], input_shape[5] + 3), weights);
	Layer conv2_relu = Bias("conv2", vector<int>(input_shape[6], input_shape[6] + 3), weights);
	Layer pool2      = Pool("pool2", vector<int>(input_shape[7], input_shape[7] + 3), weights);

	Layer flatten    = Flatten("flatten", vector<int>(input_shape[8], input_shape[8] + 3), weights);

	Layer fc1_matmul = MatMul("fc1", vector<int>(input_shape[9], input_shape[9] + 1), weights);
	Layer fc1_bias   = Bias("fc1", vector<int>(input_shape[10], input_shape[10] + 1), weights);

	Layer fc2_matmul = MatMul("fc2", vector<int>(input_shape[11], input_shape[11] + 1), weights);
	Layer fc2_bias   = Bias("fc2", vector<int>(input_shape[12], input_shape[12] + 1), weights);

	return 0;
}

