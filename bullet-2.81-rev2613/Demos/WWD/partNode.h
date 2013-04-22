#pragma once
#include "MTreeNode.h"
#include "NNLayerNode.h"

class partNode : MTreeNode{
public: 
	std::vector<int> mutate(){
		std::vector<int> result = std::vector<int>();
		NNLayerNode randomGenerator = NNLayerNode();

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
};