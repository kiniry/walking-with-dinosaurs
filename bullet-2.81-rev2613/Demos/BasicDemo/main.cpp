
//#include "Physics.h"
#include "Grammar.h"
#include "GlutStuff.h"

	
int main(int argc,char** argv)
{
	int populationSize = 1;
	std::vector<Physics*> creatures;
	int ancestor[] = {3,2,1,1,0,0,0,5,0,0,4,45,0,0,3,2,1,0};


	
	for(int i =0; i<populationSize; i++){
		//init world
		Physics* WWDPhysics = new Physics();
	
		//init creature
		//mutate(ancestor);
		//readDNA(ancestor,&WWDPhysics);
		
		creatures.push_back(WWDPhysics);
	
	}


		//start simulations
		creatures.at(0)->testPhysics();
		//default glut doesn't return from mainloop
		return glutmain(argc, argv,1024,600,"Walking with dinosaurs",creatures.at(0));

	
	
}

