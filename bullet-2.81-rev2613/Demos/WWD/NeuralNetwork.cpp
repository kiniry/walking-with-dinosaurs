#include "NeuralNetwork.h"


NeuralNetwork::NeuralNetwork(std::vector<NeuralNode*> inputs)
{
	lastLayerIndex=MAX_LAYERS;
	//previousLayerOutputs = inputs;
	//initialInputs=inputs;
	layers.push_back(inputs);
	init=true;outputUndefined=true;
	std::vector<NeuralNode*> aLayer;
	layers.push_back(aLayer);
	layerIndex=1;
}


NeuralNetwork::~NeuralNetwork(void)
{
	while(layers.size()>0){
		std::vector<NeuralNode*> L = layers.at(layers.size()-1);
		while(L.size()>0){
			delete L.at(layers.size()-1);
			L.pop_back();
		}
		layers.pop_back();
	}
}

void NeuralNetwork::insertNode(float value)
{
//1: get value and create node from it
	NeuralNode* n = new NeuralNode(value);
//2: insert created node in current layers
	layers.at(layerIndex).push_back(n);
}

void NeuralNetwork::insertNode(int f, int i1, float w1)
{
//1: get input node from previous layer and create node from it
	NeuralNode* n = new NeuralNode(f,layers.at(layerIndex-1).at(i1%layers.at(layerIndex-1).size()),w1);
//2: insert created node in current layers
	layers.at(layerIndex).push_back(n);
}

void NeuralNetwork::insertNode(int f, int i1, int i2, float w1, float w2)
{
//1: get input nodes from previous layer and create node from it	
	int size = layers.at(layerIndex-1).size();
	NeuralNode* inputNode1 = layers.at(layerIndex-1).at(i1%size);
	NeuralNode* inputNode2 = layers.at(layerIndex-1).at(i2%size);
	NeuralNode* n = new NeuralNode(f,inputNode1,inputNode2,w1,w2);
//2: insert created node in current layers
	layers.at(layerIndex).push_back(n);
}

void NeuralNetwork::insertNode(int f, int i1, int i2, int i3, float w1, float w2, float w3)
{
//1: get input nodes from previous layer and create node from it	
	int size = layers.at(layerIndex-1).size();
	NeuralNode* inputNode1 = layers.at(layerIndex-1).at(i1%size);
	NeuralNode* inputNode2 = layers.at(layerIndex-1).at(i2%size);
	NeuralNode* inputNode3 = layers.at(layerIndex-1).at(i3%size);
	NeuralNode* n = new NeuralNode(f,inputNode1,inputNode2,inputNode3,w1,w2,w3);
//2: insert created node in current layers
	layers.at(layerIndex).push_back(n);
}

void NeuralNetwork::changeLayer()
{
	
	//previousLayerOutputs.clear();
	if(!(layerIndex==lastLayerIndex)){
		/*std::vector<NeuralNode*> flipVariable;

		//we have to flip the vector to maintain the ordering of nodes...
		while(!currentLayer.empty()){
			flipVariable.push_back(currentLayer.back());
			currentLayer.pop_back();
		}
		while(!flipVariable.empty()){
			previousLayerOutputs.push_back(flipVariable.back());
			flipVariable.pop_back();
		}*/

		//TODO set currentlayer...


		layerIndex++;
		if(init){
			std::vector<NeuralNode*> aLayer;
			layers.push_back(aLayer);
		}
	}
	else
	{
		//previousLayerOutputs=initialInputs;
		//currentLayer=initialLayer;
		layerIndex=0;
	}
}

void NeuralNetwork::stopBuilding()
{
	lastLayerIndex=layerIndex;
	init=false;
	//lastLayer=currentLayer;
	//printf("LLI %d \n",lastLayerIndex);
	changeLayer();
	//printf("LI %d \n",layerIndex);
}

void NeuralNetwork::computeLayer()
{
	//printf("computing layer: %d  size =%d \n",layerIndex,layers.at(layerIndex).size());
	for(int i=0;i<(int)layers.at(layerIndex).size();i++){
		//printf("atENUM %d \n",layers.at(layerIndex).at(i)->function);
		layers.at(layerIndex).at(i)->compute();
	}
	
	if(layerIndex==lastLayerIndex){outputUndefined=false;}

	changeLayer();
}

void NeuralNetwork::computeNetwork()
{
	for(int j=0;j<(int)layers.size();j++){
		for(int i=0;i<(int)layers.at(j).size();i++){
			layers.at(j).at(i)->compute();
		}
	}
}

std::vector<NeuralNode*> NeuralNetwork::getLastLayer(){
	return layers.at(lastLayerIndex);
}

float NeuralNetwork::getOutput(int index){
	//printf("size %d \n",lastLayer.size());
	/*if(index<lastLayer.size()){
		return lastLayer.at(index)->getOutput();
	}
	else{
		//no defined output in network. Defaulting...
		return 0;
	}*/
	if(outputUndefined){return 0;}
	
	return layers.at(lastLayerIndex).at(index%layers.at(lastLayerIndex).size())->getOutput();
}
