#include "DinoTreeNode.h"

DinoTreeNode::DinoTreeNode(int startIndex, bool isNeural)
{
	this->DNAindex=startIndex;
	this->isNeural=isNeural;
}

/**
* Calculates the size of the segment, given the first index after the segment
*/
void DinoTreeNode::setEnd(int index){
	this->segmentLength= (index - this->DNAindex);
}

void DinoTreeNode::addChild(DinoTreeNode* child){
	this->children.push_back(child);
}

DinoTreeNode::~DinoTreeNode(void)
{
}