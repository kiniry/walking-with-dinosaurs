#include "NeuralNetwork.h"


NeuralNetwork::NeuralNetwork(std::vector<NeuralNode*> inputs)
{
	previousLayerOutputs = inputs;
	//initialLayer.push_back(n);
	//currentLayer.push_back(n);
	init=true;
}


NeuralNetwork::~NeuralNetwork(void)
{
}

void NeuralNetwork::insertNode(int f, int i1, int w1)
{
	NeuralNode* n = new NeuralNode(f,previousLayerOutputs.at(i1),w1);
	if(init){initialLayer.push_back(n);}
	currentLayer.push_back(n);
}

void NeuralNetwork::insertNode(int f, int i1, int i2, int w1, int w2)
{
	NeuralNode* n = new NeuralNode(f,previousLayerOutputs.at(i1),previousLayerOutputs.at(i2),w1,w2);
	if(init){initialLayer.push_back(n);}
	currentLayer.push_back(n);
}

void NeuralNetwork::insertNode(int f, int i1, int i2, int i3, int w1, int w2, int w3)
{
	NeuralNode* n = new NeuralNode(f,previousLayerOutputs.at(i1),previousLayerOutputs.at(i2),previousLayerOutputs.at(i3),w1,w2,w3);
	if(init){initialLayer.push_back(n);}
	currentLayer.push_back(n);	
}

void NeuralNetwork::changeLayer()
{
	if(init){init=false;}
	previousLayerOutputs.clear();
	std::vector<NeuralNode*> flipVariable;
	
	//we have to flip the vector to maintain the ordering of nodes...
	while(!currentLayer.empty()){
		flipVariable.push_back(currentLayer.back());
		currentLayer.pop_back();
	}
	while(!flipVariable.empty()){
		previousLayerOutputs.push_back(flipVariable.back());
		flipVariable.pop_back();
	}


}
