#pragma once
#ifndef SERVERSET
#define SERVERSET
#include "pipe.h"
#include <commctrl.h>
#include <ctime>
//server
static std::vector<std::string> fullPipeNames;
static std::vector<HANDLE> pipes;
static std::vector<std::string> creatureFilePaths;

struct progressInfo{
	statistic stats;
	float rounds;
};

creature pipeServerMain(int cores, int populationSize, int nrOfGenerations, std::vector<int> ancestor, fitnessTest type, progressInfo* progressInfo, BOOLEAN* stopSim);

int setupServer(int pop, int cores);
void startPrograms(fitnessTest);
int waitForClients();
void sendCreatures(std::vector<creature>* creatures, int start);
int sendOrders(int i);
void receiveAcknowledges();
void getResults(std::vector<creature>* creatures, int start);
void cleanUp();

#endif