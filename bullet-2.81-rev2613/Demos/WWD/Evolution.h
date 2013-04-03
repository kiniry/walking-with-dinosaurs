#include <cstdlib>
#include <vector>

#ifndef EVOLV_H
#define EVOLV_H

#define maxDNAValue 2000

std::vector<int> evolv(const std::vector<int> dna);

std::vector<int> mutate(const std::vector<int>  dna);

std::vector<int> crossOver1(std::vector<int> dna1, std::vector<int> dna2);

std::vector<int> crossOver2(std::vector<int> dna1, std::vector<int> dna2);


#endif