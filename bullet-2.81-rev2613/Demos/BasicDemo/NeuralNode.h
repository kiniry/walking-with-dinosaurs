#pragma once
#include <stdio.h>
#include <math.h>

//Definition of functions
enum{SUM,PRODUCT,DIVIDE,SUMTHRESHOLD,GREATERTHAN,SIGNOF,MIN,MAX,ABS,IF,INTERPOLATE,SIN,COS,ATAN,LOG,EXPT,SIGMOID};

class NeuralNode
{
	int function;
	int nrOfInputs;
	float currentOutput;
	NeuralNode* input1;
	NeuralNode* input2;
	NeuralNode* input3;
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
	inline float sumThreshold();
	inline float greaterThan();
	inline float signOf();
	inline float min();
	inline float max();
	inline float abs();
	inline float myIf();
	inline float interpolate();
	inline float mySin();
	inline float myCos();
	inline float myAtan();
	inline float myLog();
	inline float myExpt();
	inline float sigmoid();
};

