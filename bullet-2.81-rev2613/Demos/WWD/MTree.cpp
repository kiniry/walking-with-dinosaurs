#include "MTree.h"

std::vector<int> MTree::crossBreed(std::vector<int> myDNA, std::vector<int> seedDNA, MTree* seed){
	//30/70 chance of crossBreeding NN-main part or the body part
	bool chooser = rand()%10<7;
#ifdef NNONLY
	chooser=false;
#endif
	if(chooser){	//X-Breed body part
		int nrOfNodes = this->bodyPart->countNodes();
		//MTreeNode* theNode = this->bodyPart->findNode(rand()%nrOfNodes);	//equal chances for all boxes to be chosen
		MTreeNode* theNode = ReduceImpact(this->bodyPart->findNode(rand()%nrOfNodes),IMPACTPERCENTAGEBREEDPART);

		bool random = (rand()%10)<9;
		if(((partNode*)theNode)->NNChildren->size()==0){random = true;} //guard against invalid X-breeds

		if(random){	//90% chance of X-breeding the entire part
			//MTreeNode* theOtherNode = seed->bodyPart->findNode(rand()%seed->bodyPart->countNodes()); //find random whole body part from seed
			MTreeNode* theOtherNode = ReduceImpact(seed->bodyPart->findNode(rand()%seed->bodyPart->countNodes()),IMPACTPERCENTAGESEEDPART);
			myDNA.erase(myDNA.begin()+theNode->getStart(),myDNA.begin()+theNode->getEnd());
			myDNA.insert(myDNA.begin()+theNode->getStart(),seedDNA.begin()+theOtherNode->getStart(),seedDNA.begin()+theOtherNode->getEnd());

			if((myDNA.at(theNode->getStart()+3)%6)>0){
				partNode* falseBody = new partNode(0);
				int index = NN(theNode->getStart()+4,&myDNA,falseBody)+3+2;
				myDNA.at(index) += 180;
				delete falseBody;
			}

			return myDNA;
		}else{				//10% chance of X-breeding only the NN part
			MTreeNode* theOtherNode = getRandomBodyNode(seed);

			while(theOtherNode != 0 && ((partNode*)theOtherNode)->NNChildren->size()==0){ //Guard against invalid X-breeds
				theOtherNode = getRandomBodyNode(seed);
			}

			if(theOtherNode==0){ //then the main NN was chosen
				return NNcrossBreed(myDNA,seedDNA,((partNode*)theNode)->NNChildren,seed->NNPart);
			}
			return NNcrossBreed(myDNA,seedDNA,((partNode*)theNode)->NNChildren,((partNode*)theOtherNode)->NNChildren);
		}
	}else{				//X-Breed NN-main part
		MTreeNode* theOtherNode = getRandomBodyNode(seed);
#ifdef NNMAINONLY
		theOtherNode=0;
#endif
		while(theOtherNode != 0 && ((partNode*)theOtherNode)->NNChildren->size()==0){ //Guard against invalid X-breeds
			theOtherNode = getRandomBodyNode(seed);
		}
		if(theOtherNode==0){ //then the main NN was chosen
			return NNcrossBreed(myDNA,seedDNA,this->NNPart,seed->NNPart);
		}
		return NNcrossBreed(myDNA,seedDNA,this->NNPart,((partNode*)theOtherNode)->NNChildren);
	}
#ifdef _DEBUG
	printf("FAIL");
#endif
	return std::vector<int>();
}

inline MTreeNode* MTree::getRandomBodyNode(MTree* tree){
	int chosen = rand()%(tree->getBodyPart()->countNodes()+1); //the extra +1 is for choosing 0 (used for the special case)
	return tree->getBodyPart()->findNode(chosen);
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
		MTreeNode* aSeedNode = seedNode->at(seedChosenLayer)->findNode(seedChosen);

		myDNA.erase(myDNA.begin()+theNode->getStart(),myDNA.begin()+theNode->getEnd());
		myDNA.insert(myDNA.begin()+theNode->getStart()
			,seedDNA.begin()+aSeedNode->getStart(),seedDNA.begin()+aSeedNode->getEnd());
		return myDNA;
	}
#ifdef _DEBUG
	printf("error - NNbreeder!");
#endif
}

MTreeNode* MTree::ReduceImpact(MTreeNode* node,int choosePercentage){
	if(node->getChildren()->size()>0){
		int random = rand()%100;
		if(random<choosePercentage){
			MTreeNode* child = node->getChildren()->at(rand()%node->getChildren()->size());
			return ReduceImpact(child,choosePercentage-10);
		}
	}
	return node;
}