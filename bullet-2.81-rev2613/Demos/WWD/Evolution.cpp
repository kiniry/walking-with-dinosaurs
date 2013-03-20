#include "Evolution.h"

int* evolv(int* dna, int size){

	int random = rand()%100+1;

	int mut = 40;
	int cross1 = 30;
	int cross2 = 30;

	if(random<=mut){

		return mutate(dna, size);

	}else if(random<=mut+cross1){

		return crossOver1(dna, size);

	}else{

		return crossOver2(dna, size);
	}

}




int* mutate(int* dna, int size){
	
	


	return dna;
}

int* crossOver1(int* dna, int size){

	return dna;
}

int* crossOver2(int* dna, int size){


	return dna;
}




