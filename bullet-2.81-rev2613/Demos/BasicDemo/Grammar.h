#ifndef GRAMMAR_H
#define GRAMMAR_H

#define NULL 0

#include "btBulletDynamicsCommon.h"

enum{pressure, angel, light};

struct Joint{
	btVector3 degreesOfFreedom;
	//pre/post vec = x,y,side
	btVector3 pre;
	btVector3 post;
	int length;

};

struct Part {
	int height, width, depth;
	Part  *parts;
	Joint* joint;
	int sensor;
};
Part *body;

#endif
