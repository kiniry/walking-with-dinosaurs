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
	std::vector<int> crossBreed(std::vector<int> myDNA, std::vector<int> seedDNA, MTree* seed);
	std::vector<int> NNcrossBreed(std::vector<int> myDNA, std::vector<int> seedDNA, std::vector<NNLayerNode*>* myNode, std::vector<NNLayerNode*>* seedNode);
};