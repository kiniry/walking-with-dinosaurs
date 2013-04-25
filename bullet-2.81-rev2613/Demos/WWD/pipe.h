#pragma once

#pragma comment(lib, "Shlwapi.lib")
#include <Shlwapi.h>

#include <Windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "Physics.h"
#include "Grammar.h"
#include "Evolution.h"

// The full name of the pipe in the format of \\servername\pipe\pipename.
#define SERVER_NAME         "."	//. means local server
#define PIPE_NAME           "WWD"
#define FULL_PIPE_NAME      "\\\\" SERVER_NAME "\\pipe\\" PIPE_NAME

//LPCSTR pipeId = "";

static HANDLE pipe = INVALID_HANDLE_VALUE;
static DWORD dwError = ERROR_SUCCESS;

#define BUFFER_SIZE     1024

 //server
int pipeServerMain(int cores, int populationSize, int nrOfGenerations, std::vector<int> ancestor);

int setupServer(int cores);
void startPrograms(int cores);
int waitForClients(int cores);
int sendCreatures(std::vector<creature> creatures, int cores);
int sendOrders(int i);
void receiveAcknowledges();

//client
int pipeClientMain(int argc,char** argv);

void setupClient();
std::vector<creature> getCreatures();
void pipeSim(std::vector<creature> creatures);
void sendResult(std::vector<creature> creatures);
void sendAcknowledge();
bool receiveOrders(); 