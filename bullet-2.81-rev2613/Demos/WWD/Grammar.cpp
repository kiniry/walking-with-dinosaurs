#include "Grammar.h"

/**
*Here be Dinos!!!
*/
int readDNA(std::vector<int> *DNA, Physics *world){
	int index = 0;
	int blocks = 0;
	std::vector<int>* tempNeural = new std::vector<int>;


	try{
		int part = world->createBox(getDNA(index,DNA), getDNA(index+1,DNA), getDNA(index+2,DNA));
		blocks++;
		index = index+3;
		index=B(index, DNA, world, &blocks, part, tempNeural);
	}catch (int e){
		return e;
	}

	//hoved NN
	//NN - define inputs
	std::vector<NeuralNode*> inputs;
	for(int i=0;i<(int)world->sensors.size();i++){
		inputs.push_back(new NeuralNode(& (world->sensors.at(i)) ));
	}

	DinoTreeNode* NeuralPart = new DinoTreeNode(index,true);

	world->theNet=new NeuralNetwork(inputs);
	//NN - create nodes
	index=NN(index,DNA, world->theNet);
	//NN - end building
	world->theNet->stopBuilding();

	NeuralPart->setEnd(index);

	//link create sub NNs
	std::vector<NeuralNode*> mainOutputs = world->theNet->getLastLayer();
	for(int i=0;i<(int)tempNeural->size();i++){
		NeuralNetwork* subnet = new NeuralNetwork(mainOutputs);
		world->subnets.push_back(subnet);

		NN(tempNeural->at(i),DNA,subnet);
		subnet->stopBuilding();
	}
	delete tempNeural;

	

	return Gsucces;
}

int B(int index, std::vector<int> *DNA, Physics *world, int *blocks, int part, std::vector<int>* tempNeural){
	if(*blocks >= maxBlocks){
		throw Gfail;
	}


	switch (getDNA(index,DNA)%6){
	case 0:
		index++;
		break;
	case 1:
		index++; //the information how many boxes are attached - belongs with the previous box
		index = J(index, DNA, world, blocks, part, tempNeural);

		break;
	case 2:
		index++; //the information how many boxes are attached - belongs with the previous box

		index = J(index, DNA, world, blocks, part, tempNeural);
		index = J(index, DNA, world, blocks, part, tempNeural);
		break;
	case 3:
		index++; //the information how many boxes are attached - belongs with the previous box

		index = J(index, DNA, world, blocks, part, tempNeural);
		index = J(index, DNA, world, blocks, part, tempNeural);
		index = J(index, DNA, world, blocks, part, tempNeural);
		break;
	case 4:
		index++; //the information how many boxes are attached - belongs with the previous box

		index = J(index, DNA, world, blocks, part, tempNeural);
		index = J(index, DNA, world, blocks, part, tempNeural);
		index = J(index, DNA, world, blocks, part, tempNeural);
		index = J(index, DNA, world, blocks, part, tempNeural);
		break;
	case 5:
		index++; //the information how many boxes are attached - belongs with the previous box

		index = J(index, DNA, world, blocks, part, tempNeural);
		index = J(index, DNA, world, blocks, part, tempNeural);
		index = J(index, DNA, world, blocks, part, tempNeural);
		index = J(index, DNA, world, blocks, part, tempNeural);
		index = J(index, DNA, world, blocks, part, tempNeural);
		break;

	default:
		throw Gfail;
	}
	return index;
}

int J(int index, std::vector<int> *DNA, Physics *world, int *blocks, int part1, std::vector<int>* tempNeural){
	int part2 = world->createBox(getDNA(index+10,DNA), getDNA(index+11,DNA), getDNA(index+12,DNA));
	int j_index = world->createJoint(part1, part2, getDNA(index,DNA), getDNA(index+1,DNA), getDNA(index+2,DNA), getDNA(index+3,DNA), getDNA(index+4,DNA), getDNA(index+5,DNA), getDNA(index+6,DNA), getDNA(index+7,DNA), getDNA(index+8,DNA), getDNA(index+9,DNA));
	index+=13;
	tempNeural->push_back(index);
	index = NN(index,DNA);

	world->effectorNNindex.push_back(getDNA(index,DNA));
	world->effectorNNindex.push_back(getDNA(index+1,DNA));
	world->effectorNNindex.push_back(getDNA(index+2,DNA));
	index+=3;

	index = B(index, DNA, world, blocks, part2, tempNeural);
	return index;
}

int NN(int index,std::vector<int> *DNA){
	switch(getDNA(index,DNA)%3){
	case 0: //this is the last node.
		index= NI(index+1,DNA);
		break;
	case 1: //there is more nodes in this layer
		index=NI(index+1,DNA);
		index=NN(index,DNA);
		break;
	case 2: //there is another layer
		index=NI(index+1,DNA);
		index=NN(index,DNA);
		break;
	}
	return index;
}

int NN(int index, std::vector<int> *DNA, NeuralNetwork* net){
	switch(getDNA(index,DNA)%3){
	case 0: //this is the last node.
		index= NI(index+1, DNA, net);
		break;
	case 1: //there is more nodes in this layer
		index=NI(index+1, DNA, net);
		index=NN(index, DNA, net);
		break;
	case 2: //there is another layer
		index=NI(index+1, DNA, net);
		net->changeLayer();
		index=NN(index, DNA, net);
		break;
	}
	return index;
}

int NI(int index,std::vector<int> *DNA){
	switch(getDNA(index,DNA)%4){
	case 0:
		index+=2;
		break;
	case 1:
		index+=4;
		break;
	case 2:
		index+=6;
		break;
	case 3:
		index+=8;
		break;
	}
	return index;
}

int NI(int index, std::vector<int> *DNA, NeuralNetwork* net){
	switch(getDNA(index,DNA)%4){
	case 0:
		net->insertNode(toFloat(getDNA(index+1,DNA)));
		index+=2;
		break;
	case 1:
		net->insertNode(getDNA(index+1,DNA),getDNA(index+2,DNA),toFloat(getDNA(index+3,DNA)));
		index+=4;
		break;
	case 2:
		net->insertNode(getDNA(index+1,DNA),getDNA(index+2,DNA),getDNA(index+4,DNA),toFloat(getDNA(index+3,DNA)),toFloat(getDNA(index+5,DNA)));
		index+=6;
		break;
	case 3:
		net->insertNode(getDNA(index+1,DNA),getDNA(index+2,DNA),getDNA(index+4,DNA),getDNA(index+6,DNA),toFloat(getDNA(index+3,DNA)),toFloat(getDNA(index+5,DNA)),toFloat(getDNA(index+7,DNA)));
		index+=8;
		break;
	}
	return index;
}