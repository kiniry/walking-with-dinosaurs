#pragma once
#include <vector>
class DinoTreeNode
{
public:
	bool isNeural;
	int DNAindex;
	int segmentLength;
	std::vector<DinoTreeNode*> children;
	void setEnd(int index);
	void addChild(DinoTreeNode* child);
	DinoTreeNode(int startIndex, bool isNeural);
	~DinoTreeNode(void);
};
