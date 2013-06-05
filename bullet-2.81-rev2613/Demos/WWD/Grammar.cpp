#include "Grammar.h"

/**
*	Here be Dinos!!!
*/

/**
*	Reads and loads a creature from dna
*/
int readDNA(std::vector<int> *DNA, Physics *world){
	int index = 0;
	int blocks = 0;
	std::vector<int>* tempNeural = new std::vector<int>;

	int part = world->createBox(getDNA(index,DNA), getDNA(index+1,DNA), getDNA(index+2,DNA));
	blocks++;
	index = index+3;
	index=B(index, DNA, world, &blocks, part, tempNeural);

	//hoved NN
	//NN - define inputs
	std::vector<NeuralNode*> inputs;
	for(int i=0;i<(int)world->sensors.size();i++){
		inputs.push_back(new NeuralNode(& (world->sensors.at(i)) ));
	}

	index = NN(index,DNA,inputs,world,true);

	//link create sub NNs
	std::vector<NeuralNode*> mainOutputs = world->theNet->getLastLayer();
	for(int i=0;i<(int)tempNeural->size();i++){
		NN(tempNeural->at(i),DNA,mainOutputs,world,false);
	}
	delete tempNeural;

	return 0;
}


int B(int index, std::vector<int> *DNA, Physics *world, int *blocks, int part, std::vector<int>* tempNeural){
	int attachedCount = getDNA(index,DNA)%6;
	index++; //the information how many boxes are attached - belongs with the previous box
	for(int i=0;i<attachedCount;i++){
		index = J(index, DNA, world, blocks, part, tempNeural);
	}

	return index;
}

