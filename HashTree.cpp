
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cassert>
#include <algorithm>

#include "FP.h"
#include "Complex.h"
#include "sha256.h"
#include "HashTree.h"

using namespace std;

long long HashTree :: allocated = 0;

HashTree :: HashTree(Node *t, int length): t(t), length(length) {
}

HashTree :: HashTree(FP input[], int length): t(new Node[length * 2 - 1]), length(length) {
	for (int i = 0; i < length; ++i)
		toSHA256(input[i], t[i + i].hash);
	build(0, length - 1);
#ifdef MEASURE_COMPUTATION
	allocated += length * 2 - 1;
#endif
}

HashTree :: HashTree(const vector<FP> &input): t(new Node[input.size() * 2 - 1]), length(input.size()) {
	for (int i = 0; i < length; ++i)
		toSHA256(input[i], t[i + i].hash);
	build(0, length - 1);
#ifdef MEASURE_COMPUTATION
	allocated += length * 2 - 1;
#endif
}

HashTree :: HashTree(Complex input[], int length): t(new Node[length * 2 - 1]), length(length) {
	for (int i = 0; i < length; ++i)
		toSHA256(input[i], t[i + i].hash);
	build(0, length - 1);
#ifdef MEASURE_COMPUTATION
	allocated += length * 2 - 1;
#endif
}

HashTree :: ~HashTree() {
	delete [] t;
}

int HashTree :: build(int l, int r) {
	int id = (l + r) | (l != r);
	if (l == r)
		assert(id % 2 == 0);
	if (l < r) {
		int m( (l + r) >> 1 );
		toSHA256(t[build(l, m)].hash, t[build(m + 1, r)].hash, t[id].hash);
	}
	return id;
}

