
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <sstream>

#include "Model.h"
#include "Layers.h"
#include "NDArray.h"

using namespace std;

Model :: Model(const vector<pair<string, string> > &layers, vector<int> input_shape, map<string, NDArray> &weights) {

	for (int i = 0; i < (int) layers.size(); ++i) {
		string type = layers[i].first;
		string name = layers[i].second;

		Layer *layer;
		if (type == "conv") {
			layer = new Conv(name, input_shape, weights);
		}
		else if (type == "bias") {
			layer = new Bias(name, input_shape, weights);
		}
		else if (type == "relu") {
			layer = new ReLU(name, input_shape);
		}
		else if (type == "pool") {
			layer = new Pool(name, input_shape);
		}
		else if (type == "flatten") {
			layer = new Flatten(name, input_shape);
		}
		else if (type == "matmul") {
			layer = new MatMul(name, input_shape, weights);
		}
		this->layers.push_back(layer);
		input_shape = layer->output_shape;
	}
}

Model :: ~Model() {
	for (int i = 0; i < (int) layers.size(); ++i)
		delete layers[i];
}

int Model :: predict(NDArray feed) {
	Context :: context.roots.clear();
	Context :: context.treeSize.clear();

	if (Context :: context.level == 0) {
		Context :: context.this_layer = -1;
		Context :: context.setFile(string("Layer_data_input_-1"));
		Context :: context << new HashTree(feed.array) << feed.array;
	}

	for (int i = 0; i < (int) layers.size(); ++i) {
		ostringstream ss;
		ss << "Layer_" << layers[i]->name << "_" << i;
		Context :: context.setFile(ss.str());
//		ss.close();
		Context :: context.this_layer = i;
		feed = layers[i]->forward(feed);
	}
	if (Context :: context.mode == 0)
		Context :: context.outputRoots();
	int argmax = -1;
	FP max = FP :: from(-1e10);
	for (int i = 0; i < (int) feed.array.size(); ++i) {
		FP value = feed.array[i];
		if (max < value) {
			max = value;
			argmax = i;
		}
	}
	return argmax;
}

