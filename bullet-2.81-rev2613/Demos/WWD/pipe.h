#pragma once

#pragma comment(lib, "Shlwapi.lib")
#include <Shlwapi.h>
#include <sstream>
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include "Physics.h"
#include "Grammar.h"
#include "Evolution.h"

// The full name of the pipe in the format of \\servername\pipe\pipename.
#define serverName         "."	//. means local server
#define pipeName           "WWD"
enum orders{stop, go};

static std::string directory ="";

static DWORD dwError = ERROR_SUCCESS;

#define BUFFER_SIZE     1024

std::vector<creature> getCreatures(std::string filename);

std::string getDirectory(); 