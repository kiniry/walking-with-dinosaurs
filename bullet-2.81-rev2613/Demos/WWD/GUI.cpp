#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "main.h"
#include "resource.h"

const char g_szClassName[] = "myWindowClass";

//samme som action listener
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_CLOSE:
		//called when close windows pressed (x)

		//sends message WM_destroy
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;


	case WM_LBUTTONDOWN:
		//left mouse button

		{
			char FileName[MAX_PATH];
			HINSTANCE hInstance = GetModuleHandle(NULL);

			GetModuleFileName(hInstance, FileName, MAX_PATH);
			MessageBox(hwnd, FileName, "This program is:", MB_OK | MB_ICONINFORMATION);

		}
		break;


	case WM_RBUTTONDOWN:
		//right mouse button

		break;
	case WM_MBUTTONDOWN:
		//middel mouse button

		break;
	case WM_COMMAND:
		//menu
		switch(LOWORD(wParam)){

		case ID_FILE_NEW40002:
			  MessageBox(NULL, "Narf!", "Pinky says...", MB_OK | MB_ICONEXCLAMATION);
		break;

		case ID_FILE_EXIT:
			//post returnere med det samme Send venter på beskeden er behandlet
			//SendMessage(hwnd, WM_CLOSE, 0, 0);
			PostMessage(hwnd, WM_CLOSE, 0, 0); 
		break;
		}

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){

	//det icon med lavest defineret id i resource.h er .exe icon

	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	//definere en vindues classe og dens parametre/udsende
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = 0;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	//alt tab icon + windows linie icon
	wc.hIcon		 = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	//look of the cursor
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	//background color
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	//menu bar
	wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU1);
	
	wc.lpszClassName = g_szClassName;
	//icon venstre top hjørne
	//følgende virker også, loader dog default 32x32 icon
	//LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wc.hIconSm		 = (HICON)LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0);
					   
	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	//opretter vinduet
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"Walking With Dinosaurs",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	//The Message Loop
	//"gui tråden"
	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}

int WINAPI WinMain2(HINSTANCE hInst, HINSTANCE hPrev, LPSTR pszCmdLine, int iCmdShow)
{

	// console borrowed from http://justcheckingonall.wordpress.com/2008/08/29/console-window-win32-app/
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
	myargv[0] = _strdup ("something");

	//old main
	main(myargc, myargv);
	return 0;
}