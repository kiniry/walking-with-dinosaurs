#include "MTree.h"

std::vector<int> MTree::crossBreed(std::vector<int> myDNA, std::vector<int> seedDNA, MTree* seed){
	//30/70 chance of crossBreeding NN-main part or the body part
	if(rand()%10<7){	//X-Breed body part
		int nrOfNodes = this->bodyPart->countNodes();
		MTreeNode* theNode = this->bodyPart->findNode(rand()%nrOfNodes);	//equal chances for all boxes to be chosen

		if(rand()%10<9){	//90% chance of X-breeding the entire part
			MTreeNode* theOtherNode = seed->bodyPart->findNode(rand()%seed->bodyPart->countNodes()); //find random whole body part from seed
			myDNA.erase(myDNA.begin()+theNode->getStart(),myDNA.begin()+theNode->getEnd());
			myDNA.insert(myDNA.begin()+theNode->getStart(),seedDNA.begin()+theOtherNode->getStart(),seedDNA.begin()+theOtherNode->getEnd());
			return myDNA;
		}else{				//10% chance of X-breeding only the NN part
			int chosen = rand()%(seed->bodyPart->countNodes()+1); //the extra 1 is the main NN
			MTreeNode* theOtherNode = seed->bodyPart->findNode(chosen);
			if(theOtherNode==0){ //then the main NN was chosen
				return NNcrossBreed(myDNA,seedDNA,((partNode*)theNode)->NNChildren,seed->NNPart);
			}
			return NNcrossBreed(myDNA,seedDNA,((partNode*)theNode)->NNChildren,((partNode*)theOtherNode)->NNChildren);
		}
		
	}else{				//X-Breed NN-main part
		int chosen = rand()%(seed->bodyPart->countNodes()+1); //the extra 1 is the main NN
		MTreeNode* theOtherNode = seed->bodyPart->findNode(chosen);
		if(theOtherNode==0){ //then the main NN was chosen
			return NNcrossBreed(myDNA,seedDNA,this->NNPart,seed->NNPart);
		}
		return NNcrossBreed(myDNA,seedDNA,this->NNPart,((partNode*)theOtherNode)->NNChildren);
	}
	
	printf("FAIL");
	return std::vector<int>();
}

std::vector<int> MTree::NNcrossBreed(std::vector<int> myDNA, std::vector<int> seedDNA, std::vector<NNLayerNode*>* myNode, std::vector<NNLayerNode*>* seedNode){
	int myChosenLayer = rand()%myNode->size();
	int seedChosenLayer = rand()%seedNode->size();
	int myChosen = rand()%(myNode->at(myChosenLayer)->countNodes()+1); //+1 for the layer node itself
	MTreeNode* theNode = myNode->at(myChosenLayer)->findNode(myChosen);
	if(theNode==0){ //the layer itself is chosen
		myDNA.erase(myDNA.begin()+myNode->at(myChosenLayer)->getStart()
					,myDNA.begin()+myNode->at(myChosenLayer)->getEnd());

		myDNA.insert(myDNA.begin()+myNode->at(myChosenLayer)->getStart()
					,seedDNA.begin()+seedNode->at(seedChosenLayer)->getStart()
					,seedDNA.begin()+seedNode->at(seedChosenLayer)->getEnd());
		return myDNA;	
	}else{ //a node in the layer is chosen
		int seedChosen = rand()%(seedNode->at(seedChosenLayer)->countNodes());
		MTreeNode* seedNode = myNode->at(myChosenLayer)->findNode(myChosen);

		myDNA.erase(myDNA.begin()+theNode->getStart(),myDNA.begin()+theNode->getEnd());
		myDNA.insert(myDNA.begin()+theNode->getStart()
			,seedDNA.begin()+seedNode->getStart(),seedDNA.begin()+seedNode->getEnd());
	}

	printf("error - NNbreeder!");
}