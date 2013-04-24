#include "Grammar.h"

/**
*Here be Dinos!!!
*/
int readDNA(std::vector<int> *DNA, Physics *world){
	int index = 0;
	int blocks = 0;
	std::vector<int>* tempNeural = new std::vector<int>;
	world->theTree = new MTree();

	try{

		partNode* body = new partNode(index); //part0 begins here
		world->theTree->setBody(body);

		int part = world->createBox(getDNA(index,DNA), getDNA(index+1,DNA), getDNA(index+2,DNA));
		blocks++;
		index = index+3;
		index=B(index, DNA, world, &blocks, part, tempNeural, body);

	}catch (int e){
		return e;
	}

	//hoved NN
	//NN - define inputs
	std::vector<NeuralNode*> inputs;
	for(int i=0;i<(int)world->sensors.size();i++){
		inputs.push_back(new NeuralNode(& (world->sensors.at(i)) ));
	}

	//world->theNet=new NeuralNetwork(inputs);
	//NNLayerNode* netPart = new NNLayerNode(index);
	//world->theTree->addNeuralNetworkLayer(netPart);
	//NN - create nodes
	//index=NN(index,DNA, world->theNet, world,netPart,true);
	index = newNN(index,DNA,inputs,world,true);
	//NN - end building
	//world->theNet->stopBuilding();

	//link create sub NNs
	std::vector<NeuralNode*> mainOutputs = world->theNet->getLastLayer();
	//NNLayerNode* falseLayer = new NNLayerNode(0);
	for(int i=0;i<(int)tempNeural->size();i++){
		//NeuralNetwork* subnet = new NeuralNetwork(mainOutputs);
		//world->subnets.push_back(subnet);
		newNN(tempNeural->at(i),DNA,mainOutputs,world,false);
		//NN(tempNeural->at(i),DNA,subnet,world,falseLayer,false);
		//subnet->stopBuilding();
	}
	//delete falseLayer;
	delete tempNeural;

	

	return Gsucces;
}

int B(int index, std::vector<int> *DNA, Physics *world, int *blocks, int part, std::vector<int>* tempNeural, partNode* body){
	if(*blocks >= maxBlocks){
		throw Gfail;
	}


	switch (getDNA(index,DNA)%6){
	case 0:
		index++;
		break;
	case 1:
		index++; //the information how many boxes are attached - belongs with the previous box
		index = J(index, DNA, world, blocks, part, tempNeural, body);

		break;
	case 2:
		index++; //the information how many boxes are attached - belongs with the previous box

		index = J(index, DNA, world, blocks, part, tempNeural, body);
		index = J(index, DNA, world, blocks, part, tempNeural, body);
		break;
	case 3:
		index++; //the information how many boxes are attached - belongs with the previous box

		index = J(index, DNA, world, blocks, part, tempNeural, body);
		index = J(index, DNA, world, blocks, part, tempNeural, body);
		index = J(index, DNA, world, blocks, part, tempNeural, body);
		break;
	case 4:
		index++; //the information how many boxes are attached - belongs with the previous box

		index = J(index, DNA, world, blocks, part, tempNeural, body);
		index = J(index, DNA, world, blocks, part, tempNeural, body);
		index = J(index, DNA, world, blocks, part, tempNeural, body);
		index = J(index, DNA, world, blocks, part, tempNeural, body);
		break;
	case 5:
		index++; //the information how many boxes are attached - belongs with the previous box

		index = J(index, DNA, world, blocks, part, tempNeural, body);
		index = J(index, DNA, world, blocks, part, tempNeural, body);
		index = J(index, DNA, world, blocks, part, tempNeural, body);
		index = J(index, DNA, world, blocks, part, tempNeural, body);
		index = J(index, DNA, world, blocks, part, tempNeural, body);
		break;

	default:
		throw Gfail;
	}
	body->setEnd(index);
	return index;
}

