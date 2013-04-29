#include "pipe.h"




std::vector<creature> getCreatures(std::string filename){


	printf("reading creatures from %s\n", filename.c_str());
   	std::vector<creature> creatures;
	
	std::ifstream is (filename);


	int noCreatures;
	is.read((char*)&noCreatures, sizeof(int));


	for(int i=0;i<noCreatures;i++){
	creature* tmpCreature =new creature();
	
	//read dna
	int sizeOfDna;
	is.read((char*)&sizeOfDna, sizeof(int));
	tmpCreature->dna.resize(sizeOfDna);
	is.read((char*)&tmpCreature->dna, sizeof(tmpCreature->dna));

	//read fitness
	is.read((char*)&tmpCreature->fitness, sizeof(float));

	creatures.push_back(*tmpCreature);
	}


	is.close();
	
	
	return creatures;

}




void setDirectory(){


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


	directory.append(filePathAbs);

	printf("directory: %s\n",directory.c_str());

}