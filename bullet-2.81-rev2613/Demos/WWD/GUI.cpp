#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "main.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR pszCmdLine, int iCmdShow)
{



	// console
	AllocConsole();

    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;

    HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
    hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
    FILE* hf_in = _fdopen(hCrt, "r");
    setvbuf(hf_in, NULL, _IONBF, 128);
    *stdin = *hf_in;



	MessageBox(NULL, "Narf!", "Pinky says...", MB_OK | MB_ICONEXCLAMATION);
	

	//dummy input
	char *myargv [1];
	int myargc =1;
	myargv[0] = strdup ("something");

	//old main
	main(myargc, myargv);
	return 0;
}