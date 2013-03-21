#include "NNodeTest.h"

namespace NNode{
void construct1(){

	float val = 2.43;
	NeuralNode* node = new NeuralNode(val);

	assert2(node->getOutput(), val, 0.00001);

	node->compute();

	assert2(node->getOutput(), val, 0.00001);
}


void construct2(){

	float* val = (float*) malloc(sizeof(float));
	*val=5.53;
	
	NeuralNode* node = new NeuralNode(val);

	assert2(node->getOutput(), *val, 0.00001);

	node->compute();

	assert2(node->getOutput(), *val, 0.00001);

	*val=42;

	assert2(node->getOutput(), 5.53, 0.00001);

	node->compute();

	assert2(node->getOutput(), *val, 0.00001);

}

 void all(){

	 construct1();

	 construct2();
		

 }
}