#include "GUI.h"

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int cmdShow){
	argv =CommandLineToArgvW(GetCommandLineW(),&argc);

	directory = getDirectory();

	console();

	loadSaves();
	int height=768,width =1024;
	calcSizes(768-menuHeight,1024-border);

	WNDCLASSEX wc;
	
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
	wc.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_3DFACE);
	//menu bar
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wc.lpszClassName = "main";
	//icon venstre top hjørne
	//følgende virker også, loader dog default 32x32 icon
	//LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wc.hIconSm		 = (HICON)LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0);
	if(!RegisterClassEx(&wc)){
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// create main window
	hWnd = CreateWindowEx( WS_EX_CLIENTEDGE,
		"main", "Walking With Dinosaurs",
		WS_THICKFRAME | WS_CAPTION | WS_VISIBLE | WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX |WS_CLIPCHILDREN |WS_MAXIMIZEBOX,
		GetSystemMetrics(SM_CXMAXIMIZED)/2-SM_CXFIXEDFRAME/2-width/2-listWidth, GetSystemMetrics(SM_CYMAXIMIZED)/2-SM_CYFIXEDFRAME/2-height/2, 1024, 768,
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
	if(!RegisterClassEx(&wc)){
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}



	blank = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("blank"), "", WS_CHILD | WS_VISIBLE ,listWidth,bAreaHeight,simWidth,simHeight, hWnd,(HMENU)IDC_SIM, GetModuleHandle(NULL), NULL);

	EnableOpenGL( blank, &hDC, &hRC );

	//listbox
	hWndList = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("listbox"), "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL|LBS_NOTIFY,0, 0, listWidth, listHeight, hWnd,  (HMENU)IDC_LISTBOX, GetModuleHandle(NULL), NULL);
	for(int i=0;i<(int)saves.size();i++){
		SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)saves.at(i)->name.c_str());
	}
	SendMessage(hWndList,LB_SETCURSEL,0,0);

	WWDPhysics = new Physics();

	//init creature
	readDNA(&saves.at(0)->dna,WWDPhysics);
	WWDPhysics->solveGroundConflicts();
	WWDPhysics->reshape(simWidth,simHeight);

	//button
	// Create a push button
	HWND hWndButton=CreateWindowEx(NULL,TEXT("BUTTON"),	"RUN", WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		150, 50, 100, 24,
		hWnd, (HMENU)IDC_RUN_BUTTON, GetModuleHandle(NULL),	NULL);
	HGDIOBJ hfDefault=GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(hWndButton,WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));

	HWND hWndNoG=CreateWindowEx(NULL,TEXT("STATIC"),"NoG",WS_CHILD|WS_VISIBLE,	150, 20, 100, 18,
		hWnd, (HMENU)IDC_NOG_STATIC, GetModuleHandle(NULL),	NULL);

	HWND hWndNoGS=CreateWindowEx(NULL,TEXT("EDIT"),	"10",WS_CHILD|WS_VISIBLE,	250, 20, 100, 18,
		hWnd, (HMENU)IDC_NOG_EDIT, GetModuleHandle(NULL),	NULL);

	HWND hWndPopS=CreateWindowEx(NULL,TEXT("STATIC"),	"Population",WS_CHILD|WS_VISIBLE,	600, 20, 100, 18,
		hWnd, (HMENU)IDC_POP_STATIC, GetModuleHandle(NULL),	NULL);
	HWND hWndPop=CreateWindowEx(NULL,TEXT("EDIT"),	"10",WS_CHILD|WS_VISIBLE |ES_NUMBER,	700, 20, 100, 18,
		hWnd, (HMENU)IDC_POP_EDIT, GetModuleHandle(NULL),	NULL);

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
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	
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

		switch (wParam)											// Evaluate Size Action
		{
		case SIZE_MINIMIZED:									// Was Window Minimized?
			return 0;												// Return

		case SIZE_MAXIMIZED:
			{									// Was Window Maximized?
				int Width = LOWORD (lParam);
				int Height = HIWORD (lParam);
				calcSizes(HIWORD (lParam),LOWORD (lParam));

				MoveWindow(hWndList,0,0,listWidth,listHeight,true);
				MoveWindow(blank,listWidth,bAreaHeight,simWidth,simHeight,true);

				if (sOpenGLInitialized)
				{
					WWDPhysics->reshape(simWidth,simHeight);
				}
			}
			return 0;												// Return

			//resize
		case SIZE_RESTORED:// Was Window Restored?
			if(hwnd == hWnd) 
			{
			int Width = LOWORD (lParam);
			int Height = HIWORD (lParam);
			calcSizes(HIWORD (lParam),LOWORD (lParam));

			MoveWindow(hWndList,0,0,listWidth,listHeight,true);
			MoveWindow(blank,listWidth,bAreaHeight,simWidth,simHeight,true);

			if (sOpenGLInitialized){
				WWDPhysics->reshape(simWidth,simHeight);
			}
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

	case WM_CONTEXTMENU:{
		POINT p;
		HMENU popupMenu = CreatePopupMenu();
		InsertMenu(popupMenu,0,MF_BYPOSITION|MF_STRING,IDC_RUN_BUTTON,"Run");
		GetCursorPos(&p);
		TrackPopupMenu(popupMenu,TPM_BOTTOMALIGN|TPM_LEFTALIGN,p.x,p.y,0,hwnd,NULL);

		break;}

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
				switch (HIWORD(wParam)){
				case LBN_SELCHANGE:
					{
						HWND hwndList = GetDlgItem(hwnd, IDC_LISTBOX);

						// Get selected index.
						int index = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);

						delete WWDPhysics;

						WWDPhysics = new Physics();

						readDNA(&saves.at(index)->dna,WWDPhysics);
						WWDPhysics->solveGroundConflicts();
						WWDPhysics->reshape(simWidth,simHeight);

						// Get length of text in listbox
						int textLen = (int) SendMessage(hwndList, LB_GETTEXTLEN, (WPARAM) index, 0);

						// Allocate buffer to store text (consider +1 for end of string)
						TCHAR * textBuffer = new TCHAR[textLen + 1];

						// Get actual text in buffer
						SendMessage(hwndList, LB_GETTEXT, (WPARAM) index, (LPARAM) textBuffer );

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

				int pop =0;
				HWND hwndPop = GetDlgItem(hwnd, IDC_POP_EDIT);

				_ASSERTE(hwndPop != NULL);

				int length=0;

				length = GetWindowTextLength(hwndPop);

				if(length > 0){
					TCHAR * text = new TCHAR[length + 1];
					GetWindowText(hwndPop,text,length+1);
					for(int i =0; i<length;i++){
						int value = ((int)text[i])-48;
						int res =  (value* pow(10.,length-1-i)+0.5);
						pop+=res;
					}
				}else{
					MessageBox(NULL, "No populastion size selected", TEXT("ERROR"), MB_OK | MB_ICONERROR);
					return 0;
				}
				if(pop<10){
				 MessageBox(NULL, "the Populasion size is to small", TEXT("ERROR"), MB_OK | MB_ICONERROR);
					return 0;
				}


				int noG = 0;
				HWND hwndNoG = GetDlgItem(hwnd, IDC_NOG_EDIT);

				_ASSERTE(hwndNoG != NULL);

				length=0;

				length = GetWindowTextLength(hwndNoG);
				if(length > 0){
					TCHAR * text = new TCHAR[length + 1];
					GetWindowText(hwndNoG,text,length+1);
					for(int i =0; i<length;i++){
						int value = ((int)text[i])-48;
						int res =  (value* pow(10.,length-1-i)+0.5);
						noG+=res;
					}
				}else{
					MessageBox(NULL, "No number of generations selected", TEXT("ERROR"), MB_OK | MB_ICONERROR);
					return 0;
				}

				noGenerations=noG;
				argumentList* aList = new argumentList();
				aList->nC=numCores;
				aList->p=pop;
				aList->nG=noG;
				aList->iI=itemIndex;
				aList->theResult = new creature();
				roundCount = new int;*roundCount=0;

				HANDLE threadHandle = (HANDLE) _beginthreadex(0,0,&runServer,(void*)aList,0,0);
				
				UINT_PTR time = SetTimer(0,0,10,(TIMERPROC)&update);
				DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PROGRESS), hwnd, progressControll,(LPARAM)&noG);
				KillTimer(0,time);


				save* tmpCreature =new save();
				tmpCreature->dna= aList->theResult->dna;
				tmpCreature->fitness=aList->theResult->fitness;

				delete roundCount;
				delete aList->theResult;
				delete aList;

				DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_NAMING), hwnd, namingControl,(LPARAM)&tmpCreature->name);

				printf("name %s\n", tmpCreature->name.c_str());

				saves.push_back(tmpCreature);
				SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)saves.at(saves.size()-1)->name.c_str());
				SendMessage(hWndList,LB_SETCURSEL,saves.size()-1,0);
				delete WWDPhysics;

				WWDPhysics = new Physics();

				readDNA(&saves.at(saves.size()-1)->dna,WWDPhysics);
				WWDPhysics->solveGroundConflicts();
				WWDPhysics->reshape(simWidth,simHeight);

			}
			break;

		case ID_FILE_SAVE40003:

			saveSaves(saves);
			break;

		case ID_FILE_EXIT:
			PostQuitMessage( 0 );

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
void EnableOpenGL(HWND hWnd, HDC * hDC, HGLRC * hRC){
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
	std::stringstream filename;
	filename << directory<< "GenPool.dino";

	std::ifstream is;
	is.open(filename.str(),std::ios::in | std::ios::binary);

	if(!is.good()){
		is.close();
		MessageBox(NULL, "Error 404: file not found\nDefault value will be used", TEXT("Failed to load saves"), MB_OK | MB_ICONERROR);
		loadDefault();
		return;
	}

	int noCreatures;
	is.read((char*)&noCreatures, sizeof(int));

	for(int i=0;i<noCreatures;i++){
		save* tmpCreature =new save();

		//read name
		int nameLength;
		is.read((char*)&nameLength, sizeof(int));
		tmpCreature->name.resize(nameLength);
		is.read((char*)&tmpCreature->name[0], sizeof(char)*nameLength);

		//read dna
		int sizeOfDna;
		is.read((char*)&sizeOfDna, sizeof(int));
		tmpCreature->dna.resize(sizeOfDna);
		is.read((char*)&tmpCreature->dna[0], sizeof(int)*sizeOfDna);

		//read fitness
		is.read((char*)&tmpCreature->fitness, sizeof(float));
		saves.push_back(tmpCreature);
	}
	if(!is.good()){
		MessageBox(NULL, "the loading failed and a default value will be read", TEXT("Failed to load saves"), MB_OK | MB_ICONERROR);
		loadDefault();
	}
	is.close();
}