int J(int index, std::vector<int> *DNA, Physics *world, int *blocks, int part1, std::vector<int>* tempNeural){
	partNode* falseBody = new partNode(0);
	index = NN(index,DNA,falseBody);
	delete falseBody;
	world->effectorNNindex.push_back(getDNA(index,DNA));
	world->effectorNNindex.push_back(getDNA(index+1,DNA));
	world->effectorNNindex.push_back(getDNA(index+2,DNA));
	index+=3;

	int part2 = world->createBox(getDNA(index+10,DNA), getDNA(index+11,DNA),getDNA(index+12,DNA));

	int j_index = world->createJoint(part1, part2, getDNA(index,DNA), getDNA(index+1,DNA), getDNA(index+2,DNA), getDNA(index+3,DNA), getDNA(index+4,DNA), getDNA(index+5,DNA), getDNA(index+6,DNA), getDNA(index+7,DNA), getDNA(index+8,DNA),getDNA(index+9,DNA));
	index+=13;
	tempNeural->push_back(index);

	index = B(index, DNA, world, blocks, part2, tempNeural);
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

int NN(int index, std::vector<int>* DNA, std::vector<NeuralNode*> inputs, Physics *world, bool isNotSubnet){
	NeuralNetwork* aNet = new NeuralNetwork(inputs);
	int amountOfLayers=0;
	if(isNotSubnet){
		world->theNet=aNet;
	}else{
		world->subnets.push_back(aNet);
	}

	int chooseValue = getDNA(index,DNA)%100;
	index++;

	if(chooseValue<15){
		amountOfLayers=1;
	}else if(chooseValue<40){
		amountOfLayers=2;
	}else if(chooseValue<85){
		amountOfLayers=3;
	}else{ //if chooseValue<100
		amountOfLayers=4;
	}
	for(int i=0;i<amountOfLayers;i++){
		NNLayerNode* falseNode = new NNLayerNode(0);
		index = NNL(index,SUBNET_NET_WIDTH,DNA,aNet,falseNode,false);
		delete falseNode;

		if(i!=amountOfLayers-1) aNet->changeLayer(); //Dont change on last layer (would create an empty layer)
	}

	aNet->stopBuilding();
	return index;
}

//function for calculating subnet size
int NN(int index, std::vector<int> *DNA,partNode* body){
	int amountOfLayers=0;

	int chooseValue = getDNA(index,DNA)%100;
	index++;
	if(chooseValue<15){
		amountOfLayers=1;
	}else if(chooseValue<40){
		amountOfLayers=2;
	}else if(chooseValue<85){
		amountOfLayers=3;
	}else{ //if chooseValue<100
		amountOfLayers=4;
	}
	for(int i=0;i<amountOfLayers;i++){
		NNLayerNode* aLayerNode = new NNLayerNode(index);
		body->addNNChild(aLayerNode);
		index = NNL(index,SUBNET_NET_WIDTH,DNA,0,aLayerNode,true);
		if(index<=DNA->size()){
			aLayerNode->setEnd(index);}
		else{aLayerNode->setEnd(DNA->size());}
	}

	return index;
}

//is isJustChecking is set, we are only calculating the size of the DNA part, not creating the actual NN nodes
int NNL(int index, int inputAmount, std::vector<int>* DNA, NeuralNetwork* aNet, NNLayerNode* node, bool isJustChecking){
	int chooseValue = getDNA(index,DNA)%1000;
	int usedChance = 0;
	int nrOfNodes = inputAmount; //assume the most probable case

	for(int i=1;i<inputAmount;i++){
		if(inputAmount-i<=0){break;}	//since we reached this, none of the chances procced... since the remaining chance-
		//all belong to the "nrOfNodes = inputAmount" case, this is chosen
		double dChance = 300;
		for(int j=0;j<i;j++){
			dChance = dChance / 2;		//dChance is calculated... the further from the MPC, the lower chance
		}

		usedChance+=dChance;			//this case is procced if chooseValue is in the interval usedChance..usedChance+dChance
		if(chooseValue < usedChance){nrOfNodes = inputAmount-i;break;}
		usedChance+=dChance;			//this case is procced if chooseValue is in the interval usedChance..usedChance+dChance
		if(chooseValue < usedChance){nrOfNodes = inputAmount+i;break;}
	}

	if(nrOfNodes==0){nrOfNodes=1;} //there must allways be at least one
	for(int i=0;i<nrOfNodes;i++){
		NNNode* aNode = new NNNode(index);
		node->addChild(aNode);
		if(!isJustChecking){index = NI(index,DNA,aNet);} //insert the nodes
		else{index = NI(index,DNA);}
		if(index<=DNA->size()){
			aNode->setEnd(index);}
		else{aNode->setEnd(DNA->size());}
	}
	return index;
}

/**
*Function to create MTree
*/
MTree* getMTree(std::vector<int> *DNA){
	int index = 0;
	int blocks = 0;
	MTree* result = new MTree();

	partNode* body = new partNode(index); //part0 begins here
	result->setBody(body);

	blocks++;
	index = index+3;
	index=treeB(index, DNA, &blocks, body);

	//hoved NN
	index = treeNN(index,DNA,result);

	return result;
}

int treeB(int index, std::vector<int> *DNA, int *blocks, partNode* body){
	int attachedCount=getDNA(index,DNA)%6;
	index++;
	for(int i=0;i<attachedCount;i++){
		index = treeJ(index, DNA, blocks, body);
	}
	if(index<=DNA->size()){
		body->setEnd(index);}
	else{body->setEnd(DNA->size());}
	return index;
}

int treeJ(int index, std::vector<int> *DNA, int *blocks, partNode* body){
	index = NN(index,DNA,body);
	index+=3; //effektors

	//partTreeBox begins here from index+10 (x,y,z)
	partNode* nextBody = new partNode(index+10);
	body->addChild(nextBody);

	//for the normal joint vars
	index+=13;

	index = treeB(index, DNA, blocks,nextBody); //nextBody will end at the end of this B() function
	return index;
}

int treeNN(int index, std::vector<int> *DNA,MTree* mainTree){
	int amountOfLayers=0;

	int chooseValue = getDNA(index,DNA)%100;
	index++;
	if(chooseValue<15){
		amountOfLayers=1;
	}else if(chooseValue<40){
		amountOfLayers=2;
	}else if(chooseValue<85){
		amountOfLayers=3;
	}else{ //if chooseValue<100
		amountOfLayers=4;
	}
	for(int i=0;i<amountOfLayers;i++){
		NNLayerNode* aLayerNode = new NNLayerNode(index);
		mainTree->addNeuralNetworkLayer(aLayerNode);
		index = NNL(index,SUBNET_NET_WIDTH,DNA,0,aLayerNode,true);
		if(index<=DNA->size()){
			aLayerNode->setEnd(index);}
		else{aLayerNode->setEnd(DNA->size());}
	}
	return index;
}