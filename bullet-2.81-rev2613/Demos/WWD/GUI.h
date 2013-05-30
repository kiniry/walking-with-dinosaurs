#pragma once

//magic "enable windows visual style" command
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include "Visual Leak Detector\include\vld.h"

#include "Grammar.h"

#include "Server.h"
#include <ctime>
#include "Physics.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"

#include <io.h>
#include <fcntl.h>

#include "resource.h"

#include <process.h>

#define IDC_SHOWDNA_MBUTTON 986
#define IDC_TEST_STATIC 987
#define IDC_RESET_BUTTON 988
#define IDC_FITNESSTYPE_COMBOBOX 989
#define IDC_RUN_MBUTTON 990
#define IDC_RENAME_MBUTTON 991
#define IDC_DELETE_MBUTTON 992
#define IDC_LISTBOX 993
#define IDC_SIM 994
#define IDC_RUN_BUTTON 995
#define IDC_NOG_EDIT 996
#define IDC_NOG_STATIC 997
#define IDC_POP_EDIT 998
#define IDC_POP_STATIC 999

static bool sOpenGLInitialized = false;
static int quitRequest = 0;

#define	menuHeight 62
#define border 20
int mainHeight, mainWidth;
int listHeight, listWidth;
int simHeight, simWidth;
int bAreaHeight, bAreaWidth;

HWND blank;
HWND hWndList;
HWND hWnd;
LPWSTR *argv;
int argc;

HWND progress;
HWND okButton;
HWND progressText;
HWND maxText;
HWND minText;
HWND meanText;
HWND medianText;
HWND deviationText;
HWND killedText;
int noGenerations;
VOID CALLBACK update();
struct argumentList{
	int nC;
	int p;
	int nG;
	int iI;
	fitnessTest type;
	creature* theResult;
};


struct save{
	std::string name;
	std::vector<int> dna;
	float fitness;
};
std::vector<save*> saves;
static Physics* WWDPhysics;
progressInfo* proInfo;
static int popupMenuSel =0;

// Function Declarations

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK namingControl(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK progressControll(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
unsigned int _stdcall runServer(void* args);
void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);

void console();
void calcSizes(int height, int witdh);
void loadSaves();
void loadDefault();
void saveSaves(std::vector<save*> saves);