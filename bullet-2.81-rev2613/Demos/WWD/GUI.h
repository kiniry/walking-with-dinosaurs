#pragma once

#include <windows.h>

#include "Grammar.h"

#include "Server.h"
#include <ctime>
#include "Physics.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"

#include <io.h>
#include <fcntl.h>

#include "resource.h"

#define IDC_LISTBOX 993
#define IDC_SIM 994
#define IDC_RUN_BUTTON 995


static bool sOpenGLInitialized = false;
static int sWidth = 0;
static int sHeight =0;
static int quitRequest = 0;

#define	menuHeight 50
#define borders 16
int mainHeight, mainWidth;
int listHeight, listWidth;
int simHeight, simWidth;
int bAreaHeight, bAreaWidth;

HWND blank;

LPWSTR *argv;
int argc;

struct save{
	std::string name;
	std::vector<int> dna;
	float fitness;
};
std::vector<save*> saves; 
static Physics* WWDPhysics = new Physics();


// Function Declarations

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);


void console();
 void calcSizes(int height, int witdh);
void loadSaves();