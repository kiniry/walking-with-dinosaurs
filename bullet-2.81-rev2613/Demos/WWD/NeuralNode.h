#pragma once
#include <stdio.h>
#include <math.h>
#include <limits>



//Definition of functions
#define SENSOR -1
enum{SUM,PRODUCT,DIVIDE,SUMTHRESHOLD,GREATERTHAN,SIGNOF,MIN,MAX,ABS,IF,INTERPOLATE,SIN,COS,ATAN,LOG,EXPT,SIGMOID,WAVE,SAW,NR_OF_FUNCTIONS};

class NeuralNode
{
	
	int nrOfInputs;
	float currentOutput;
	float* sensorInput;
	NeuralNode* input1;
	NeuralNode* input2;
	NeuralNode* input3;
	float weight1,weight2,weight3;
	long stepsRun;

public:
	int function;
	NeuralNode(float* pointer);
	NeuralNode(float value);
	NeuralNode(int function, NeuralNode* input, float weight);
	NeuralNode(int function, NeuralNode* input1, NeuralNode* input2, float weight1, float weight2);
	NeuralNode(int function, NeuralNode* input1, NeuralNode* input2, NeuralNode* input3, float weight1, float weight2, float weight3);
	~NeuralNode(void);
	float getOutput();
	void compute();
	void setWeights(float w1, float w2, float w3);

private:
	inline float sum();
	inline float product();
	inline float divide();
	inline float sumThreshold();
	inline float greaterThan();
	inline float signOf();
	inline float myMin();
	inline float myMax();
	inline float abs();
	inline float myIf();
	inline float interpolate();
	inline float mySin();
	inline float myCos();
	inline float myAtan();
	inline float myLog();
	inline float myExpt();
	inline float sigmoid();
	inline float wave();
	inline float saw();
};