void loadDefault(){
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

void saveSaves(std::vector<save*> saves){
	printf("Sending results\n");

	std::stringstream filename;
	filename << directory<< "GenPool.dino";
	std::ofstream os;
	os.open(filename.str(),std::ios::out | std::ios::binary);

	int noCreatures =saves.size();

	os.write((const char*)&noCreatures, sizeof(int));

	for(int j =0; j< (int)saves.size();j++){
		int nameLenght = saves.at(j)->name.length();
		os.write((const char*)&nameLenght, sizeof(int));
		os.write((const char*)&saves.at(j)->name[0], sizeof(char)*nameLenght);

		int size = saves.at(j)->dna.size();
		os.write((const char*)&size, sizeof(int));

		os.write((const char*)&saves.at(j)->dna[0], sizeof(int)*size);
		os.write((const char*)&saves.at(j)->fitness, sizeof(float));
	}
	if(!os.good()){
		printf("error\n");
		exit(-1);
	}
	os.flush();
	os.close();
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

void calcSizes(int height, int witdh){
	mainHeight=height;
	mainWidth= witdh;

	listWidth=150;
	listHeight=mainHeight+10;

	bAreaHeight=100;
	bAreaWidth=mainWidth-listWidth;

	simHeight=mainHeight-bAreaHeight;
	simWidth=mainWidth-listWidth;
}

BOOL CALLBACK namingControl(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam){
	static std::string* result;
	switch(Message){
	case WM_INITDIALOG:
		result= (std::string*) lParam;

		break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				HWND hwndPop = GetDlgItem(hwnd, IDC_NAMING_EDIT);

				_ASSERTE(hwndPop != NULL);

				int length=0;

				length = GetWindowTextLength(hwndPop);
				TCHAR * text;
				if(length > 0){
					text = new TCHAR[length + 1];
					GetWindowText(hwndPop,text,length+1);
				}else{
					text = "new Creature";
				}
				*result=text;
				printf("name %s", result->c_str());
				EndDialog(hwnd,0);
				break;
			}
		case IDCANCEL:
			MessageBox(hwnd, "Bye!", "This is also a message",
				MB_OK | MB_ICONEXCLAMATION);
			EndDialog(hwnd, 0);
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

BOOL CALLBACK progressControll(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam){
	switch(Message){
	case WM_INITDIALOG:{
		int nrGens =*((int*)lParam);
		//printf("thevalue %d\n",nrGens); //works
		progress = GetDlgItem(hwnd,IDC_PROGRESSBAR);
		okButton = GetDlgItem(hwnd,IDOK);
		progressText = GetDlgItem(hwnd,IDC_STATIC2);
		SendMessage(progress, PBM_SETRANGE, 0, MAKELPARAM(0, nrGens));
		break;}
	case WM_COMMAND:{
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				EndDialog(hwnd,0);
				break;
			}
		}
		break;
		}
	case WM_CLOSE:{
		printf("hell no :)");
		break;}
	default:
		//SendMessage(progress,PBM_SETPOS,*roundCount,0);
		return FALSE;
	}
	return TRUE;
}
unsigned int _stdcall runServer(void* args){
	
	argumentList* aList = (argumentList*) args;

	*aList->theResult = pipeServerMain(aList->nC,aList->p,aList->nG,saves.at(aList->iI)->dna,roundCount);
	
	return 1;
}

VOID CALLBACK update(){
	if(progress!=0){
		SendMessage(progress,PBM_SETPOS,*roundCount,0);

		std::stringstream aStream;
		aStream <<"Current Progress: "<< *roundCount<<" out of "<<noGenerations;
		SetWindowText(progressText,aStream.str().c_str());
		if(*roundCount>=noGenerations){
			ShowWindow(okButton,SW_SHOW);
		}
	}
}