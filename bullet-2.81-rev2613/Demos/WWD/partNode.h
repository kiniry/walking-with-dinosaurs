#pragma once
#include "MTreeNode.h"
#include "NNLayerNode.h"

class partNode : public MTreeNode{
public: 
	std::vector<NNLayerNode*>* NNChildren;
	
	partNode(int start) :  MTreeNode(start){
		//startIndex = start;						//set in the other contructor
		//children = new std::vector<MTreeNode*>();	//set in the other contructor
		NNChildren = new std::vector<NNLayerNode*>();
	}
	partNode(const partNode& other) : MTreeNode(other){
		NNChildren = new std::vector<NNLayerNode*>();
		for(int i=0;i<other.NNChildren->size();i++){
			NNChildren->push_back(new NNLayerNode((const NNLayerNode&) (*other.NNChildren->at(i))));
		}
		children = new std::vector<MTreeNode*>();
		for(int i=0;i<other.children->size();i++){
			children->push_back(new partNode((const partNode&) *other.children->at(i)));
		}
	}
	virtual ~partNode(void){
		for(int i=0;i<NNChildren->size();i++){
			delete NNChildren->at(i);
		}
		delete NNChildren;
	}

	void addNNChild(NNLayerNode* aNode){
		NNChildren->push_back(aNode);
	}

	/*int countNodes(int prevIndex=-1){
		nodeIndex=prevIndex;
		int i=1;
		for(int j=0;j<children->size();j++){
			i+=children->at(j)->countNodes();
			nodeIndex=children->at(j)->nodeIndex;
		}
		for(int j=0;j<NNChildren->size();j++){
			i+=NNChildren->at(j)->countNodes();
		}
		return i;
	}*/

/*
	std::vector<int> mutate(){
		std::vector<int> result = std::vector<int>();
		NNLayerNode randomGenerator = NNLayerNode(0);

		result.push_back(rand()%maxDNAVal);//size.x
		result.push_back(rand()%maxDNAVal);//size.y
		result.push_back(rand()%maxDNAVal);//size.z
		
		int random = rand()%maxDNAVal;
		result.push_back(random);	//nr of boxes attached to this

		for(int i = 0; i<random%6; i++){
			std::vector<int> randNN = randomGenerator.mutate();
			result.insert(result.end(),randNN.begin(),randNN.end()); //randomized NN

			result.push_back(rand()%maxDNAVal); //effector index 1
			result.push_back(rand()%maxDNAVal); //effector index 2
			result.push_back(rand()%maxDNAVal); //effector index 3
			
			for(int j = 0; j<13; j++){
				result.push_back(rand()%maxDNAVal); //joint values + size values for the attached box
			}

			result.push_back(0); //attach no boxes to the attached box! averts the monstrous eternal probability recursion
		}

		return result;
	}
*/
};