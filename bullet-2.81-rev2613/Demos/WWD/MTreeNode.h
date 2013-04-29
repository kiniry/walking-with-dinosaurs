#pragma once
#include <vector>

#define maxDNAVal 10000 //TODO:? get from shared/global var instead

class MTreeNode{
protected:	
	int startIndex;
	int endIndex;
	std::vector<MTreeNode*>* children;
public:
	int nodeIndex;
	MTreeNode(int start){
		startIndex=start;
		children = new std::vector<MTreeNode*>();
	}
	MTreeNode(int start,int end){
		startIndex=start;
		endIndex=end;
	}
	void setEnd(int end){
		endIndex=end;
	}
	int getStart(){return startIndex;}
	int getEnd(){return endIndex;}
	void addChild(MTreeNode* child){
		children->push_back(child);
	}
	std::vector<MTreeNode*>* getChildren(){
		return children;
	}
	virtual int countNodes(int prevIndex=-1){
		nodeIndex=prevIndex;
		int i=1;
		for(int j=0;j<children->size();j++){
			i+=children->at(j)->countNodes(nodeIndex);
			nodeIndex=children->at(j)->nodeIndex;
		}
		nodeIndex++;
		return i;
	}
	virtual MTreeNode* findNode(int nodeIndex){
		if(this->nodeIndex==nodeIndex){return this;}
		for(int i=0;i<this->children->size();i++){
			MTreeNode* temp = children->at(i)->findNode(nodeIndex);
			if(temp!=0){return temp;}
		}
		return 0;
	}

	//virtual std::vector<int> mutate()=0;
};