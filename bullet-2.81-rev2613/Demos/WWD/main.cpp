#include "main.h"

int main(int argc,char** argv)
{
	//seeds random generator
	srand(time(0));

	//threads
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );

	numCores= sysinfo.dwNumberOfProcessors;
	//numCores=1;
	printf("cores %d\n", numCores);

#ifdef  _DEBUG
	
	return debug(argc,argv);
	return WWD(argc,argv);
#else
	const int temp[] = {1387,38,23,2,1924};
	int size = sizeof( temp ) / sizeof ( *temp );
	std::vector<int> ancestor (temp, temp+size);
	//return pipeServerMain(numCores,populationSize,nrOfGenerations,ancestor);
	return pipeClientMain(argc,argv);
	//return WWD(argc,argv);

#endif

	return 0;
}

//dosnt work
//simulation loop changed
unsigned int __stdcall threadSim(void* data){
	//printf("%d ",(int) data);
	for(int j=(int) data;j< (int) worlds.size();j+=numCores){
		worlds.at(j)->runSimulation(); //runs a physics simulation and save the fitness values
	}

	return 0;
}

int debug(int argc,char** argv){
	test();
	const int temp[] = {0, 7306, 77, 9859, 8996, 0, 7615, 0, 0, 1641, 8007, 0, 1666, 1098, 3253, 0, 5908, 6549, 7241, 8392, 2533, 8392, 7810, 150, 0, 8248, 1100, 0, 0, 0, 300, 350, 300
, 2324, 4627, 1393, 457, 4736, 0, 819, 0, 289, 0, 0, 2872, 5034, 1, 539, 2160, 3399, 9902, 1094, 6143, 100, 0, 4660, 5670, 400, 1214, 0, 5202, 0, 6629, 5316, 0,
 8392, 7810, 1519, 4743, 0, 513, 3589, 1790, 25, 1433, 5552, 0, 8695, 3, 9352, 1413, 9324, 1, 25, 3278, 0, 0, 0, 6020, 0, 1071, 2400, 3678, 1641, 8007, 0, 4650,
 1098, 1801, 1211, 2465, 6549, 6435, 1471, 3032, 2598, 100, 3005, 9326, 150, 1,25, 3278, 0, 4710, 9276, 3266, 4386, 0, 0, 7340, 0, 0, 3559, 9407, 0, 4710, 6038
, 3187, 9961, 7280, 1460, 7224, 7744, 200, 5723, 2289, 0, 3948, 9274, 0, 573, 0, 8815, 3264, 0, 3809, 0, 0, 0};
	int size = sizeof( temp ) / sizeof ( *temp );
	std::vector<int> ancestor (temp, temp+size);
	Physics* WWDPhysics = new Physics();
	//init creature
	readDNA(&ancestor,WWDPhysics);
	//WWDPhysics->testPhysics();
	//default glut doesn't return from mainloop
	WWDPhysics->solveGroundConflicts();
	return glutmain(argc, argv,1024,600,"Walking with dinosaurs",WWDPhysics);
}

int WWD(int argc,char** argv){

	HANDLE *handles =new HANDLE[numCores];

	const int temp[] = {
		//Body
		295,195,95,	//main box: h,w,d
		1,		//1 box attached
		1,/*Joint type*/		50,50,5,/*preXYS*/		50,50,4,/*postXYS*/	45,45,45,/*DofXYZ*/ //Create joint
		295,195,95,	//attached box: h,w,d
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

	std::vector<creature>* creatures = new std::vector<creature>();

	//Creates one ancestor and the rest is mutation of the ancestor
	creatures->push_back(creature());
	creatures->at(0).dna=ancestor;
	creatures->at(0).fitness=0;
	for(int i=1; i<populationSize;i++){
		creatures->push_back(creature());
		creatures->at(i).dna=mutate(ancestor,2);
		creatures->at(i).fitness=0;
	}

	for(int i=0;i<nrOfGenerations;i++){
		//#pragma omp parallel
		{
			//run simulator
			//#pragma omp for schedule(dynamic)
			for(int j =0; j<populationSize; j++){
				//init world
				Physics* WWDPhysics = new Physics();

				//init creature
				readDNA(&creatures->at(j).dna,WWDPhysics);

				//run sim
				WWDPhysics->runSimulation();
				creatures->at(j).fitness = WWDPhysics->getFitness();
				creatures->at(j).treePointer = getMTree(&creatures->at(j).dna);
				delete WWDPhysics;
			}

			/*
			//threads
			for(int j =0; j<numCores; j++){
			handles[j] = (HANDLE)_beginthreadex(0, 0, &threadSim,(void*) j, 0, 0);
			}

			WaitForMultipleObjects(numCores, handles, true,INFINITE);

			for(int j =0; j<numCores; j++){
			CloseHandle(handles[j]);
			} */

			//print all unsorted
			/*for(int j=0;j< (int) worlds.size();j++){
			printf("nr %d %f\n",j,worlds.at(j)->getFitness());
			}*/

			//mutate
			//#pragma omp single nowait
			evolve(creatures); //Evolve cleans up the MTrees so no need for that here
			//print survivors sorted

			//#pragma omp for ordered
			for(int j=0;j< (int) (creatures->size()/5.f);j++){
				printf("nr %d %f\n",j,creatures->at(j).fitness);
			}
			//#pragma omp for ordered
			printf("round %d \n",i);
		}
	}

	for (int i = 0; i < (int) creatures->at(0).dna.size(); i++){
		printf("%d,", creatures->at(0).dna.at(i));
	}

	//Show end result if we want to...
	Physics* WWDPhysics = new Physics();
	//init creature
	readDNA(&creatures->at(0).dna,WWDPhysics);
	//default glut doesn't return from mainloop

	WWDPhysics->calcSize();
	WWDPhysics->solveGroundConflicts();
	printf("\nPress enter to continue\n");
	getchar();
	return glutmain(argc, argv,1024,600,"Walking with dinosaurs",WWDPhysics);
}