
//#include "Physics.h"
#include "Grammar.h"
#include "GlutStuff.h"

	
int main(int argc,char** argv)
{
	int populationSize = 1;
	std::vector<Physics*> creatures;
	//int ancestor[] = {3,2,1,1,0,0,0,5,0,0,4,45,0,0,3,2,1,0};
	
	const int ancestor[] = {
	//Body
		3,2,1,	//main box: h,w,d
		1,		//1 box attached
		1,/*Joint type*/		0,0,5,/*preXYS*/		0,0,4,/*postXYS*/	45,45,45,/*DofXYZ*/ //Create joint
		3,2,1,	//attached box: h,w,d
			//NN-Effector0-layer 0
			1,	2,10,0,1,100,100,	//2-in-node: f=interpolate, in0=0,in1=1,w0=100,w1=100
			0,	2,0,0,0,0,100,		//2-in-node: f=sum,in0=0,in1=1,w0=0,w1=100 (just sends in1 through unchanged) - No more nodes
		1,0,1,	//NN-Effector: use outputs O0=1,O1=0,O2=1
		0,		//no attached boxes
	//NN-main-layer 0	(sensors not implemented yet)
		1,	0,30,			//constant node 30
		1,	0,62,			//constant node 62
		2,	0,125,			//constant node 125 - change layer
	//NN-main-layer 1
		1,	2,0,0,1,50,23,	//2-in-node: f=sum in0=0 in1=1 w0=50 w1=23
		2,	1,12,2,5,		//1-in node: f=cos in0=2 w0=5 - change layer
	//NN-main-layer 2
		1,	2,1,0,1,1,200,	//2-in node: f=product in0=0 in1=1, w0=1 w1=200
		0,	2,0,0,1,1,1		//2-in node: f=sum in0=0 in1=1, w0=1 w1=1 - No more nodes
	};
	
	for(int i =0; i<populationSize; i++){
		//init world
		Physics* WWDPhysics = new Physics();
	
		//init creature
		//mutate(ancestor);
		readDNA(ancestor,WWDPhysics);
		
		//creatures.push_back(WWDPhysics);
	
	}


		//start simulations
		//creatures.at(0)->testPhysics();
		//default glut doesn't return from mainloop
		return glutmain(argc, argv,1024,600,"Walking with dinosaurs",creatures.at(0));

	
	
}

