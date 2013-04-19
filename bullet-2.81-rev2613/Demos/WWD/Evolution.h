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

//results in a range  of values from
#define maxDNAValue 10000
#define randomDnaValue rand()%maxDNAValue

struct creature{
	std::vector<int> dna;
	float fitness;
};

static bool compareCreatures(const creature &a, const creature &b){
	return a.fitness > b.fitness;
}
std::vector<creature> evolve(std::vector<creature> creatures);

std::vector<int> mutate(const std::vector<int>  dna);

std::vector<int> crossOver1(std::vector<int> dna1, std::vector<int> dna2);

std::vector<int> crossOver2(std::vector<int> dna1, std::vector<int> dna2);

const int survivalRatio = 20;

#endif