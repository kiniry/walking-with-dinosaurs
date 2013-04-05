#include <cstdlib>
#include <vector>

#ifndef EVOLV_H
#define EVOLV_H

#define maxDNAValue 2000

	struct creature{
		std::vector<int> dna;
		float fitness;
	};

std::vector<creature> evolv(const std::vector<creature> creatures);

std::vector<int> mutate(const std::vector<int>  dna);

std::vector<int> crossOver1(std::vector<int> dna1, std::vector<int> dna2);

std::vector<int> crossOver2(std::vector<int> dna1, std::vector<int> dna2);

int survivalRatio 20;



#endif