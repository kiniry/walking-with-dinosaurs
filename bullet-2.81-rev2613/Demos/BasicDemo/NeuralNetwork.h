#pragma once
#include "NeuralNode.h"
#include <vector>
class NeuralNetwork
{
	std::vector<NeuralNode*> initialLayer;
	std::vector<NeuralNode*> initialInputs;
	std::vector<NeuralNode*> previousLayerOutputs;
	bool init;

	//debug var's?
	int layerIndex;
	int lastLayerIndex;
	//\debug var's?

public:
	std::vector<NeuralNode*> currentLayer;
	std::vector<NeuralNode*> lastLayer;

	NeuralNetwork(std::vector<NeuralNode*> inputs);
	~NeuralNetwork(void);
	
	void insertNode(int f, int i1, float w1);
	void insertNode(int f, int i1, int i2, float w1, float w2);
	void insertNode(int f, int i1, int i2, int i3, float w1, float w2, float w3);
	void changeLayer();
	void stopBuilding();
	void computeLayer();
};

