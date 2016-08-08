
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <cassert>
#include <fstream>

#include "Context.h"
#include "sha256.h"

using namespace std;

void Context :: makePrediction(int predict) {
	string name = this->prefix + "/predict.txt";
	cout << name << endl;
	ofstream os(name.c_str());
	os << predict << endl;
	os.close();
}

void Context :: setFile(const string &name) {
	this->filename = name;
}

Context& Context::operator << (HashTree *rhs) {
	if (Context :: context.this_layer >= 0) {
		this->roots.push_back(rhs->t[ (rhs->length - 1) | (rhs->length != 1) ]);
		this->treeSize.push_back(rhs->length);
	}
	string name = this->filename + "_hashtree.txt";
	for (int i = 0; i < (int)name.size(); ++i)
		if (name[i] == '/')
			name[i] = '_';
	name = this->prefix + "/" + name;
	cout << name << endl;
	ofstream os(name.c_str());
	int length = (rhs->length << 1) - 1;
	for (int i = 0; i < length; ++i) {
		string result = prettyPrintSha256(rhs->t[i].hash);
		os << result << ' ';
	}
	os << endl;
	os.close();
	return *this;
}

void Context :: outputRoots () {
	string name = this->prefix + "/hashtree_roots.txt";
	cout << name << endl;
	ofstream os(name.c_str());
	for (int i = 0; i < (int)roots.size(); ++i) {
		string result = prettyPrintSha256(roots[i].hash);
		os << result << ' ';
	}
	os << endl;
	for (int i = 0; i < (int)roots.size(); ++i) {
		os << treeSize[i] << ' ';
	}
	os << endl;
	os.close();
}

Context& Context::operator << (const vector<FP> &rhs) {
	string name = this->filename + "_output.txt";
	for (int i = 0; i < (int)name.size(); ++i)
		if (name[i] == '/')
			name[i] = '_';
	name = this->prefix + "/" + name;
	cout << name << endl;
	ofstream os(name.c_str());
	for (int i = 0; i < (int) rhs.size(); ++i) {
		os << rhs[i].n << ' ';
	}
	os << endl;
	os.close();
	return *this;
}

Context& Context::operator << (const FP &rhs) {
	string name = this->filename + "_FP.txt";
	for (int i = 0; i < (int)name.size(); ++i)
		if (name[i] == '/')
			name[i] = '_';
	name = this->prefix + "/" + name;
	ofstream os(name.c_str());
	os << rhs.n << endl;
	os.close();
	return *this;
}

void Context :: parse(int argc, char **argv) {
	assert(argc >= 2);

	this->mode = 0;
	this->level = 0;
	this->position = -1;
	this->misbehave = false;

	cout << argv[1] << endl;
	if (strcmp(argv[1], "predict") == 0)
		this->mode = 0;
	else if (strcmp(argv[1], "interactive") == 0)
		this->mode = 1;
	else
		assert(0);

	for (int i = 0; i < argc; ++i)
		if (strcmp(argv[i], "--layer") == 0 && i + 1 < argc) {
			this->layer = atoi(argv[i + 1]);
		}
		else if (strcmp(argv[i], "--level") == 0 && i + 1 < argc) {
			this->level = atoi(argv[i + 1]);
		}
		else if (strcmp(argv[i], "--position") == 0 && i + 1 < argc) {
			this->position = atoi(argv[i + 1]);
		}
		else if (strcmp(argv[i], "--prefix") == 0 && i + 1 < argc) {
			this->prefix = string(argv[i + 1]);
		}
		else if (strcmp(argv[i], "--misbehave") == 0) {
			this->misbehave = true;
		}
}

