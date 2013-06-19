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
	MTreeNode(const MTreeNode& other){
		startIndex=other.startIndex;
		endIndex=other.endIndex;
		//children = new std::vector<MTreeNode*>();
		//for(int i=0;i<other.children->size();i++){
		//	children->push_back(new MTreeNode((const MTreeNode&) *other.children->at(i)));
		//}
	}
	MTreeNode(int start,int end){
		startIndex=start;
		endIndex=end;
	}
	virtual ~MTreeNode(void){
		for(int i=0;i<children->size();i++){
			delete children->at(i);
		}
		delete children;
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