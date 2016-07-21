
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
    float *array;

    NDArray(): shape(), array(NULL) {
    }

    ~NDArray() {
        if (array != NULL)
            delete [] array;
    }
};

map<string, NDArray *> load_pretrained_model(const char *path) {
    map<string, NDArray *> result;
    ifstream fin(path);
    int n;
    fin >> n;
    for (int i = 0; i < n; ++i) {
        int d, tmp, e = 1;
        string name;
        fin >> name >> d;
        NDArray *array = new NDArray();
        for (int j = 0; j < d; ++j) {
            fin >> tmp;
            array->shape.push_back(tmp);
            e *= tmp;
        }
        array->array = new float[e];
        for (int j = 0; j < e; ++j) {
            fin >> array->array[j];
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
                P2 = Complex(P2.x + t.x, P2.y - t.y);
                unit = mul(unit, unit_p0);
            }
        }
    }
}
#undef mul


int main() {
	load_pretrained_model("pretrained.txt");
	return 0;
}

