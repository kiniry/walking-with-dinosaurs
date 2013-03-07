#ifndef GRAMMAR_H
#define GRAMMAR_H

#define NULL 0

enum{pressure, angel, light};
struct Point3{
	int x,y,z;
};

struct Joint{
	Point3 degreesOfFreedom;
	Point3 pre;
	Point3 post;
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
