#pragma once
#include "pipe.h"

//client
static std::string ID ="0";
static std::string fullPipeName;
static char filePathAbs[MAX_PATH];
static HANDLE pipe = INVALID_HANDLE_VALUE;

static std::string creatureFilePath;

int pipeClientMain(int argc,char** argv);
void setParameters(int argc,char** argv);
void setupClient();
std::vector<creature> getCreatures();
void pipeSim(std::vector<creature> creatures);
void sendResult(std::vector<creature> creatures);
void sendAcknowledge();
bool receiveOrders(); 