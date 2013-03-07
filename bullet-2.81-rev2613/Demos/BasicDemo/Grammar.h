#ifndef GRAMMAR_H
#define GRAMMAR_H

#define NULL 0

enum{pressure, angel, light}

struct Joint {
	Point3 degreesOfFreedom;
	Point3 pre, post;
	int length;

};

struct Part {
	int height, width, depth;
	part*[5];
	Joint* joint;
	int sensor;
};

part* body;
#endif
