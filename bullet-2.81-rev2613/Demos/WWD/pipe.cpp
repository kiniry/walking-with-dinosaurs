#include "pipe.h"

std::vector<creature> getCreatures(std::string filename){
	std::vector<creature> creatures;

	std::ifstream is;
	is.open(filename,std::ios::in | std::ios::binary);

	int noCreatures;
	is.read((char*)&noCreatures, sizeof(int));

	for(int i=0;i<noCreatures;i++){
		creature* tmpCreature =new creature();

		//read dna
		int sizeOfDna;
		is.read((char*)&sizeOfDna, sizeof(int));
		tmpCreature->dna.resize(sizeOfDna);
		is.read((char*)&tmpCreature->dna[0], sizeof(int)*sizeOfDna);

		//read fitness
		is.read((char*)&tmpCreature->fitness, sizeof(float));

		creatures.push_back(*tmpCreature);
		delete tmpCreature;
	}
	if(!is.good()){
#ifdef _DEBUG
		printf("error\n");
#endif
		exit(-1);
	}
	is.close();

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

	return directory;
}