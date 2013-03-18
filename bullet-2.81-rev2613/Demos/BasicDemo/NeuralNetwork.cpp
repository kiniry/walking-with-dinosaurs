#include "NeuralNetwork.h"


NeuralNetwork::NeuralNetwork(std::vector<NeuralNode*> inputs)
{
	lastLayerIndex=100000;
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
	//printf("insert i1:%d i2:%d prev_size:%d\n",i1,i2,size);
	//printf("insert prev size %d\n",i1%size);
	//printf("insert prev size %d\n",i2%size);
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
	//failing... trashes all other layers than initialLayer

	//printf("current layer %d \n",layerIndex);
	
	if(init){init=false;}
	previousLayerOutputs.clear();
	//printf("LLI %d \n",lastLayerIndex);
	//printf("LI %d \n",layerIndex);
	if(!(layerIndex==lastLayerIndex)){
		//printf("inside\n");
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
		//printf("inside LI %d \n",layerIndex);
	}
	else
	{
		previousLayerOutputs=initialInputs;
		currentLayer=initialLayer;
		layerIndex=0;
	}
}

void NeuralNetwork::stopBuilding()
{
	lastLayerIndex=layerIndex;
	lastLayer=currentLayer;
	printf("LLI %d \n",lastLayerIndex);
	changeLayer();
	printf("LI %d \n",layerIndex);
}

void NeuralNetwork::computeLayer()
{
	printf("computing layer: %d  size =%d \n",layerIndex,currentLayer.size());
	for(int i=0;i<currentLayer.size();i++){
		printf("atENUM %d \n",currentLayer.at(i)->function);
		currentLayer.at(i)->compute();
	}
	changeLayer();
}

float NeuralNetwork::getOutput(int index){
	//printf("size %d \n",lastLayer.size());
	if(index<lastLayer.size()){
		return lastLayer.at(index)->getOutput();
	}
	else{
		//no defined output in network. Defaulting...
		return 0;
	}
}
