/**
*	this files handels reading the dna (gneotype) and creating the phenotype in the given physicsworld
*
*	if the dna is to short it will add the default value 0 until a complete a complete genotype is reached
**/

#include "btBulletDynamicsCommon.h"
#include "Physics.h"

#ifndef GRAMMAR_H
#define GRAMMAR_H

#define NULL 0


#define toFloat(X) X/100.f

#define SUBNET_NET_WIDTH 5

/**
* checks the of the dna, and returns add 0 if the range is excided by one.
* 0 == default terminate value
*/
inline int getDNA(int x, std::vector<int> *dna){
	while(x >= (int) dna->size()){
		dna->push_back(0);
	}
		return dna->at(x);
};

int B(int index, std::vector<int> *DNA, Physics *world, int *blocks, int part, std::vector<int>* tempNeural);
int J(int index, std::vector<int> *DNA, Physics *world, int *blocks, int part1, std::vector<int>* tempNeural);

int readDNA(std::vector<int> *DNA, Physics *world);

//int NN(int index, std::vector<int> *DNA,partNode* body,NNLayerNode* layer);
int NI(int index, std::vector<int> *DNA);

//int NN(int index, std::vector<int> *DNA, NeuralNetwork* net, Physics *world, NNLayerNode* layer, bool notSubnet);
int NI(int index, std::vector<int> *DNA, NeuralNetwork* net);

int NNL(int index, int inputAmount, std::vector<int>* DNA, NeuralNetwork* aNet, NNLayerNode* node, bool isJustChecking);
//int NN(int index, std::vector<int> *DNA,partNode* body); //moved to MTree.h
int NN(int index, std::vector<int>* DNA, std::vector<NeuralNode*> inputs, Physics *world, bool isNotSubnet);

MTree* getMTree(std::vector<int> *DNA);
int treeB(int index, std::vector<int> *DNA, int *blocks, partNode* body);
int treeJ(int index, std::vector<int> *DNA, int *blocks, partNode* body);
int treeNN(int index, std::vector<int> *DNA,MTree* mainTree);

#endif
