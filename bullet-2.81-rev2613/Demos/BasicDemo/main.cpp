
//#include "Physics.h"
#include "Grammar.h"
#include "GlutStuff.h"

	
int main(int argc,char** argv)
{

	Physics WWDPhysics;
	WWDPhysics.initPhysics();

	//insert random shite
	int dof = 0,S1=5,S2=4;
	int string[] = {3,2,1,1,0,0,0,S1,0,0,S2,dof,0,0,3,2,1,0};
	int box = WWDPhysics.createBox(3,3,1);
	int box2 = WWDPhysics.createBox(2,2,1);
	int box3 = WWDPhysics.createBox(2,2,1);
	WWDPhysics.createJoint(box, box2, GENERIC6DOF,0.5, 0.5, 1, 0.5, 0.5, 1, 45,45,0);
	WWDPhysics.createJoint(box, box3, GENERIC6DOF,0.5, 0.5, 5, 0.5, 0.5, 1, 45,45,0);
	//WWDPhysics.createSensor(box, pressure);
	//int string[] = {3,2,1,0};


	//readDNA(string,&WWDPhysics);

	//default glut doesn't return from mainloop
	return glutmain(argc, argv,1024,600,"Walking with dinosaurs",&WWDPhysics);
}

