
//#include "Physics.h"
#include "Grammar.h"
#include "GlutStuff.h"

	
int main(int argc,char** argv)
{

	Physics WWDPhysics;
	WWDPhysics.initPhysics();
	WWDPhysics.testPhysics();
	//insert random shite
	int dof = 0,S1=5,S2=4;
	//int string[] = {3,2,1,1,0,0,0,S1,0,0,S2,dof,0,0,3,2,1,0};

	//int string[] = {3,2,1,0};


	//readDNA(string,&WWDPhysics);

	//default glut doesn't return from mainloop
	return glutmain(argc, argv,1024,600,"Walking with dinosaurs",&WWDPhysics);
}

