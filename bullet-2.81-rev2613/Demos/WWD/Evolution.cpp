#include "Evolution.h"


statistic evolve(std::vector<creature>* creatures){
	statistic stats = statistic();
	float fitnessSumElites = 0;
	float fitnessSumBreeders = 0;
	std::vector<creature> result;
	std::vector<creature> elites;
	std::vector<creature> breeders;

	std::sort(creatures->begin(), creatures->end(), compareCreatures);

	double deviation=statistik(creatures, &stats);

	int survivors = (int) ((creatures->size()/100.f)*((float)survivalRatio)+0.5);
	int cullAmount = (int) ((creatures->size()/100.f)*((float)cullRatio)+0.5);


	printf("\nsurvivors %d\n", survivors);
	printf("\nculled creatures %d\n", cullAmount);

	if(survivors == 0 && creatures->size() != 0){
		survivors =1;
	}

	for(int i =0; i<(int)survivors; i++){
		result.push_back(creatures->at(i));
		elites.push_back(creatures->at(i));
		fitnessSumElites += (float) (creatures->at(i).fitness>0.001?creatures->at(i).fitness:0.001);
	}

	for(int i =survivors; i<(((int)creatures->size())-survivors-cullAmount); i++){
		breeders.push_back(creatures->at(i));
		fitnessSumBreeders += (float) (creatures->at(i).fitness>0.001?creatures->at(i).fitness:0.001);
	}

	for(int i =0; i<(int) creatures->size()-survivors;i++){
		creature creat;
		creat.fitness=0;

		int random = rand()%100+1;

		/*std::vector<int> dna = getWorthyCreature(fitnessSumBreeders,breeders).dna;
		std::vector<int> dna2 = getWorthyCreature(fitnessSumElites,elites).dna;
		if(random<=50){
		creat.dna = mutate(crossOver1(dna,dna2),deviation);
		}else{
		creat.dna = mutate(crossOver2(dna,dna2),deviation);
		}*/
		creature breed = getWorthyCreature(fitnessSumBreeders,&breeders);
		creature seed = getWorthyCreature(fitnessSumElites,&elites);

		creat.dna = breed.treePointer->crossBreed(breed.dna,seed.dna,seed.treePointer);
		creat.dna = mutate(creat.dna,deviation);
		result.push_back(creat);
		printf("i %d\n",i);
	}

	//cleanup
	for(int i=0;i<(int)creatures->size();i++){
		delete creatures->at(i).treePointer;
	}

	//return result;
	*creatures = result;

	return stats;
}

creature getWorthyCreature(float fitnessSum, std::vector<creature> *creatures){
	int random = rand()%creatures->size();
	return creatures->at(random);

}


creature getWorthyCreatureFail(float fitnessSum, std::vector<creature> *creatures){
	int Sum = (int)(fitnessSum*1000.f);
	if(Sum<1){
		int random = rand()%creatures->size();
		return creatures->at(random);//if the sum of all the fitness is zero return a random instead... they are all useless
	}
	int random = (rand()*100)%Sum;	//Random number generator doesn't give high enough values so we multiply with 100 to be sure
	//we are in the range
	float val = 0;
	int i=0;
	for(i;i<(int)creatures->size();i++){
		val+=(creatures->at(i).fitness>0.001?creatures->at(i).fitness*1000.f:1.f);
		if(val>random){
			break;
		}
	}
	if(i>= (int)creatures->size()){
		printf("FAIL... how irritating");
		return creatures->at(creatures->size()-1);
	}
	//printf(" %d ",i);
	return creatures->at(i);
}

std::vector<int> mutate(const std::vector<int> dna, double deviation){
	std::vector<int> newCreature = dna;
	for (int i = 0; i< (int) dna.size(); i++){
		int chanceToMutate;

		double forskel= expectedDiviation-deviation;

		if(forskel>0){
			int chance=(int)(dna.size()*forskel/deviation+0.5);
			if(chance==dna.size()){chance--;}
			chanceToMutate = rand()%(dna.size()-chance);
		}else{
			chanceToMutate = rand()%dna.size();
		}

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

double statistik(std::vector<creature>* creatures, statistic* stats){
	float max=0, min=0, median=0, mean=0, deviation=0, killed=0;

	
	max =creatures->at(0).fitness;
	stats->max=max;

	for(int i =(int)creatures->size()-1;i>=0;i--){
		if(creatures->at(i).fitness>dead+0.0005){
			min=creatures->at(i).fitness;

			killed=creatures->size()-i;
			break;
		}
	
	}

	stats->killed=killed;
	stats->min=min;

	normalizeFitness(creatures, stats);


	for(int i = 0; i<(int)creatures->size(); i++){
		mean+=creatures->at(i).fitness;
	}
	mean/=creatures->size();
	stats->mean=mean;

	if(creatures->size()%2 ==0){
		float tmp1 =creatures->at((int)(creatures->size()/2.+0.5)).fitness;
		float tmp2 =creatures->at((int)(creatures->size()/2.+0.5)-1).fitness;

		median = (tmp2-tmp1)/2.+tmp1;
	}else{
		median= creatures->at((int)(creatures->size()/2.+0.5)-1).fitness;
	}
	stats->median=median;


	//should dead creatures be used for the deviration
	if(creatures->size()>1){
		for(int i = 0; i<(int)creatures->size(); i++){
			deviation+=pow(creatures->at(i).fitness-mean,2);
		}
		deviation=deviation/((float)(creatures->size()-1));
		deviation=sqrt(deviation);
	}else{
		deviation=0;
	}
	stats->deviation=deviation;
	printf("\nmax %f, min %f, median %f, mean %f, deviation %f\n", max, min, median, mean, deviation);

	timesDiviation+=1.;
	totalDiviation+=deviation;
	printf("mean diviation %f\n",totalDiviation/timesDiviation);
	return deviation;
}

void normalizeFitness(std::vector<creature> *creatures, statistic* stats){
	float min = 0;

	for(int i =creatures->size()-1; i>=0;i--){
		if(	creatures->at(i).fitness>dead+0.00001){
			min=creatures->at(i).fitness;
			break;
		}
	}
	stats->min=min;



	double max = creatures->at(0).fitness-min;
	for(int i =0; i<(int)creatures->size();i++){
		if(	creatures->at(i).fitness+0.00001>dead && creatures->at(i).fitness-0.00001<dead){
			creatures->at(i).fitness=0.000001;
		}else{
			creatures->at(i).fitness-=min;
			float tmp =creatures->at(i).fitness;

			creatures->at(i).fitness=(float)(creatures->at(i).fitness)/max*100.+0.00001;

		}

	}
}