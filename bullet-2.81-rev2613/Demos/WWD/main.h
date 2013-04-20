#pragma once

#include "omp.h"
#include "Physics.h"
#include "Grammar.h"
#include "Evolution.h"
#include "GlutStuff.h"
#include "test.h"
#include <ctime>

const int populationSize = 100;
const int nrOfGenerations = 10;


int main(int argc,char** argv);

int debug(int argc,char** argv);

int WWD(int argc,char** argv);