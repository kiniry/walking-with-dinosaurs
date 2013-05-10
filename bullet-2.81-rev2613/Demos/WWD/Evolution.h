/**
*	this files handels removing the creatures with lowest fitness score and adding new creatures based on the surving creatures.
*	this means the resulting batch of creatures will be a combination of surviors and children of the survivors
*
*	the correctness of the new dna belonging to the children are not check in this file.
*
**/

#ifndef EVOLV_H
#define EVOLV_H

#include <cstdlib>
#include <vector>
#include <algorithm>
#include "MTree.h"
#include <limits>

//results in a range  of values from
#define maxDNAValue 10000
#define dead -999999
#define randomDnaValue rand()%maxDNAValue

#define expectedDiviation 1.5

struct creature{
	std::vector<int> dna;
	float fitness;
	MTree* treePointer;
};

static double totalDiviation =0;
static double timesDiviation =0;

void normalizeFitness(std::vector<creature> creatures);


double statistik(std::vector<creature> creatures);

static bool compareCreatures(const creature &a, const creature &b){
	return a.fitness > b.fitness;
}
std::vector<creature> evolve(std::vector<creature> creatures);

std::vector<int> mutate(const std::vector<int>  dna, double deviation);

std::vector<int> crossOver1(std::vector<int> dna1, std::vector<int> dna2);

std::vector<int> crossOver2(std::vector<int> dna1, std::vector<int> dna2);

creature getWorthyCreature(float fitnessSum, std::vector<creature> creatures);

const int survivalRatio = 20; //how many elite creatures are there?
const int cullRatio = 50; //how many creatures must die?

#endif