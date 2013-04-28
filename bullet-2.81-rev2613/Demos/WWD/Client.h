#pragma once
#include "pipe.h"

//client
static std::string ID ="0";
static LPCSTR fullPipeName ="\\\\.\\pipe\\WWD0";
static char filePathAbs[MAX_PATH];
static HANDLE pipe = INVALID_HANDLE_VALUE;


int pipeClientMain(int argc,char** argv);
void setParameters(int argc,char** argv);
void setupClient();
std::vector<creature> getCreatures();
void pipeSim(std::vector<creature> creatures);
void sendResult(std::vector<creature> creatures);
void sendAcknowledge();
bool receiveOrders(); 