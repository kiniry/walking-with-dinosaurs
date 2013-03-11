
//#include "Physics.h"
#include "Grammar.h"
#include "GlutStuff.h"
#include "btBulletDynamicsCommon.h"
#include "LinearMath/btHashMap.h"


	
int main(int argc,char** argv)
{

	Physics WWDPhysics;
	WWDPhysics.initPhysics();

	//insert random shite
	int dof = 0,S1=5,S2=4;
	int string[] = {3,2,1,1,0,0,0,S1,0,0,S2,dof,0,0,3,2,1,0};
	//int box = WWDPhysics.createBox(3,1,1);
	//WWDPhysics.createSensor(box, pressure);
	//int string[] = {3,2,1,0};
	readDNA(string,&WWDPhysics);
	
#ifdef CHECK_MEMORY_LEAKS
	ccdDemo.exitPhysics();
#else
	return glutmain(argc, argv,1024,600,"Walking with dinosaurs",&WWDPhysics);
#endif
	
	//default glut doesn't return from mainloop
	return 0;
}

