#include "Grammar.h"

int readDNA(const int* DNA, Physics *world){
	int index = 0;
	int blocks = 0;
	try{	
		int part = world->createBox(DNA[index]%maxHeight+1, DNA[index+1]%maxWidth+1, DNA[index+2]%maxDepth+1);
		blocks++;
		index = index+3;
		index=B(index, DNA, world, &blocks, part);
	}catch (int e){
		return e;
	}
	return Gsucces;
}

int B(int index, const int* DNA, Physics *world, int *blocks, int part){
	if(*blocks >= maxBlocks){
		throw Gfail;
	}
				
	switch (DNA[index]%6){
		case 0:
			index++;
			break;
		case 1:
			index++;
			index = J(index, DNA, world, blocks, part);
			break;
		case 2:
			index++;
			index = J(index, DNA, world, blocks, part);
			index = J(index, DNA, world, blocks, part);
			break;
		case 3:
			index++;
			index = J(index, DNA, world, blocks, part);
			index = J(index, DNA, world, blocks, part);
			index = J(index, DNA, world, blocks, part);
			break;
		case 4:
			index++;
			index = J(index, DNA, world, blocks, part);
			index = J(index, DNA, world, blocks, part);
			index = J(index, DNA, world, blocks, part);
			index = J(index, DNA, world, blocks, part);
			break;
		case 5:
			index++;
			index = J(index, DNA, world, blocks, part);
			index = J(index, DNA, world, blocks, part);
			index = J(index, DNA, world, blocks, part);
			index = J(index, DNA, world, blocks, part);
			index = J(index, DNA, world, blocks, part);
			break;
		

		default:
			throw Gfail;
		
	}
	
	return index;
}

int J(int index, const int* DNA, Physics *world, int *blocks, int part1){
	int part2 = world->createBox(DNA[index+10]%maxHeight+1, DNA[index+11]%maxWidth+1, DNA[index+12]%maxDepth+1);
	int j_index = world->createJoint(part1, part2, DNA[index]%2, DNA[index+1], DNA[index+2], DNA[index+3]%6, DNA[index+4], DNA[index+5], DNA[index+6]%6, DNA[index+7], DNA[index+8], DNA[index+9]);
	index = NN(index+13, DNA, world);
	index = B(index, DNA, world, blocks, part2);
	return index;
}

int NN(int index, const int* DNA, Physics *world){
	
	index = NN(index, DNA, world);
	return index;
}