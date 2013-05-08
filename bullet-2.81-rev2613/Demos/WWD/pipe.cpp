#include "pipe.h"

std::vector<creature> getCreatures(std::string filename){
	printf("reading creatures from %s\n", filename.c_str());
	std::vector<creature> creatures;

	std::ifstream is;
	is.open(filename,std::ios::in | std::ios::binary);

	int noCreatures;
	is.read((char*)&noCreatures, sizeof(int));

	printf("creatures to be read %d\n",	 noCreatures);

	for(int i=0;i<noCreatures;i++){
		creature* tmpCreature =new creature();

		//read dna
		int sizeOfDna;
		is.read((char*)&sizeOfDna, sizeof(int));
		tmpCreature->dna.resize(sizeOfDna);
		is.read((char*)&tmpCreature->dna[0], sizeof(int)*sizeOfDna);

		//read fitness
		is.read((char*)&tmpCreature->fitness, sizeof(float));
		printf("fitness nr %d: %f\n",i, tmpCreature->fitness);
		creatures.push_back(*tmpCreature);
	}
	if(!is.good()){
		printf("error\n");
		exit(-1);
	}
	is.close();
	printf("\n");
	return creatures;
}

std::string getDirectory(){
	//filepath creatures
	char fileName[MAX_PATH];
	HINSTANCE hInstance = GetModuleHandle(NULL);
	GetModuleFileName(hInstance, fileName, MAX_PATH);

	char filePathAbs[MAX_PATH];
	//make absolute
	std::vector<int> slashes;
	int j =0;
	for(int i =0;i<MAX_PATH; i++){
		if(fileName[i]=='\\'){
			slashes.push_back(i);
		}
		if(fileName[i]=='.' && fileName[i+1]=='.' && fileName[i+2]=='\\'){
			slashes.pop_back();
			i++;
			j=slashes.back()+1;
			slashes.pop_back();
		}else{
			filePathAbs[j]=fileName[i];
			j++;
		}
	}

	PathRemoveFileSpec(filePathAbs);
	PathAddBackslash(filePathAbs);

	directory.clear();
	directory.append(filePathAbs);

	printf("directory: %s\n",directory.c_str());

	return directory;
}