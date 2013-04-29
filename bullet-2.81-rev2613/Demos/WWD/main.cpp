#include "main.h"

int main(int argc,char** argv)
{
	//seeds random generator
	srand(time(0));


#ifdef  _DEBUG	 
	return pipeClientMain(argc,argv);
		std::vector<int> ancestor;
	//return pipeServerMain(1,populationSize,nrOfGenerations,ancestor);
		return WWD(argc,argv);
	return debug(argc,argv);


	

#else

	const int temp[] = {1387,38,23,2,1924};
	int size = sizeof( temp ) / sizeof ( *temp );
	std::vector<int> ancestor (temp, temp+size);
	return pipeServerMain(1,populationSize,nrOfGenerations,ancestor);
	//return pipeClientMain(argc,argv);
	return WWD(argc,argv);

#endif

	return 0;
}


unsigned int __stdcall threadSim(void* data){
	//printf("%d ",(int) data); 
	for(int j=(int) data;j< (int) worlds.size();j+=numCores){

		worlds.at(j)->runSimulation(); //runs a physics simulation and save the fitness values
	}

	return 0;
}



int debug(int argc,char** argv){

	test();
	const int temp[] = {1387,38,23,2,1924};
	//	const int temp[] = {
	//Body
	//		295,195,95,	//main box: h,w,d
	//		1,		//1 box attached
	//		1,/*Joint type*/		50,50,5,/*preXYS*/		50,50,4,/*postXYS*/	45,45,45,/*DofXYZ*/ //Create joint
	//		295,195,95,	//attached box: h,w,d
	//NN-Effector0-layer 0
	//		1,	2,10,0,1,100,100,	//2-in-node: f=interpolate, in0=0,in1=1,w0=100,w1=100
	//		0,	2,0,0,0,0,100,		//2-in-node: f=sum,in0=0,in1=1,w0=0,w1=100 (just sends in1 through unchanged) - No more nodes
	//		1,0,1,	//NN-Effector: use outputs O0=1,O1=0,O2=1
	//		0,		//no attached boxes
	//NN-main-layer 0	(sensors not implemented yet)
	//		1,	0,30,			//constant node 30
	//		1,	0,62,			//constant node 62
	//		2,	0,125,			//constant node 125 - change layer
	//NN-main-layer 1
	//		1,	2,0,0,1,50,23,	//2-in-node: f=sum in0=0 in1=1 w0=50 w1=23
	//		2,	1,12,2,5,		//1-in node: f=cos in0=2 w0=5 - change layer
	//NN-main-layer 2
	//		1,	2,1,0,1,1,200,	//2-in node: f=product in0=0 in1=1, w0=1 w1=200
	//		0,	2,0,0,1,1,1		//2-in node: f=sum in0=0 in1=1, w0=1 w1=1 - No more nodes
	//	};
	int size = sizeof( temp ) / sizeof ( *temp );
	std::vector<int> ancestor (temp, temp+size);
	Physics* WWDPhysics = new Physics(ancestor);
	//init creature
	readDNA(&ancestor,WWDPhysics);
	//WWDPhysics->testPhysics();
	//default glut doesn't return from mainloop
	WWDPhysics->solveGroundConflicts();
	return glutmain(argc, argv,1024,600,"Walking with dinosaurs",WWDPhysics);
}


int WWD(int argc,char** argv){

	//threads
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );

	numCores= sysinfo.dwNumberOfProcessors;
	numCores=1;
	printf("cores %d\n", numCores);


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

	std::vector<creature> creatures;

	//Creates one ancestor and the rest is mutation of the ancestor
	creatures.push_back(creature());
	creatures.at(0).dna=ancestor;
	creatures.at(0).fitness=0;
	for(int i=1; i<populationSize;i++){
		creatures.push_back(creature());
		creatures.at(i).dna=mutate(ancestor,2);
		creatures.at(i).fitness=0;
	}

	for(int i=0;i<nrOfGenerations;i++){
		  //#pragma omp parallel
		{
			//initialise
			//#pragma omp single
			for(int j =0; j<populationSize; j++){
				//init world
				Physics* WWDPhysics = new Physics(creatures.at(j).dna);

				//init creature
				readDNA(&creatures.at(j).dna,WWDPhysics);

				worlds.push_back(WWDPhysics);
			}
			  
			//run simulator
			//#pragma omp for schedule(dynamic)
			for(int i=0;i< (int) worlds.size();i++){
				worlds.at(i)->runSimulation(); //runs a physics simulation and save the fitness values
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


			//#pragma omp for
			for(int j= worlds.size()-1; j>=0; j--){
				creatures.at(j).fitness = worlds.at(j)->getFitness();

			}
			//#pragma omp single nowait
			for(int j= worlds.size()-1; j>=0; j--){
				delete worlds.at(j);
				worlds.pop_back();
			}
			//print all unsorted
			/*for(int j=0;j< (int) worlds.size();j++){
			printf("nr %d %f\n",j,worlds.at(j)->getFitness());
			}*/

			//mutate
			//#pragma omp single nowait
			creatures=evolve(creatures);
			//print survivors sorted

			 //#pragma omp for ordered
			for(int j=0;j< (int) (creatures.size()/5.f);j++){
				printf("nr %d %f\n",j,creatures.at(j).fitness);
			}
			 //#pragma omp for ordered
			printf("round %d \n",i);
		}
	}

	for (int i = 0; i < (int) creatures.at(0).dna.size(); i++){
		printf("%d,", creatures.at(0).dna.at(i));
	}

	//Show end result if we want to...
	Physics* WWDPhysics = new Physics(creatures.at(0).dna);
	//init creature
	readDNA(&creatures.at(0).dna,WWDPhysics);
	//default glut doesn't return from mainloop

	WWDPhysics->calcSize();
	WWDPhysics->solveGroundConflicts();
	getchar();
	return glutmain(argc, argv,1024,600,"Walking with dinosaurs",WWDPhysics);

}