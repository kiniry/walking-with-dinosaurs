#pragma once

#include "omp.h"
#include "Physics.h"
#include "Grammar.h"
#include "Evolution.h"
#include "GlutStuff.h"
#include "test.h"
#include <ctime>
#include <Windows.h>
#include <process.h>
#include "server.h"
#include "client.h"

const int populationSize = 10;
const int nrOfGenerations = 2;


static int numCores;
static std::vector<Physics*> worlds;


unsigned int __stdcall run(void* data);

int main(int argc,char** argv);

int debug(int argc,char** argv);

int WWD(int argc,char** argv);