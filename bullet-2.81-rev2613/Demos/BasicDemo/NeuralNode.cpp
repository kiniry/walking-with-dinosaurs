#include "NeuralNode.h"


NeuralNode::NeuralNode(float value)
{
	this->nrOfInputs=0;
	this->currentOutput=value;
}

NeuralNode::NeuralNode(int function, NeuralNode* input, float weight)
{
	this->function=function;
	this->in1=input;
	this->weight1=weight;
	this->nrOfInputs=1;
}

NeuralNode::NeuralNode(int function, NeuralNode* input1, NeuralNode* input2, float weight1, float weight2)
{
	this->function=function;
	this->in1=input1;
	this->in2=input2;
	this->weight1=weight1;
	this->weight2=weight2;
	this->nrOfInputs=2;
}

NeuralNode::NeuralNode(int function, NeuralNode* input1, NeuralNode* input2, NeuralNode* input3, float weight1, float weight2, float weight3)
{
	this->function=function;
	this->in1=input1;
	this->in2=input2;
	this->in3=input3;
	this->weight1=weight1;
	this->weight2=weight2;
	this->weight3=weight3;
	this->nrOfInputs=3;
}

float NeuralNode::getOutput()
{
	return this->currentOutput;
}

void NeuralNode::compute()
{
	switch(function){
		case SUM:
			currentOutput=sum();
			break;
		case MULTIPLY:
			currentOutput=multiply();
			break;
		default:
			perror("Undefined neural function");
			break;
		}
}

float NeuralNode::sum(){
	if(nrOfInputs!=2){perror("Wrong number of inputs for function: SUM... erratic behavior expected");}
	return in1->currentOutput+in2->currentOutput;
}

float NeuralNode::multiply(){
	if(nrOfInputs!=2){perror("Wrong number of inputs for function: MULTIPLY... erratic behavior expected");}
	return in1->currentOutput*in2->currentOutput;
}

NeuralNode::~NeuralNode(void)
{
}
