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
#include "pipe.h"

const int populationSize = 100;
const int nrOfGenerations = 50;


static int numCores;
static std::vector<Physics*> worlds;


unsigned int __stdcall run(void* data);

int main(int argc,char** argv);

int debug(int argc,char** argv);

int WWD(int argc,char** argv);