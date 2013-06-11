#include "NNodeTest.h"

namespace NNode{
	void construct1(){
		float val = 2.43;
		NeuralNode* node = new NeuralNode(val);

		assertFloat(node->getOutput(), val, 0.00001);

		node->compute();

		assertFloat(node->getOutput(), val, 0.00001);

		delete node;
	}

	void construct2(){
		float* val = (float*) malloc(sizeof(float));
		*val=5.53;

		NeuralNode* node = new NeuralNode(val);

		assertFloat(node->getOutput(), *val, 0.00001);

		node->compute();

		assertFloat(node->getOutput(), *val, 0.00001);

		*val=42;

		assertFloat(node->getOutput(), 5.53, 0.00001);

		node->compute();

		assertFloat(node->getOutput(), *val, 0.00001);

		delete  node;
	}

	void functionsAndWeights(){
	}
	void all(){
		construct1();

		construct2();
	}
}