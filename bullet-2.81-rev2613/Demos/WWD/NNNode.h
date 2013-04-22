#pragma once
#include "MTreeNode.h"

class NNNode : MTreeNode{
public: 
	std::vector<int> mutate(){
		std::vector<int> result = std::vector<int>();
		
		result.push_back(rand()%maxDNAVal); //last node/ more node / change layer - variable
		
		int random = rand()%maxDNAVal;
		result.push_back(random); //nr of inputs - variable

		switch(random%4){
			case 0:
				result.push_back(rand()%maxDNAVal); // constant input
				break;
			case 1:
				result.push_back(rand()%maxDNAVal); //function type
				result.push_back(rand()%maxDNAVal); //input1
				result.push_back(rand()%maxDNAVal); //input1 weight
				break;
			case 2:
				result.push_back(rand()%maxDNAVal); //function type
				result.push_back(rand()%maxDNAVal); //input1
				result.push_back(rand()%maxDNAVal); //input1 weight
				result.push_back(rand()%maxDNAVal); //input2
				result.push_back(rand()%maxDNAVal); //input2 weight
				break;
			case 3:
				result.push_back(rand()%maxDNAVal); //function type
				result.push_back(rand()%maxDNAVal); //input1
				result.push_back(rand()%maxDNAVal); //input1 weight
				result.push_back(rand()%maxDNAVal); //input2
				result.push_back(rand()%maxDNAVal); //input2 weight
				result.push_back(rand()%maxDNAVal); //input3
				result.push_back(rand()%maxDNAVal); //input3 weight
				break;
			}
		return result;
	}
};
