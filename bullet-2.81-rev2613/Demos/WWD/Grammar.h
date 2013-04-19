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

#define Gsucces 0
#define Gfail -1

#define maxBlocks 30

#define toFloat(X) X/100.f

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

int B(int index, std::vector<int> *DNA, Physics *world, int *blocks, int part, std::vector<DinoTreeNode*>* tempNeural,DinoTreeNode* partTree);
int J(int index, std::vector<int> *DNA, Physics *world, int *blocks, int part1, std::vector<DinoTreeNode*>* tempNeural,DinoTreeNode* partTree);

int readDNA(std::vector<int> *DNA, Physics *world);

int NN(int index, std::vector<int> *DNA);
int NI(int index, std::vector<int> *DNA);

int NN(int index, std::vector<int> *DNA, NeuralNetwork* net,DinoTreeNode* partTree);
int NI(int index, std::vector<int> *DNA, NeuralNetwork* net);

#endif
