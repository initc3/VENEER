#ifndef __CONTEXT_H
#define __CONTEXT_H

#include <cstring>
#include <string>
#include <vector>
#include "FP.h"
#include "HashTree.h"

struct Context {

	std :: vector<Node> roots;
	std :: vector<int> treeSize;
//	std :: vector<std :: vector<FP> > fps;

	int mode; // 0 : predict ; 1 : interactive
	int layer;
	int level;
	int position;
	int this_layer;
	std :: string filename, prefix;
	bool misbehave;

	void parse(int argc, char **argv);

	Context& operator << (HashTree *rhs);
	Context& operator << (const std :: vector<FP> &rhs);
	Context& operator << (const FP &rhs);

	
	void setFile(const std :: string &name);
	void outputRoots();
	void makePrediction(int predict);

	static Context context;

private:
	Context() {}
};

#endif

