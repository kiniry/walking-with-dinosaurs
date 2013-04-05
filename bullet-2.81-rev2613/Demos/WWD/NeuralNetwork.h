#pragma once
#include "NeuralNode.h"
#include <vector>

#define MAX_LAYERS 10000
class NeuralNetwork
{
	/*
	std::vector<NeuralNode*> initialLayer;
	std::vector<NeuralNode*> initialInputs;
	std::vector<NeuralNode*> previousLayerOutputs;
	*/
	std::vector<std::vector<NeuralNode*>> layers;
	
	bool init,outputUndefined;

	int layerIndex;
	int lastLayerIndex;

public:
	//std::vector<NeuralNode*> currentLayer;
	//std::vector<NeuralNode*> lastLayer;

	NeuralNetwork(std::vector<NeuralNode*> inputs); //should not be public?
	std::vector<NeuralNode*> getLastLayer();
	~NeuralNetwork(void);
	
	void insertNode(float value);
	void insertNode(int f, int i1, float w1);
	void insertNode(int f, int i1, int i2, float w1, float w2);
	void insertNode(int f, int i1, int i2, int i3, float w1, float w2, float w3);
	void changeLayer();
	void stopBuilding();
	void computeLayer();
	void computeNetwork();
	void killFirstLayer();

	float getOutput(int index);
};

