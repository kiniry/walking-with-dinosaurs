/**
*	this files handels removing the creatures with lowest fitness score and adding new creatures based on the surving creatures.
*	this means the resulting batch of creatures will be a combination of surviors and children of the survivors
*
*	the correctness of the new dna belonging to the children are not check in this file.
*
**/

#pragma once

#include <cstdlib>
#include <vector>
#include <algorithm>
#include "MTree.h"
#include <limits>

//results in a range  of values from
#define maxDNAValue 10000
#define dead -999999
//rand() span 0-32767
#define randomDnaValue rand()%maxDNAValue

#define expectedDiviation 30

struct creature{
	std::vector<int> dna;
	float fitness;
	MTree* treePointer;
};

struct statistic{
	float max;
	float min;
	float median;
	float mean;
	float deviation;
	int killed;
	int population;
};
static double totalDiviation =0;
static double timesDiviation =0;
static int survivors;
static int cullAmount;

void initEvolution(int noCreatures);
std::vector<float>* normalizeFitness(std::vector<creature> *creatures, statistic* stats);

double statistik(std::vector<creature>* creatures, statistic* stats);

static bool compareCreatures(const creature &a, const creature &b){
	return a.fitness > b.fitness;
}
//std::vector<creature> evolve(std::vector<creature>* creatures);
statistic evolve(std::vector<creature>* creatures);

std::vector<int> mutate(const std::vector<int>  dna, double deviation);

std::vector<int> crossOver1(std::vector<int> dna1, std::vector<int> dna2);

std::vector<int> crossOver2(std::vector<int> dna1, std::vector<int> dna2);

creature getWorthyCreature(float fitnessSum, std::vector<creature> *creatures);

const int survivalRatio = 20; //how many procent elite creatures are there?
const int cullRatio = 50; //how many procent creatures must die?
