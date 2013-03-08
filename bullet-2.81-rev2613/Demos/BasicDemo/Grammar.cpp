#include "Grammar.h"

int readDNA(const int* DNA){
	int index = 0;
	int blocks = 0;
	try{
	int part = CreateBox(DNA[index]%maxHeight, DNA[index+1]%maxWidth, DNA[index+2]%maxDepth);
	blocks++;
	index=B(index, DNA, &blocks, part);
	}catch (int e){
		return e;
	}

	return sucecs;
}

int B(int index, const int* DNA, int *blocks, int part){
	if(*blocks >= maxBlocks){
		throw fail;
	}

	switch (DNA[index]%6){
		case 0:
			index++;
			break;
		case 1:
			index++;
			index = J(index, DNA, blocks, part);
			break;
		case 2:
			index++;
			index = J(index, DNA, blocks, part);
			index = J(index, DNA, blocks, part);
			break;
		case 3:
			index++;
			index = J(index, DNA, blocks, part);
			index = J(index, DNA, blocks, part);
			index = J(index, DNA, blocks, part);
			break;
		case 4:
			index++;
			index = J(index, DNA, blocks, part);
			index = J(index, DNA, blocks, part);
			index = J(index, DNA, blocks, part);
			index = J(index, DNA, blocks, part);
			break;
		case 5:
			index++;
			index = J(index, DNA, blocks, part);
			index = J(index, DNA, blocks, part);
			index = J(index, DNA, blocks, part);
			index = J(index, DNA, blocks, part);
			index = J(index, DNA, blocks, part);
			break;
		

		default:
			throw fail;
		
	}

	return index;
}

int J(int index, const int* DNA, int *blocks, int part){
	//fix index
	int part2 = CreateBox(DNA[index]%maxHeight, DNA[index+1]%maxWidth, DNA[index+2]%maxDepth);
	CreateJoint(part, part2,....);
	B(index, DNA, blocks, part2);
}