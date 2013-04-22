#pragma once
#include <vector>
//#include "MTreeNode.h"
#include "partNode.h"
#include "NNNode.h"
class MTree{
private:
	partNode* bodyPart;
	std::vector<NNLayerNode*>* NNPart;
public:
	MTree(){
		NNPart = new std::vector<NNLayerNode*>();
	}
	std::vector<int> mutateDNA(std::vector<int> DNA);
	void setBody(partNode* body){bodyPart=body;}
	void addNeuralNetworkLayer(NNLayerNode* NN){NNPart->push_back(NN);}
};