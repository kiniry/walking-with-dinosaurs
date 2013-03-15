#pragma once
#include "NeuralNode.h"
#include <vector>
class NeuralNetwork
{
	std::vector<NeuralNode*> initialLayer;
	std::vector<NeuralNode*> previousLayerOutputs;
	bool init;

public:
	std::vector<NeuralNode*> currentLayer;

	NeuralNetwork(std::vector<NeuralNode*> inputs);
	~NeuralNetwork(void);
	
	void insertNode(int f, int i1, int w1);
	void insertNode(int f, int i1, int i2, int w1, int w2);
	void insertNode(int f, int i1, int i2, int i3, int w1, int w2, int w3);
	void changeLayer();
};

