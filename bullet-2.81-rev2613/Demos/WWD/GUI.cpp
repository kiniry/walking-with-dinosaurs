#include "GUI.h"


// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, int cmdShow)
{

	argv =CommandLineToArgvW(GetCommandLineW(),&argc);


	console();


	loadSaves();

	WNDCLASSEX wc;
	HWND hWnd;
	HDC hDC;
	HGLRC hRC;
	MSG msg;
	BOOL quit = FALSE;
	float theta = 0.0f;



	// register window class
	//definere et vindues classe og dens parametre/udsende
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	//alt tab icon + windows linie icon
	wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	//look of the cursor
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	//background color
	wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
	//menu bar
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wc.lpszClassName = "main";
	//icon venstre top hjørne
	//følgende virker også, loader dog default 32x32 icon
	//LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wc.hIconSm		 = (HICON)LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0);
	if(!RegisterClassEx(&wc)){
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// create main window
	hWnd = CreateWindowEx( WS_EX_CLIENTEDGE,
		"main", "Walking With Dinosaurs", 
		WS_CAPTION | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		0, 0, 1024, 768,
		NULL, NULL, hInstance, NULL );
	// create main window

	// register window class
	wc.cbSize		 = sizeof(WNDCLASSEX);
	//cs_owndc svaes the cach, used for painting outside normal routine
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "blank";
	RegisterClassEx( &wc );

	int height =768-16-50;
	int width= 1024-16;
	blank = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("blank"), "", WS_CHILD | WS_VISIBLE ,150,100,width-150,height-100, hWnd,(HMENU)IDC_SIM, GetModuleHandle(NULL), NULL);

	EnableOpenGL( blank, &hDC, &hRC );



	HWND hWndList = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("listbox"), "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL|LBS_NOTIFY,0, 0, 150, height, hWnd,  (HMENU)IDC_LISTBOX, GetModuleHandle(NULL), NULL);
	for(int i=0;i<(int)saves.size();i++){
		SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)saves.at(i)->name.c_str());
	}
	SendMessage(hWndList,LB_SETCURSEL,0,0);

	WWDPhysics = new Physics();

	//init creature
	readDNA(&saves.at(0)->dna,WWDPhysics);
	WWDPhysics->solveGroundConflicts();
	WWDPhysics->reshape(0,0,sWidth,sHeight);





	//button
	// Create a push button
	HWND hWndButton=CreateWindowEx(NULL,TEXT("BUTTON"),	"RUN", WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		150,
		50,
		100,
		24,
		hWnd,
		(HMENU)IDC_RUN_BUTTON,
		GetModuleHandle(NULL),
		NULL);
	HGDIOBJ hfDefault=GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(hWndButton,WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));



	//ShowWindow(hWnd, cmdShow);
	//UpdateWindow(hWnd);

	// program main loop
	while ( !quit )
	{

		// check for messages
		if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE )  )
		{

			// handle or dispatch messages
			if ( msg.message == WM_QUIT ) 
			{
				quit = TRUE;
			} 
			else 
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}

			//WWDPhysics->displayCallback();


		};

		//OpenGL animation code goes here

		glClearColor( .7f, 0.7f, 0.7f, 1.f );

		WWDPhysics->clientMoveAndDisplay();


		SwapBuffers( hDC );

		theta += 1.0f;


	}



	// shutdown OpenGL
	DisableOpenGL( blank, hDC, hRC );

	// destroy the window explicitly
	DestroyWindow( hWnd );

	delete WWDPhysics;

	return msg.wParam;

}

// Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{



	switch (message)
	{

	case WM_SYSKEYDOWN:
		{
			if (lParam & 1<<29)
			{
				WWDPhysics->m_modifierKeys = VK_LMENU;
			}
			break;
		}
	case WM_SYSKEYUP:
		{
			if (lParam & 1<<29)
			{
				WWDPhysics->m_modifierKeys = VK_LMENU;
			} else
			{
				WWDPhysics->m_modifierKeys = 0;
			}

			break;
		}


	case WM_SIZE:													// Size Action Has Taken Place

		switch (wParam)												// Evaluate Size Action
		{
		case SIZE_MINIMIZED:									// Was Window Minimized?
			return 0;												// Return

		case SIZE_MAXIMIZED:									// Was Window Maximized?
			sWidth = LOWORD (lParam);
			sHeight = HIWORD (lParam);
			if (sOpenGLInitialized)
			{
				//TODO
				//WWDPhysics->reshape(0,0,sWidth,sHeight);
			}
			return 0;												// Return

			//resize
		case SIZE_RESTORED:										// Was Window Restored?
			sWidth = LOWORD (lParam);
			sHeight = HIWORD (lParam);
			if (sOpenGLInitialized)
			{	
				//TODO
				//WWDPhysics->reshape(0,0,sWidth,sHeight);
			}
			return 0;												// Return
		}
		break;	

	case WM_CREATE:
		{

		}

		return 0;

	case WM_MBUTTONUP:
		{
			int xPos = LOWORD(lParam); 
			int yPos = HIWORD(lParam); 
			WWDPhysics->mouseFunc(1,1,xPos,yPos);
			POINT p = POINT();
			p.x=xPos; p.y=yPos;
			SetFocus(ChildWindowFromPoint(hwnd,p));
			break;
		}
	case WM_MBUTTONDOWN:
		{
			int xPos = LOWORD(lParam); 
			int yPos = HIWORD(lParam); 
			WWDPhysics->mouseFunc(1,0,xPos,yPos);
			POINT p = POINT();
			p.x=xPos; p.y=yPos;
			SetFocus(ChildWindowFromPoint(hwnd,p));
			break;
		}

	case WM_LBUTTONUP:
		{
			int xPos = LOWORD(lParam); 
			int yPos = HIWORD(lParam); 
			WWDPhysics->mouseFunc(0,1,xPos,yPos);
			POINT p = POINT();
			p.x=xPos; p.y=yPos;
			SetFocus(ChildWindowFromPoint(hwnd,p));
			break;
		}
	case 0x020A://WM_MOUSEWHEEL:
		{

			int  zDelta = (short)HIWORD(wParam);
			int xPos = LOWORD(lParam); 
			int yPos = HIWORD(lParam); 
			if (zDelta>0)
				WWDPhysics->zoomIn();
			else
				WWDPhysics->zoomOut();
			break;
		}

	case WM_MOUSEMOVE:
		{
			int xPos = LOWORD(lParam); 
			int yPos = HIWORD(lParam); 
			WWDPhysics->mouseMotionFunc(xPos,yPos);
			break;
		}
	case WM_RBUTTONUP:
		{
			int xPos = LOWORD(lParam); 
			int yPos = HIWORD(lParam); 
			WWDPhysics->mouseFunc(2,1,xPos,yPos);
			POINT p = POINT();
			p.x=xPos; p.y=yPos;
			SetFocus(ChildWindowFromPoint(hwnd,p));
			break;
		}
	case WM_RBUTTONDOWN:
		{
			int xPos = LOWORD(lParam); 
			int yPos = HIWORD(lParam); 
			WWDPhysics->mouseFunc(2,0,xPos,yPos);
			POINT p = POINT();
			p.x=xPos; p.y=yPos;
			SetFocus(ChildWindowFromPoint(hwnd,p));
			break;
		}
	case WM_LBUTTONDOWN:
		{
			int xPos = LOWORD(lParam); 
			int yPos = HIWORD(lParam); 
			WWDPhysics->mouseFunc(0,0,xPos,yPos);
			POINT p = POINT();
			p.x=xPos; p.y=yPos;
			SetFocus(ChildWindowFromPoint(hwnd,p));
			break;
		}
		/*#define WM_LBUTTONUP                  0x0202
		#define WM_LBUTTONDBLCLK                0x0203
		#define WM_RBUTTONDOWN                  0x0204
		#define WM_RBUTTONUP                    0x0205
		#define WM_RBUTTONDBLCLK                0x0206
		#define WM_MBUTTONDOWN                  0x0207
		#define WM_MBUTTONUP                    0x0208
		#define WM_MBUTTONDBLCLK                0x0209
		*/



	case WM_CLOSE:
		PostQuitMessage( 0 );
		return 0;

	case WM_DESTROY:
		return 0;

	case WM_KEYUP:
		switch ( wParam )
		{		
		case VK_PRIOR:
		case VK_NEXT:
		case VK_END:
		case VK_HOME:
		case VK_LEFT:
		case VK_UP:			
		case VK_RIGHT:
		case VK_DOWN:
			{
				if (WWDPhysics)
					WWDPhysics->specialKeyboardUp(wParam,0,0);
				return 0;
			}
		default:
			{
				WWDPhysics->keyboardUpCallback(tolower(wParam),0,0);
			}
			return DefWindowProc( hwnd, message, wParam, lParam );
		}

	case WM_KEYDOWN:
		switch ( wParam )
		{
		case VK_CONTROL:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_END:
		case VK_HOME:
		case VK_LEFT:
		case VK_UP:
		case VK_RIGHT:
		case VK_DOWN:
			{
				if (WWDPhysics)
					WWDPhysics->specialKeyboard(wParam,0,0);
				break;
			}

		case ' ':
			{
				if (WWDPhysics)
					WWDPhysics->clientResetScene();
				break;
			}
		case 'Q':
		case VK_ESCAPE:
			{
				quitRequest = 1;
				PostQuitMessage(0);
			}
			return 0;

		}
		return 0;

	case WM_CHAR:
		if (!quitRequest)
			WWDPhysics->keyboardCallback(wParam,0,0);
		break;


	case WM_COMMAND:
		switch(LOWORD(wParam)){

		case IDC_LISTBOX:
			{

				switch (HIWORD(wParam)) 
				{ 	  
				case LBN_SELCHANGE:
					{
						HWND hwndList = GetDlgItem(hwnd, IDC_LISTBOX); 

						// Get selected index.
						int index = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0); 

						delete WWDPhysics;

						WWDPhysics = new Physics();

						readDNA(&saves.at(0)->dna,WWDPhysics);
						WWDPhysics->solveGroundConflicts();
						WWDPhysics->reshape(0,0,sWidth,sHeight);



						// Get length of text in listbox
						int textLen = (int) SendMessage(hwndList, LB_GETTEXTLEN, (WPARAM) index, 0);

						// Allocate buffer to store text (consider +1 for end of string)
						TCHAR * textBuffer = new TCHAR[textLen + 1];

						// Get actual text in buffer
						SendMessage(hwndList, LB_GETTEXT, (WPARAM) index, (LPARAM) textBuffer );

						// Show it
						MessageBox(NULL, textBuffer, TEXT("Selected Creature:"), MB_OK);

						// Free text
						delete [] textBuffer;

						// Avoid dangling references
						textBuffer = NULL; 

						return TRUE; 
					} 
				}
			}
			break;
		case IDC_RUN_BUTTON:
			{
				HWND hwndList1 = GetDlgItem(hwnd, IDC_LISTBOX);

				_ASSERTE(hwndList1 != NULL);

				// Get current selection index in listbox
				int itemIndex = (int) SendMessage(hwndList1, LB_GETCURSEL, (WPARAM)0, (LPARAM) 0);
				if (itemIndex == LB_ERR)
				{
					// No selection
					return 0;
				}


				//seeds random generator
				srand(time(0));

				//threads
				SYSTEM_INFO sysinfo;
				GetSystemInfo( &sysinfo );

				int numCores= sysinfo.dwNumberOfProcessors;
				//int numCores=1;
				int pop =100;
				int noG = 10;
				pipeServerMain(numCores,pop,noG,saves.at(itemIndex)->dna);


			}
			break;
		default:
			printf("");

		}
		break;

	default:
		return DefWindowProc( hwnd, message, wParam, lParam );

	}
	return 0;
}


