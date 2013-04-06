
#include "Physics.h"
#include "Grammar.h"
#include "Evolution.h"
#include "GlutStuff.h"
#include "test.h"
#include <ctime>
	
int main(int argc,char** argv)
{
	//seeds random generator
	srand(time(0));

	#ifdef  _DEBUG
		test();

	#else
		

	#endif
	int populationSize = 100;
	int nrOfGenerations=10;


	std::vector<Physics*> worlds;
	 
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


	//mult. creatures test
	for(int i=0; i<populationSize;i++){
		creatures.push_back(creature());
		creatures.at(i).dna=ancestor;
	}
	//\mult. creatures test

	for(int i =0; i<populationSize; i++){
		//init world
		Physics* WWDPhysics = new Physics(creatures.at(i).dna);
	
		//init creature
		readDNA(creatures.at(i).dna,WWDPhysics);
		
		worlds.push_back(WWDPhysics);
	
	}



	for(int i=0;i<nrOfGenerations;i++){
		//start simulations - Todo: run for all creatures in population
		printf("%d\n", worlds.size());

		for(int j=0;j< (int) worlds.size();j++){
			worlds.at(j)->runSimulation(); //runs a physics simulation and save the fitness values
		}

		//print all unsorted
		/*for(int j=0;j< (int) worlds.size();j++){
			printf("nr %d %f\n",j,worlds.at(j)->getFitness());
		}*/

		for(int j= worlds.size()-1; j>=0; j--){
			creatures.at(j).fitness = worlds.at(j)->getFitness();
			delete worlds.at(j);
			worlds.pop_back();
		}


		//mutate
		creatures=evolve(creatures);

		//print survivors sorted
		for(int j=0;j< (int) (creatures.size()/5.f);j++){
			printf("nr %d %f\n",j,creatures.at(j).fitness);
		}

		
		for(int j =0; j<populationSize; j++){
			//init world
			Physics* WWDPhysics = new Physics(creatures.at(j).dna);
	
			//init creature
			readDNA(creatures.at(j).dna,WWDPhysics); //problem...
		
			worlds.push_back(WWDPhysics);
	
		}
		
		printf("round %d \n",i);
	}



	/*
	for(int i=0;i<populationSize;i++){
		for(int j=0;j<creatures.at(i).dna.size();j++){
			printf("%d ",creatures.at(i).dna.at(j));
		}

		printf("%f \n",worlds.at(i)->getFitness());
	}
	*/
		//Show end result if we want to...
	//default glut doesn't return from mainloop
	return glutmain(argc, argv,1024,600,"Walking with dinosaurs",worlds.at(0));

	
	
}

