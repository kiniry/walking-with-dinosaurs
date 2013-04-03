#include "btBulletDynamicsCommon.h"
#include "Physics.h"

#ifndef GRAMMAR_H
#define GRAMMAR_H

#define NULL 0

#define Gsucces 0
#define Gfail -1

#define maxHeight 10
#define maxWidth 10
#define maxDepth 10
#define maxBlocks 30

#define toFloat(X) X/100.f



int B(int index, const std::vector<int> DNA, Physics *world, int *blocks, int part, std::vector<DinoTreeNode*>* tempNeural,DinoTreeNode* partTree);
int J(int index, const std::vector<int>  DNA, Physics *world, int *blocks, int part1, std::vector<DinoTreeNode*>* tempNeural,DinoTreeNode* partTree);

int readDNA(const std::vector<int> DNA, Physics *world);

int NN(int index,const std::vector<int>  DNA);
int NI(int index,const std::vector<int>  DNA);
//int II(int index);
//int I(int index);


int NN(int index, const std::vector<int> DNA, NeuralNetwork* net,DinoTreeNode* partTree);
int NI(int index, const std::vector<int> DNA, NeuralNetwork* net);
//int II(int index, const int* DNA, Physics *world);
//int I(int index, const int* DNA, Physics *world);

#endif
