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

struct Joint{
	btVector3 degreesOfFreedom;
	//pre/post vec = x,y,side
	btVector3 pre;
	btVector3 post;
	int length;

};

struct Part2 {
	int id;
	int height, width, depth;
	Part2  *parts;
	Joint* joint;
	int sensor;
};
//Part2 *body;

int B(int index, const int* DNA, Physics *world, int *blocks, int part);
int J(int index, const int* DNA, Physics *world, int *blocks, int part1);
int NN(int index, const int* DNA, Physics *world);
int readDNA(const int* DNA, Physics *world);

#endif
