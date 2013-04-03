#include "Evolution.h"

std::vector<int> evolv(const std::vector<int> dna, int size){

	int random = rand()%100+1;

	int mut = 40;
	int cross1 = 30;
	int cross2 = 30;

	if(random<=mut){

		return mutate(dna);

	}else if(random<=mut+cross1){

		return crossOver1(dna);

	}else{

		return crossOver2(dna);
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

std::vector<int> crossOver1(std::vector<int> dna){

	return dna;
}

std::vector<int> crossOver2(std::vector<int> dna){


	return dna;
}