// Enable OpenGL
void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC)
{
	PIXELFORMATDESCRIPTOR pfd;
	int format;

	// get the device context (DC)
	*hDC = GetDC( hWnd );

	// set the pixel format for the DC
	ZeroMemory( &pfd, sizeof( pfd ) );
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.cStencilBits = 1;
	pfd.iLayerType = PFD_MAIN_PLANE;
	format = ChoosePixelFormat( *hDC, &pfd );
	SetPixelFormat( *hDC, format, &pfd );

	// create and enable the render context (RC)
	*hRC = wglCreateContext( *hDC );
	wglMakeCurrent( *hDC, *hRC );
	sOpenGLInitialized = true;


}

// Disable OpenGL
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
	sOpenGLInitialized = false;

	wglMakeCurrent( NULL, NULL );
	wglDeleteContext( hRC );
	ReleaseDC( hWnd, hDC );
}


void loadSaves(){

	int temp[] = {1387,38,23,2,1924};
	int size = sizeof( temp ) / sizeof ( *temp );
	std::vector<int> ancestor (temp, temp+size);

	save* tmp = new save();
	tmp->dna=ancestor;
	tmp->name="test string";

	saves.push_back(tmp);

	int temp2[] = {1387,38,23,2,1924};
	int size2 = sizeof( temp2 ) / sizeof ( *temp2 );
	std::vector<int> ancestor2 (temp2, temp2+size2);

	save* tmp2 = new save();
	tmp2->dna=ancestor2;
	tmp2->name="hula hula";

	saves.push_back(tmp2);
}

void console(){

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

}