
//#include "Physics.h"
#include "Grammar.h"
#include "Evolution.h"
#include "GlutStuff.h"
#include "test.h"

	
int main(int argc,char** argv)
{

	#ifdef  _DEBUG
		test();

	#else
		

	#endif


	int populationSize = 1;
	int nrOfGenerations=5; //temp var... todo:replace
	



	std::vector<Physics*> worlds;
	//int ancestor[] = {3,2,1,1,0,0,0,5,0,0,4,45,0,0,3,2,1,0};
	 
	const int temp[] = {
	//Body
		3,2,1,	//main box: h,w,d
		1,		//1 box attached
		1,/*Joint type*/		50,50,5,/*preXYS*/		50,50,4,/*postXYS*/	45,45,45,/*DofXYZ*/ //Create joint
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
	int size = sizeof( temp ) / sizeof ( *temp );
	std::vector<int> ancestor (temp, temp+size);
	
	std::vector<creature> creatures;
	creatures.push_back(creature());
	creatures.at(0).dna=ancestor;

	for(int i =0; i<populationSize; i++){
		//init world
		Physics* WWDPhysics = new Physics(creatures.at(i).dna);
	
		//init creature
		readDNA(creatures.at(i).dna,WWDPhysics);
		
		worlds.push_back(WWDPhysics);
	
	}



	for(int i=0;i<nrOfGenerations;i++){
		//start simulations - Todo: run for all creatures in population
		for(int j=0;j< (int) worlds.size();j++){
			worlds.at(j)->runSimulation(); //this should run a physics simulation and save the fitness values
		}
		//Todo: mutate/breed population to a new population

		while(worlds.size()>0){
			//todo get fitness
			creatures.at(worlds.size()-1).fitness = worlds.at(worlds.size()-1)->getFitness();
			delete worlds.at(worlds.size()-1);
			worlds.pop_back();
		}

		//Todo mutate
		creatures=evolve(creatures);

		
		for(int j =0; j<populationSize; j++){
			//init world
			Physics* WWDPhysics = new Physics(creatures.at(j).dna);
	
			//init creature
			readDNA(creatures.at(j).dna,WWDPhysics);
		
			worlds.push_back(WWDPhysics);
	
		}
		
		printf("round %d \n",i);
	}



	//Show end result if we want to...
	//default glut doesn't return from mainloop
	return glutmain(argc, argv,1024,600,"Walking with dinosaurs",worlds.at(0));

	
	
}

