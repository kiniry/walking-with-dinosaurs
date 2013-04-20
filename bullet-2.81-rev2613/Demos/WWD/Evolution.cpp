#include "Evolution.h"


std::vector<creature> evolve(std::vector<creature> creatures){
	float fitnessSum = 0;
	std::vector<creature> result;

	std::sort(creatures.begin(), creatures.end(), compareCreatures);

	int survivors = (int) ((creatures.size()/100.f)*((float)survivalRatio)+0.5);

	printf("\nsurvivors %d\n", survivors);

	if(survivors == 0 && creatures.size() != 0){
		survivors =1;
	}

	for(int i =0; i<survivors; i++){
		result.push_back(creatures.at(i));
	}

	for(int i =0; i<(int) creatures.size()-survivors;i++){
		creature creat;
		creat.fitness=0;

		int random = rand()%100+1;

		int mut = 20;
		int cross1 = 40;
		//int cross2 = 40;
		std::vector<int> dna = creatures.at(i%survivors).dna;
		std::vector<int> dna2 = creatures.at(rand()%survivors).dna;

		if(random<=mut){
			creat.dna=mutate(dna);
		}else if(random<=mut+cross1){
			creat.dna=crossOver1(dna,dna2);
		}else{
			creat.dna= crossOver2(dna,dna2);
		}

		result.push_back(creat);
	}

	return result;
}

creature getWorthyCreature(float fitnessSum, std::vector<creature> creatures){
	int Sum = (int)(fitnessSum*10000.f);
	int random = rand()%Sum;
	int val = 0;
	int i=0;
	for(i;i<creatures.size();i++){
		val+=creatures.at(i).fitness*10000;
		if(val>random){break;}
	}
	return creatures.at(i);
}

std::vector<int> mutate(const std::vector<int> dna){
	std::vector<int> newCreature = dna;
	for (int i = 0; i< (int) dna.size(); i++){
		int chanceToMutate = rand()%dna.size();
		if(chanceToMutate==0){
			newCreature.at(i)=randomDnaValue;
		}
	}

	return newCreature;
}

std::vector<int> crossOver1(std::vector<int> dna1, std::vector<int> dna2){
	std::vector<int> newCreature;

	int random1 = rand()%dna1.size();
	for(int i = 0; i<=random1;i++){
		newCreature.push_back(dna1.at(i));
	}

	int random2 = rand()%dna2.size();
	for(int i = random2; i< (int) dna2.size();i++){
		newCreature.push_back(dna2.at(i));
	}

	return newCreature;
}

/**
*	replaces the middel part of dna1 with a part from dna2
**/
std::vector<int> crossOver2(std::vector<int> dna1, std::vector<int> dna2){
	std::vector<int> newCreature;

	int random1 = rand()%dna1.size();
	for(int i = 0; i<=random1;i++){
		newCreature.push_back(dna1.at(i));
	}

	int random2 = rand()%dna2.size();
	int length = rand()%(dna2.size()-random2);
	for(int i = random2; i<=random2+length;i++){
		newCreature.push_back(dna2.at(i));
	}

	for(int i = random1+length; i< (int) dna1.size();i++){
		newCreature.push_back(dna1.at(i));
	}
	return newCreature;
}