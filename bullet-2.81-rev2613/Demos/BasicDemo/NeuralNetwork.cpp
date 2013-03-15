#include "NeuralNetwork.h"


NeuralNetwork::NeuralNetwork(std::vector<NeuralNode*> inputs)
{
	previousLayerOutputs = inputs;
	initialInputs=inputs;
	init=true;

	layerIndex=0;
}


NeuralNetwork::~NeuralNetwork(void)
{
}

void NeuralNetwork::insertNode(int f, int i1, float w1)
{
	NeuralNode* n = new NeuralNode(f,previousLayerOutputs.at(i1%previousLayerOutputs.size()),w1);
	if(init){initialLayer.push_back(n);}
	currentLayer.push_back(n);
}

void NeuralNetwork::insertNode(int f, int i1, int i2, float w1, float w2)
{
	int size = previousLayerOutputs.size();
	NeuralNode* n = new NeuralNode(f,previousLayerOutputs.at(i1%size),previousLayerOutputs.at(i2%size),w1,w2);
	if(init){initialLayer.push_back(n);}
	currentLayer.push_back(n);
}

void NeuralNetwork::insertNode(int f, int i1, int i2, int i3, float w1, float w2, float w3)
{
	int size = previousLayerOutputs.size();
	NeuralNode* n = new NeuralNode(f,previousLayerOutputs.at(i1%size),previousLayerOutputs.at(i2%size),previousLayerOutputs.at(i3%size),w1,w2,w3);
	if(init){initialLayer.push_back(n);}
	currentLayer.push_back(n);	
}

void NeuralNetwork::changeLayer()
{
	if(init){init=false;}
	previousLayerOutputs.clear();
	
	if(!layerIndex==lastLayer){
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
		layerIndex++;
	}
	else
	{
		previousLayerOutputs=initialInputs;
		currentLayer=initialLayer;
	}
}

void NeuralNetwork::stopBuilding()
{
	lastLayerIndex=layerIndex;
	lastLayer=currentLayer;
	changeLayer();
}

void NeuralNetwork::computeLayer()
{
	for(int i=0;i<currentLayer.size();i++){
		currentLayer.at(i)->compute();
	}
	changeLayer();
}
