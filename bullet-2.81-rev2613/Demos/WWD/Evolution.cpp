#include "Evolution.h"

void initEvolution(int noCreatures){
	survivors = (int) ((noCreatures/100.f)*((float)survivalRatio)+0.5);
	cullAmount = (int) ((noCreatures/100.f)*((float)cullRatio)+0.5);

#ifdef _DEBUG
	printf("\nsurvivors %d\n", survivors);
	printf("\nculled creatures %d\n", cullAmount);
#endif

	if(survivors == 0 && noCreatures != 0){
		survivors =1;
	}
}

/**
*	main evolution loop
*/
statistic evolve(std::vector<creature>* creatures){
	statistic stats = statistic();
	//float fitnessSumElites = 0;
	//float fitnessSumBreeders = 0;
	std::vector<creature> result = std::vector<creature>();
	result.reserve(creatures->size());
	std::vector<creature> elites = std::vector<creature>();
	elites.reserve(survivors);
	std::vector<creature> breeders = std::vector<creature>();
	breeders.reserve(creatures->size()-survivors-cullAmount);
	
	std::sort(creatures->begin(), creatures->end(), compareCreatures);

	double deviation=statistik(creatures, &stats);
	bool panicking = checkForPanicAttack(creatures,stats);

	for(int i =0; i<(int)survivors; i++){
		result.push_back(creatures->at(i));
		elites.push_back(creatures->at(i));
		//fitnessSumElites += (float) (creatures->at(i).fitness>0.001?creatures->at(i).fitness:0.001);
	}

	for(int i =survivors; i<(((int)creatures->size())-cullAmount); i++){
		breeders.push_back(creatures->at(i));
		//fitnessSumBreeders += (float) (creatures->at(i).fitness>0.001?creatures->at(i).fitness:0.001);
	}

	for(int i =0; i<(int) creatures->size()-survivors;i++){
		creature creat;
		creat.fitness=0;

		int random = rand()%100+1;

		/*std::vector<int> dna = getWorthyCreature(fitnessSumBreeders,&breeders).dna;
		std::vector<int> dna2 = getWorthyCreature(fitnessSumElites,&elites).dna;
		if(random<=50){
		creat.dna = mutate(crossOver1(dna,dna2),deviation);
		}else{
		creat.dna = mutate(crossOver2(dna,dna2),deviation);
		}*/
		//creature breed = getWorthyCreature(fitnessSumBreeders,&breeders);
		//creature seed = getWorthyCreature(fitnessSumElites,&elites);
		creature breed = getWorthyCreature(0,&breeders);
		creature seed = getWorthyCreature(0,&elites);

		creat.dna = breed.treePointer->crossBreed(breed.dna,seed.dna,seed.treePointer);
		creat.dna = mutate(creat.dna,deviation);
		result.push_back(creat);
#ifdef _DEBUG
		printf("i %d\n",i);
#endif
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
/*
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

	return creatures->at(i);
}
*/
/**
*	mutes the dna according to the chance calculated
*	the chance is calculated as 1/lenght if diviation is as expected
*	if diviation is less the chance goes up
*	if higher it goes down
*/
std::vector<int> mutate(const std::vector<int> dna, double deviation){
	std::vector<int> newCreature = dna;
	double forskel= expectedDiviation-deviation;
	float chanceToMutate;
	if(forskel<0){
		chanceToMutate = 1/((float)dna.size())+1/((float)dna.size())*forskel/100.;
	}else{
		chanceToMutate = 1/((float)dna.size())*((float)dna.size())*forskel/100.;
	}
	for (int i = 0; i< (int) dna.size(); i++){
		float chance= (rand()%10000)/10000.;
		if(chanceToMutate>chance){
			newCreature.at(i)=randomDnaValue;
		}
	}

	return newCreature;
}

/**
*	One-point crossover
*//*
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
*/
/**
*   Two-point crossover
*	replaces the middel part of dna1 with a part from dna2
**//*
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
*/
/**
*	Calculation of statistics
*	all values excepet min and max are normalised to be in the span 0-100
*/
double statistik(std::vector<creature>* creatures, statistic* stats){
	float max=0, min=0, median=0, mean=0, deviation=0;
	stats->killed=0;
	stats->population=creatures->size();

	max =creatures->at(0).fitness;
	stats->max=max;

	for(int i =(int)creatures->size()-1;i>=0;i--){
		if(creatures->at(i).fitness>dead+0.00001){
			min=creatures->at(i).fitness;

			stats->killed=stats->population-1-i;
			break;
		}
	}
	stats->min=min;

	std::vector<float>* normValues=normalizeFitness(creatures, stats);

	for(int i = 0; i<(int)normValues->size(); i++){
		mean+=normValues->at(i);
	}
	mean/=normValues->size();
	stats->mean=mean;

	if(normValues->size()%2 ==0){
		float tmp1 =normValues->at((int)(normValues->size()/2.+0.5));
		float tmp2 =normValues->at((int)(normValues->size()/2.+0.5)-1);

		median = (tmp2-tmp1)/2.+tmp1;
	}else{
		median= normValues->at((int)(normValues->size()/2.+0.5)-1);
	}
	stats->median=median;

	if(normValues->size()>1){
		for(int i = 0; i<(int)normValues->size(); i++){
			deviation+=pow(normValues->at(i)-mean,2);
		}
		deviation=deviation/((float)(normValues->size()-1));
		deviation=sqrt(deviation);
	}else{
		deviation=0;
	}
	stats->deviation=deviation;
#ifdef _DEBUG
	printf("\nmax %f, min %f, median %f, mean %f, deviation %f\n", max, min, median, mean, deviation);
#endif
	delete normValues;
	timesDiviation+=1.;
	totalDiviation+=deviation;
#ifdef _DEBUG
	printf("mean diviation %f\n",totalDiviation/timesDiviation);
#endif
	return deviation;
}

/**
*	normalises the values to be in the span 0-100
*/
std::vector<float>* normalizeFitness(std::vector<creature> *creatures, statistic* stats){
	std::vector<float>* normValues= new std::vector<float>(creatures->size()-stats->killed);
	double difference = stats->max-stats->min;
	if(difference>0){
		for(int i =(int)creatures->size()-1-stats->killed;i>=0;i--){
			float tmpFit=creatures->at(i).fitness-stats->min;

			normValues->at(i)=(float)(tmpFit)/difference*100.;
		}
	}else{
		for(int i =(int)creatures->size()-1-stats->killed;i>=0;i--){
			normValues->at(i)=(float)50;
		}
	}
	return normValues;
}

bool checkForPanicAttack(std::vector<creature>* creatures,statistic statstruct){
#ifdef _DEBUG
			printf("killed: %d  culled: %d\n",statstruct.killed,cullAmount);
#endif
	if(statstruct.killed<=cullAmount){return false;}
	else{
		for(int dif = statstruct.killed-cullAmount;dif>0;dif--){
#ifdef _DEBUG
			printf("dif: %d\n",dif);
#endif
			int index = (creatures->size()-1) - cullAmount - (dif-1);
			int eliteIndex = rand()%survivors;
			creatures->at(index).dna = creatures->at(eliteIndex).dna;
			delete creatures->at(index).treePointer;
			creatures->at(index).treePointer = new MTree((const MTree&)*creatures->at(eliteIndex).treePointer);
		}
		return true;
	}
}