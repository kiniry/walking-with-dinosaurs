#pragma once
#include <stdio.h>

//Definition of functions
enum{SUM,PRODUCT,DIVIDE};

class NeuralNode
{
	int function;
	int nrOfInputs;
	float currentOutput;
	NeuralNode* in1;
	NeuralNode* in2;
	NeuralNode* in3;
	float weight1,weight2,weight3;

public:
	NeuralNode(float value);
	NeuralNode(int function, NeuralNode* input, float weight);
	NeuralNode(int function, NeuralNode* input1, NeuralNode* input2, float weight1, float weight2);
	NeuralNode(int function, NeuralNode* input1, NeuralNode* input2, NeuralNode* input3, float weight1, float weight2, float weight3);
	~NeuralNode(void);
	float getOutput();
	void compute();

private:
	inline float sum();
	inline float product();
	inline float divide();
};

