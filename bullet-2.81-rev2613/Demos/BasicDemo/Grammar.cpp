#include "Grammar.h"

int readDNA(const int* DNA, Physics *world){

	int index = 0;
	int blocks = 0;
	std::vector<int> tempNeural;
	
	try{	
		int part = world->createBox(DNA[index]%maxHeight+1, DNA[index+1]%maxWidth+1, DNA[index+2]%maxDepth+1);
		blocks++;
		index = index+3;
		index=B(index, DNA, world, &blocks, part, tempNeural);
	}catch (int e){
		return e;
	}

	//hoved NN
		//NN - define inputs
	std::vector<NeuralNode*> inputs;
	for(int i=0;i<world->sensors.size();i++){
		inputs.push_back(new NeuralNode(& (world->sensors.at(i)) ));
	}
	world->theNet=new NeuralNetwork(inputs);
		//NN - create nodes
	NN(index,DNA, world->theNet);
		//NN - end building
	world->theNet->stopBuilding();

	//link create sub NNs
	std::vector<NeuralNode*> mainOutputs = world->theNet->getLastLayer();
	for(int i=0;i<tempNeural.size();i++){
		NeuralNetwork* subnet = new NeuralNetwork(mainOutputs);
		world->subnets.push_back(subnet);
		NN(tempNeural.at(i),DNA,subnet);
		subnet->stopBuilding();
	}
	
	return Gsucces;
}

int B(int index, const int* DNA, Physics *world, int *blocks, int part, std::vector<int> tempNeural){
	if(*blocks >= maxBlocks){
		throw Gfail;
	}
				
	switch (DNA[index]%6){
		case 0:
			index++;
			break;
		case 1:
			index++;
			index = J(index, DNA, world, blocks, part, tempNeural);
			break;
		case 2:
			index++;
			index = J(index, DNA, world, blocks, part, tempNeural);
			index = J(index, DNA, world, blocks, part, tempNeural);
			break;
		case 3:
			index++;
			index = J(index, DNA, world, blocks, part, tempNeural);
			index = J(index, DNA, world, blocks, part, tempNeural);
			index = J(index, DNA, world, blocks, part, tempNeural);
			break;
		case 4:
			index++;
			index = J(index, DNA, world, blocks, part, tempNeural);
			index = J(index, DNA, world, blocks, part, tempNeural);
			index = J(index, DNA, world, blocks, part, tempNeural);
			index = J(index, DNA, world, blocks, part, tempNeural);
			break;
		case 5:
			index++;
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

int J(int index, const int* DNA, Physics *world, int *blocks, int part1, std::vector<int> tempNeural){
	int part2 = world->createBox(DNA[index+10]%maxHeight+1, DNA[index+11]%maxWidth+1, DNA[index+12]%maxDepth+1);
	int j_index = world->createJoint(part1, part2, DNA[index]%2, DNA[index+1], DNA[index+2], DNA[index+3]%6, DNA[index+4], DNA[index+5], DNA[index+6]%6, DNA[index+7], DNA[index+8], DNA[index+9]);
	
	tempNeural.push_back(index+13);
	index = NN(index+13);

	world->effectorNNindex.push_back(DNA[index]);
	world->effectorNNindex.push_back(DNA[index+1]);
	world->effectorNNindex.push_back(DNA[index+2]);
	index+=3;

	index = B(index, DNA, world, blocks, part2, tempNeural);
	return index;
}

int NN(int index){
	
	switch(index%3){
	case 0: //this is the last node.
		index+= NI(index+1);
		break;
	case 1: //there is more nodes in this layer
		index+=NI(index+1);
		index+=NN(index);
		break;
	case 2: //there is another layer
		index+=NI(index+1);
		index+=NN(index);
		break;
	}
	return index;
}

int NN(int index, const int* DNA, NeuralNetwork* net){

	switch(index%3){
	case 0: //this is the last node.
		index+= NI(index+1, DNA, net);
		break;
	case 1: //there is more nodes in this layer
		index+=NI(index+1, DNA, net);
		index+=NN(index, DNA, net);
		break;
	case 2: //there is another layer
		index+=NI(index+1, DNA, net);
		net->changeLayer();
		index+=NN(index, DNA, net);
		break;
	}
	return index;

}

int NI(int index){
	return 2+I(index);
}

int NI(int index, const int* DNA, NeuralNetwork* net){
	int function = DNA[index]; index++;
	switch(index%3){
	case 0:
		net->insertNode(toFloat(function));
	case 1:
		net->insertNode(function,DNA[index],toFloat(DNA[index+1]));
		index+=2;
		break;
	case 2:
		net->insertNode(function,DNA[index],DNA[index+2],toFloat(DNA[index+1]),toFloat(DNA[index+3]));
		index+=4;
		break;
	case 3:
		net->insertNode(function,DNA[index],DNA[index+2],DNA[index+4],toFloat(DNA[index+1]),toFloat(DNA[index+3]),toFloat(DNA[index+5]));
		index+=6;
		break;
	}
	return index;
}


int I(int index){
	switch(index%3){
	case 0:
		index+=II(index+1);
		break;
	case 1:
		index+=II(index+1);
		index+=II(index);
		break;
	case 2:
		index+=II(index+1);
		index+=II(index);
		index+=II(index);
		break;
	}
	return index;
}

int II(int index){
	switch(index%2){
	case 0:
		return index+1+2;
		break;
	case 1:
		return index+1+1;
	}
}