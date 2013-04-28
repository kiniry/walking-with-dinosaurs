#pragma once
#include "pipe.h"


//server
static std::vector<LPCSTR> fullPipeNames;
static std::vector<HANDLE> pipes;


int pipeServerMain(int cores, int populationSize, int nrOfGenerations, std::vector<int> ancestor);

int setupServer(int cores);
void startPrograms(int cores);
int waitForClients(int cores);
int sendCreatures(std::vector<creature> creatures, int cores);
int sendOrders(int i);
void receiveAcknowledges();	