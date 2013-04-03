#include "Evolution.h"

std::vector<int> evolv(const std::vector<int> dna, int size){

	int random = rand()%100+1;

	int mut = 40;
	int cross1 = 30;
	int cross2 = 30;

	if(random<=mut){

		return mutate(dna);

	}else if(random<=mut+cross1){

		return crossOver1(dna,dna);

	}else{

		return crossOver2(dna,dna);
	}

}




std::vector<int> mutate(const std::vector<int> dna){
	
	std::vector<int> newCreature = dna;
	for (int i = 0; i<dna.size(); i++){
		int random = rand()%dna.size();
		if(random==0){

			newCreature.at(i)=rand()%maxDNAValue;
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
	for(int i = random2; i<dna2.size();i++){

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


	for(int i = random1+length; i<=dna1.size();i++){

		newCreature.push_back(dna1.at(i));

	}
	return newCreature;
}




