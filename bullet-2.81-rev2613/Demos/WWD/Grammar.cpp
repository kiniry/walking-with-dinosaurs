#include "Grammar.h"

/**
*Here be Dinos!!!
*/
int readDNA(const std::vector<int> DNA, Physics *world){

	int index = 0;
	int blocks = 0;
	std::vector<DinoTreeNode*>* tempNeural = new std::vector<DinoTreeNode*>;
	world->DinoStructure = new DinoTreeNode(index,false);

	try{	
		DinoTreeNode* firstPart = new DinoTreeNode(index,false);
		world->DinoStructure->addChild(firstPart);
		int part = world->createBox(getDNA(index,DNA)%maxHeight+1, getDNA(index+1,DNA)%maxWidth+1, getDNA(index+2,DNA)%maxDepth+1);
		blocks++;
		index = index+3;
		firstPart->setEnd(index+1);
		index=B(index, DNA, world, &blocks, part, tempNeural,firstPart);
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
	world->DinoStructure->addChild(NeuralPart);
		
	world->theNet=new NeuralNetwork(inputs);
		//NN - create nodes
	index=NN(index,DNA, world->theNet,NeuralPart);
		//NN - end building
	world->theNet->stopBuilding();

	NeuralPart->setEnd(index);
	
	//link create sub NNs
	std::vector<NeuralNode*> mainOutputs = world->theNet->getLastLayer();
	for(int i=0;i<(int)tempNeural->size();i++){
		NeuralNetwork* subnet = new NeuralNetwork(mainOutputs);
		world->subnets.push_back(subnet);

		NN(tempNeural->at(i)->DNAindex,DNA,subnet,tempNeural->at(i));
		subnet->stopBuilding();
	}
	delete tempNeural;
	
	
	world->DinoStructure->setEnd(index);
	
	return Gsucces;
}

int B(int index, const std::vector<int> DNA, Physics *world, int *blocks, int part, std::vector<DinoTreeNode*>* tempNeural, DinoTreeNode* partTree){
	if(*blocks >= maxBlocks){
		throw Gfail;
	}

	DinoTreeNode* part1;
	DinoTreeNode* part2;
	DinoTreeNode* part3;
	DinoTreeNode* part4;
	DinoTreeNode* part5;

	switch (getDNA(index,DNA)%6){
		case 0:
			index++;
			break;
		case 1:
			index++; //the information how many boxes are attached - belongs with the previous box
			
			part1 = new DinoTreeNode(index,false);
			partTree->addChild(part1);
			 index = J(index, DNA, world, blocks, part, tempNeural, part1);
			part1->setEnd(index);
			break;
		case 2:
			index++; //the information how many boxes are attached - belongs with the previous box
			
			part1 = new DinoTreeNode(index,false);
			partTree->addChild(part1);
			 index = J(index, DNA, world, blocks, part, tempNeural,part1);
			part1->setEnd(index);

			part2 = new DinoTreeNode(index,false);
			partTree->addChild(part2);
			 index = J(index, DNA, world, blocks, part, tempNeural,part2);
			part2->setEnd(index);
			break;
		case 3:
			index++; //the information how many boxes are attached - belongs with the previous box
			
			part1 = new DinoTreeNode(index,false);
			partTree->addChild(part1);
			 index = J(index, DNA, world, blocks, part, tempNeural,part1);
			part1->setEnd(index);

			part2 = new DinoTreeNode(index,false);
			partTree->addChild(part2);
			 index = J(index, DNA, world, blocks, part, tempNeural,part2);
			part2->setEnd(index);
			
			part3 = new DinoTreeNode(index,false);
			partTree->addChild(part3);
			 index = J(index, DNA, world, blocks, part, tempNeural,part3);
			part3->setEnd(index);
			break;
		case 4:
			index++; //the information how many boxes are attached - belongs with the previous box
			
			part1 = new DinoTreeNode(index,false);
			partTree->addChild(part1);
			 index = J(index, DNA, world, blocks, part, tempNeural,part1);
			part1->setEnd(index);

			part2 = new DinoTreeNode(index,false);
			partTree->addChild(part2);
			 index = J(index, DNA, world, blocks, part, tempNeural,part2);
			part2->setEnd(index);
			
			part3 = new DinoTreeNode(index,false);
			partTree->addChild(part3);
			 index = J(index, DNA, world, blocks, part, tempNeural,part3);
			part3->setEnd(index);
			
			part4 = new DinoTreeNode(index,false);
			partTree->addChild(part4);
			 index = J(index, DNA, world, blocks, part, tempNeural,part4);
			part4->setEnd(index);
			break;
		case 5:
			index++; //the information how many boxes are attached - belongs with the previous box
			
			part1 = new DinoTreeNode(index,false);
			partTree->addChild(part1);
			 index = J(index, DNA, world, blocks, part, tempNeural,part1);
			part1->setEnd(index);

			part2 = new DinoTreeNode(index,false);
			partTree->addChild(part2);
			 index = J(index, DNA, world, blocks, part, tempNeural,part2);
			part2->setEnd(index);
			
			part3 = new DinoTreeNode(index,false);
			partTree->addChild(part3);
			 index = J(index, DNA, world, blocks, part, tempNeural,part3);
			part3->setEnd(index);
			
			part4 = new DinoTreeNode(index,false);
			partTree->addChild(part4);
			 index = J(index, DNA, world, blocks, part, tempNeural,part4);
			part4->setEnd(index);
			
			part5 = new DinoTreeNode(index,false);
			partTree->addChild(part5);
			 index = J(index, DNA, world, blocks, part, tempNeural,part5);
			part5->setEnd(index);
			break;
		

		default:
			throw Gfail;
		
	}
	
	return index;
}

int J(int index, const std::vector<int> DNA, Physics *world, int *blocks, int part1, std::vector<DinoTreeNode*>* tempNeural,DinoTreeNode* partTree){
	int part2 = world->createBox(getDNA(index+10,DNA)%maxHeight+1, getDNA(index+11,DNA)%maxWidth+1, getDNA(index+12,DNA)%maxDepth+1);
	int j_index = world->createJoint(part1, part2, getDNA(index,DNA)%2, getDNA(index+1,DNA), getDNA(index+2,DNA), getDNA(index+3,DNA)%6, getDNA(index+4,DNA), getDNA(index+5,DNA), getDNA(index+6,DNA)%6, getDNA(index+7,DNA), getDNA(index+8,DNA), getDNA(index+9,DNA));
	index+=13;
	tempNeural->push_back(new DinoTreeNode(index,true));
	index = NN(index,DNA);

	world->effectorNNindex.push_back(getDNA(index,DNA));
	world->effectorNNindex.push_back(getDNA(index+1,DNA));
	world->effectorNNindex.push_back(getDNA(index+2,DNA));
	index+=3;

	index = B(index, DNA, world, blocks, part2, tempNeural,partTree);
	return index;
}

int NN(int index,const std::vector<int> DNA){
	
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

int NN(int index, const std::vector<int> DNA, NeuralNetwork* net,DinoTreeNode* partTree){

	DinoTreeNode* aNode = new DinoTreeNode(index,true);
	partTree->addChild(aNode);

	switch(getDNA(index,DNA)%3){
	case 0: //this is the last node.
		index= NI(index+1, DNA, net);
		aNode->setEnd(index);
		break;
	case 1: //there is more nodes in this layer
		index=NI(index+1, DNA, net);
		aNode->setEnd(index);
		index=NN(index, DNA, net,aNode);
		break;
	case 2: //there is another layer
		index=NI(index+1, DNA, net);
		aNode->setEnd(index);
		net->changeLayer();
		index=NN(index, DNA, net,aNode);
		break;
	}
	return index;

}

int NI(int index,const std::vector<int> DNA){
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

int NI(int index, const std::vector<int> DNA, NeuralNetwork* net){
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

/*
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
}*/
