#ifndef __HASHTREE_H
#define __HASHTREE_H

#include <vector>
#include "FP.h"
#include "Complex.h"
#include "sha256.h"

struct Node {
	unsigned char hash[SHA256 :: DIGEST_SIZE];
};

struct HashTree {
	Node *t; // TODO: did not deallocate
	int length;

	HashTree(Node *t, int length);
	HashTree(FP input[], int length);
	HashTree(Complex input[], int length);
	HashTree(const std :: vector<FP> &input);
	~HashTree();

	static long long allocated;

private: // copy is not allowed
	HashTree(const HashTree &rhs);
	HashTree operator = (const HashTree &rhs);
	int build(int l, int r);

};

#endif