int J(int index, std::vector<int> *DNA, Physics *world, int *blocks, int part1, std::vector<int>* tempNeural, partNode* body){
	//NNLayerNode* aLayer = new NNLayerNode(index);
	//body->addChild(aLayer);
	//index = NN(index,DNA,body,aLayer);
	index = newNN(index,DNA,body);
	world->effectorNNindex.push_back(getDNA(index,DNA));
	world->effectorNNindex.push_back(getDNA(index+1,DNA));
	world->effectorNNindex.push_back(getDNA(index+2,DNA));
	index+=3;

	int part2 = world->createBox(getDNA(index+10,DNA), getDNA(index+11,DNA), getDNA(index+12,DNA));
	//partTreeBox begins here from index+10 (x,y,z)
	partNode* nextBody = new partNode(index+10);
	body->addChild(nextBody);
	
	int j_index = world->createJoint(part1, part2, getDNA(index,DNA), getDNA(index+1,DNA), getDNA(index+2,DNA), getDNA(index+3,DNA), getDNA(index+4,DNA), getDNA(index+5,DNA), getDNA(index+6,DNA), getDNA(index+7,DNA), getDNA(index+8,DNA), getDNA(index+9,DNA));
	index+=13;
	tempNeural->push_back(index);
	
	index = B(index, DNA, world, blocks, part2, tempNeural,nextBody); //nextBody will end at the end of this B() function
	return index;
}
/*
int NN(int index, std::vector<int> *DNA, NeuralNetwork* net, Physics *world, NNLayerNode* layer, bool notSubnet){
	NNNode* aNode;
	if(notSubnet){
		aNode = new NNNode(index);
		layer->addChild(aNode);
	}
	switch(getDNA(index,DNA)%3){
	case 0: //this is the last node.
		index= NI(index+1,DNA,net);
		if(notSubnet){
			aNode->setEnd(index);			//set end of current Node
			layer->setEnd(index);			//Since we are at the end, the layer also must end
		}
		break;
	case 1: //there is more nodes in this layer
		index=NI(index+1,DNA,net);
		if(notSubnet){
			aNode->setEnd(index);			//set end of current Node
		}
		index=NN(index,DNA,net,world,layer,notSubnet);		//use same NNLayerNode (since we are in the same layer)
		break;
	case 2: //there is another layer
		index=NI(index+1,DNA,net);
		NNLayerNode* newLayer;
		if(notSubnet){
			aNode->setEnd(index);			//set end of current Node
			layer->setEnd(index);			//set end of layer... since... we are changing layer ;)
			newLayer = new NNLayerNode(index);
			world->theTree->addNeuralNetworkLayer(newLayer);
		}
		net->changeLayer();
		index=NN(index,DNA,net,world,newLayer,notSubnet);	//create the new layer and parse it to the next NN function
		break;
	}
	return index;
}

int NN(int index, std::vector<int> *DNA,partNode* body,NNLayerNode* layer){
	NNNode* aNode = new NNNode(index);
	switch(getDNA(index,DNA)%3){
	case 0: //this is the last node.
		layer->addChild(aNode);
		index= NI(index+1, DNA);
		aNode->setEnd(index);
		layer->setEnd(index);
		break;
	case 1: //there is more nodes in this layer
		layer->addChild(aNode);
		index=NI(index+1, DNA);
		aNode->setEnd(index);
		index=NN(index, DNA,body,layer);
		break;
	case 2: //there is another layer
		layer->addChild(aNode);
		index=NI(index+1, DNA);
		aNode->setEnd(index);
		layer->setEnd(index);
		NNLayerNode* newLayer = new NNLayerNode(index);
		body->addChild(newLayer);
		index=NN(index, DNA,body,newLayer);
		break;
	}
	return index;
}
*/
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

int newNN(int index, std::vector<int>* DNA, std::vector<NeuralNode*> inputs, Physics *world, bool isNotSubnet){
	if(inputs.size()==0){inputs.push_back(new NeuralNode(0.f));} //default input if no sensors (subnets will allways have input.size>0)
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
		if(isNotSubnet){
			NNLayerNode* aLayerNode = new NNLayerNode(index);
			world->theTree->addNeuralNetworkLayer(aLayerNode);
			index = NNL(index,inputs.size(),DNA,aNet,aLayerNode,false);
			aLayerNode->setEnd(index);
		}else{
			NNLayerNode* aLayerNode = new NNLayerNode(index);
			index = NNL(index,SUBNET_NET_WIDTH,DNA,aNet,aLayerNode,false);
			delete aLayerNode;
		}
		aNet->changeLayer();
	}

	aNet->stopBuilding();
	return index;
}

//function for calculating subnet size
int newNN(int index, std::vector<int> *DNA,partNode* body){
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
		body->addChild(aLayerNode);
		index = NNL(index,SUBNET_NET_WIDTH,DNA,0,aLayerNode,true);
		aLayerNode->setEnd(index);
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
	
	for(int i=0;i<nrOfNodes;i++){
		NNNode* aNode = new NNNode(index);
		node->addChild(aNode);
		if(!isJustChecking){index = NI(index,DNA,aNet);} //insert the nodes
		else{index = NI(index,DNA);}
		aNode->setEnd(index);
	}
	return index;
}
