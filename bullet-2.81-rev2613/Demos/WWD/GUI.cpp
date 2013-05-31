#include "GUI.h"

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int cmdShow){
	argv =CommandLineToArgvW(GetCommandLineW(),&argc);

	directory = getDirectory();
	
#ifdef _DEBUG
	console();
#endif
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
	wc.hIcon = LoadIcon( GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1) );
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


	int row1=10,row2=35, row3 =60;
	int col1 =160,col2=330,col3=600,col4=700;
	HWND hWndFitS=CreateWindowEx(NULL,TEXT("STATIC"),	"Fitness Test",WS_CHILD|WS_VISIBLE,	col1, row1, 100, 18, hWnd, (HMENU)IDC_TEST_STATIC, GetModuleHandle(NULL),	NULL);
	HWND hwndCombo = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("combobox"), "", WS_CHILD | WS_VISIBLE| CBS_DROPDOWNLIST ,col2, row1-5, 100, 24, hWnd,  (HMENU)IDC_FITNESSTYPE_COMBOBOX, GetModuleHandle(NULL), NULL);
	
	SendMessage(hwndCombo,CB_ADDSTRING, 0, (LPARAM)"Move");
	SendMessage(hwndCombo,CB_SETITEMDATA, 0, move);

	SendMessage(hwndCombo,CB_ADDSTRING, 0, (LPARAM)"OLDMove");
	SendMessage(hwndCombo,CB_SETITEMDATA, 1, oldMove);
	
	SendMessage(hwndCombo,CB_ADDSTRING, 0, (LPARAM)"Jump");
	SendMessage(hwndCombo,CB_SETITEMDATA, 2, jump);

	SendMessage(hwndCombo,CB_ADDSTRING, 0, (LPARAM)"Combi");
	SendMessage(hwndCombo,CB_SETITEMDATA, 3, combi);
	
	SendMessage(hwndCombo,CB_ADDSTRING, 0, (LPARAM)"None");
	SendMessage(hwndCombo,CB_SETITEMDATA, 4, none);

	SendMessage(hwndCombo,CB_SETCURSEL,0,0);

	WWDPhysics = new Physics();

	//init creature
	readDNA(&saves.at(0)->dna,WWDPhysics);
	WWDPhysics->solveGroundConflicts();
	WWDPhysics->relaxCreature();
	WWDPhysics->reshape(simWidth,simHeight);


	HWND hWndButton=CreateWindowEx(NULL,TEXT("BUTTON"),	"RUN", WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		col1, row3, 100, 24,
		hWnd, (HMENU)IDC_RUN_BUTTON, GetModuleHandle(NULL),	NULL);

	HWND hWndResetButton=CreateWindowEx(NULL,TEXT("BUTTON"),	"Reset Simulation", WS_TABSTOP|WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
		col3, row3, 150, 24,
		hWnd, (HMENU)IDC_RESET_BUTTON, GetModuleHandle(NULL),	NULL);

	HGDIOBJ hfDefault=GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(hWndButton,WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE,0));

	HWND hWndNoG=CreateWindowEx(NULL,TEXT("STATIC"),"Number of Generations",WS_CHILD|WS_VISIBLE,	col1, row2, 150, 18,
		hWnd, (HMENU)IDC_NOG_STATIC, GetModuleHandle(NULL),	NULL);

	HWND hWndNoGS=CreateWindowEx(NULL,TEXT("EDIT"),	"10",WS_CHILD|WS_VISIBLE,	col2, row2, 100, 18,
		hWnd, (HMENU)IDC_NOG_EDIT, GetModuleHandle(NULL),	NULL);

	HWND hWndPopS=CreateWindowEx(NULL,TEXT("STATIC"),	"Population",WS_CHILD|WS_VISIBLE,	col3, row1, 100, 18,
		hWnd, (HMENU)IDC_POP_STATIC, GetModuleHandle(NULL),	NULL);
	HWND hWndPop=CreateWindowEx(NULL,TEXT("EDIT"),	"10",WS_CHILD|WS_VISIBLE |ES_NUMBER,	col4, row1, 100, 18,
		hWnd, (HMENU)IDC_POP_EDIT, GetModuleHandle(NULL),	NULL);

	HWND hWndViewRate=CreateWindowEx(NULL,TEXT("STATIC"),	"Simulation view precision",WS_CHILD|WS_VISIBLE,	col3, row2, 100, 18,
	hWnd, (HMENU)IDC_VIEW_STATIC, GetModuleHandle(NULL),	NULL);
	
	HWND hWndViewRateB=CreateWindowEx(NULL,TEXT("BUTTON"),	"",WS_CHILD|WS_VISIBLE | BS_CHECKBOX,	col4, row2, 100, 18,
	hWnd, (HMENU)IDC_VIEW_CHECKBOX, GetModuleHandle(NULL),	NULL);
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

		WWDPhysics->clientMoveAndDisplay(fixedSteps);

		SwapBuffers( hDC );

		theta += 1.0f;
	}

	// shutdown OpenGL
	DisableOpenGL( blank, hDC, hRC );

	// destroy the window explicitly
	DestroyWindow( hWnd );

	delete WWDPhysics;

	for(int i =0;i<saves.size();i++){
		delete saves.at(i);
	}

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

	case WM_CONTEXTMENU:
		{
			POINT p;
			GetCursorPos(&p);
			ScreenToClient(hWndList,&p);
			int index = SendMessage(hWndList,LB_ITEMFROMPOINT,0, MAKELPARAM(p.x,p.y));

			int noElements = SendMessage(hWndList,LB_GETCOUNT,0,0);
			if(0<=index && index <noElements){
				popupMenuSel=index;

				HMENU popupMenu = CreatePopupMenu();
				InsertMenu(popupMenu,0,MF_BYPOSITION|MF_STRING,IDC_RUN_MBUTTON,"Run");
				InsertMenu(popupMenu,0,MF_BYPOSITION|MF_STRING,IDC_RENAME_MBUTTON,"Rename");
				InsertMenu(popupMenu,0,MF_BYPOSITION|MF_STRING,IDC_DELETE_MBUTTON,"Delete");
				InsertMenu(popupMenu,0,MF_BYPOSITION|MF_STRING,IDC_SHOWDNA_MBUTTON,"Show DNA");
				GetCursorPos(&p);
				TrackPopupMenu(popupMenu,TPM_TOPALIGN|TPM_LEFTALIGN,p.x,p.y,0,hwnd,NULL);
			}

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

		case IDC_VIEW_CHECKBOX:
			{
			HWND check = GetDlgItem(hwnd, IDC_VIEW_CHECKBOX);
			switch (SendMessage(check, BM_GETCHECK, 0,0))
			{
			case BST_CHECKED:
				SendMessage(check, BM_SETCHECK, BST_UNCHECKED,0);
				fixedSteps=false;
			break;

			case BST_UNCHECKED:
				SendMessage(check, BM_SETCHECK, BST_CHECKED,0);
				fixedSteps=true;
				break;
			}			
			}
			break;


		case IDC_FITNESSTYPE_COMBOBOX:
			switch (HIWORD(wParam)){
				case CBN_SELCHANGE:
					HWND hwndfit = GetDlgItem(hwnd, IDC_FITNESSTYPE_COMBOBOX);
					int index = SendMessage(hwndfit, CB_GETCURSEL,0,0);
					fitnessTest tmptest=(fitnessTest) SendMessage(hwndfit,CB_GETITEMDATA, index,0);
					WWDPhysics->setFitnesFunction(tmptest);	
				break;

			}

		break;
	
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
						SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDC_FITNESSTYPE_COMBOBOX,LBN_SELCHANGE),0);
						readDNA(&saves.at(index)->dna,WWDPhysics);
						WWDPhysics->solveGroundConflicts();
						WWDPhysics->relaxCreature();
						WWDPhysics->reshape(simWidth,simHeight);

					}
				}
			}
			break;
		case IDC_RESET_BUTTON:
			{
				if(saves.size()>0){
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDC_LISTBOX,LBN_SELCHANGE),0);
				}
			}
			break;
		case IDC_RUN_BUTTON:
			{
				HWND hwndList = GetDlgItem(hwnd, IDC_LISTBOX);

				_ASSERTE(hwndList != NULL);

				// Get current selection index in listbox
				int itemIndex = (int) SendMessage(hwndList, LB_GETCURSEL, (WPARAM)0, (LPARAM) 0);
				if (itemIndex == LB_ERR)
				{
					// No selection
					return 0;
				}

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
					delete text;
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
					delete text;
				}else{
					MessageBox(NULL, "No number of generations selected", TEXT("ERROR"), MB_OK | MB_ICONERROR);
					return 0;
				}


				HWND hwndfit = GetDlgItem(hwnd, IDC_FITNESSTYPE_COMBOBOX);
				int index = SendMessage(hwndfit, CB_GETCURSEL,0,0);
				fitnessTest tmptest=(fitnessTest) SendMessage(hwndfit,CB_GETITEMDATA, index,0);

				noGenerations=noG;
				argumentList* aList = new argumentList();
				aList->nC=numCores;
				aList->p=pop;
				aList->nG=noG;
				aList->iI=itemIndex;
				aList->type=tmptest;
				aList->theResult = new creature();


				proInfo = new progressInfo();


				HANDLE threadHandle = (HANDLE) _beginthreadex(0,0,&runServer,(void*)aList,0,0);

				UINT_PTR time = SetTimer(0,0,10,(TIMERPROC)&update);
				DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PROGRESS), hwnd, progressControll,(LPARAM)&noG);
				KillTimer(0,time);

				save* tmpCreature =new save();
				tmpCreature->dna= aList->theResult->dna;
				tmpCreature->fitness=aList->theResult->fitness;

				delete proInfo;
				delete aList->theResult;
				delete aList;

				DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_NAMING), hwnd, namingControl,(LPARAM)&tmpCreature->name);

				saves.push_back(tmpCreature);
				SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)saves.at(saves.size()-1)->name.c_str());
				SendMessage(hWndList,LB_SETCURSEL,saves.size()-1,0);
				delete WWDPhysics;

				WWDPhysics = new Physics();
				WWDPhysics->setFitnesFunction(tmptest);
				readDNA(&saves.at(saves.size()-1)->dna,WWDPhysics);
				WWDPhysics->solveGroundConflicts();
				WWDPhysics->relaxCreature();
				WWDPhysics->reshape(simWidth,simHeight);
			}
			break;

		case IDC_RUN_MBUTTON:
			{
				HWND hwndList = GetDlgItem(hwnd, IDC_LISTBOX);
				SendMessage(hwndList,LB_SETCURSEL,popupMenuSel,0);

				SendMessage(hWnd,WM_COMMAND, MAKEWPARAM(IDC_RUN_BUTTON,0),0);
			}

			break;
		case IDC_RENAME_MBUTTON:
			{
				DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_NAMING), hwnd, namingControl,(LPARAM)&saves.at(popupMenuSel)->name);
				HWND hwndList = GetDlgItem(hwnd, IDC_LISTBOX);
				int itemIndex = (int) SendMessage(hwndList, LB_GETCURSEL, (WPARAM)0, (LPARAM) 0);
				for(int i =0; i<saves.size();i++){
					SendMessage(hwndList,LB_DELETESTRING,0,0);
				}
				for(int i =0; i<saves.size();i++){
					SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)saves.at(i)->name.c_str());
				}

				SendMessage(hwndList,LB_SETCURSEL,itemIndex,0);
			}
			break;
		case IDC_DELETE_MBUTTON:
			{
				HWND hwndList = GetDlgItem(hwnd, IDC_LISTBOX);
				int itemIndex = (int) SendMessage(hwndList, LB_GETCURSEL, (WPARAM)0, (LPARAM) 0);

				SendMessage(hwndList,LB_DELETESTRING,popupMenuSel,0);
				delete saves.at(popupMenuSel);
				saves.erase(saves.begin()+popupMenuSel,saves.begin()+popupMenuSel+1);

				if(itemIndex==popupMenuSel){
					if(saves.size() > 0){
						SendMessage(hwndList,LB_SETCURSEL,itemIndex,0);
						SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDC_LISTBOX,LBN_SELCHANGE),0);
					}else{
						delete WWDPhysics;
						WWDPhysics=new Physics();
						WWDPhysics->reshape(simWidth,simHeight);
					}
				}
			}
			break;
				case IDC_SHOWDNA_MBUTTON:
			{
				HWND hwndList = GetDlgItem(hwnd, IDC_LISTBOX);
				

				printf("DNA of %s\n",saves.at(popupMenuSel)->name.c_str());
				for(int i=0; i<saves.at(popupMenuSel)->dna.size();i++){
					printf("%d, ", saves.at(popupMenuSel)->dna.at(i));

				}
				printf("\n");
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
	int temp[] = {300,350,300,			4,//nrAttachedToMain
//leg1
0,0,0,0,0,0,0,0,0,
0,0,0,
1,75,0,2,0,0,3,100,0,10,100,400,100,0,
//leg2
0,0,0,0,0,0,0,0,0,
0,0,0,
1,25,0,2,0,0,3,100,0,10,100,400,100,0,
//Tail1
0,0,0,0,0,0,0,0,0,
0,0,0,
1,0,0,5,0,0,0,100,100,5,150,150,400,1,
//Tail2
0,0,0,0,0,0,0,0,0,
0,0,0,
1,0,0,5,0,0,0,100,100,5,130,130,300,0,
//UpperBody
0,0,0,0,0,0,0,0,0,
0,0,0,
1,0,0,3,0,0,2,0,0,0,300,350,300,3,
//Head
0,0,0,0,0,0,0,0,0,
0,0,0,
1,0,0,3,0,0,2,20,20,20,230,230,230,0,
//arm1
0,0,0,0,0,0,0,0,0,
0,0,0,
1,25,40,0,0,0,5,50,50,50,70,70,200,0,
//arm1
0,0,0,0,0,0,0,0,0,
0,0,0,
1,75,40,0,0,0,5,50,50,50,70,70,200,0
	};
	int size = sizeof( temp ) / sizeof ( *temp );
	std::vector<int> ancestor (temp, temp+size);

	save* tmp = new save();
	tmp->dna=ancestor;
	tmp->name="DefDino";

	saves.push_back(tmp);

	int temp2[] = {300, 350, 300, 2324, 4627, 1393, 457, 0, 0, 819, 0, 0, 0, 0, 5117, 5034, 1, 539,
 2160, 2, 9902, 0, 3, 100, 0, 4660, 388, 400, 100, 0, 0, 0, 0, 5316, 0, 8392, 7810, 0, 0, 0, 513, 3589, 1790, 25, 0, 5552, 0, 8695, 3, 9352, 1413, 10, 0, 6826,
77, 0, 0, 0, 4710, 0, 0, 2400, 3678, 1641, 8007, 0, 4650, 1098, 1801, 0, 7649, 6549, 7241, 0, 0, 2598, 100, 1728, 150, 150, 1, 25, 3278, 0, 0, 0, 2, 0, 0, 8693,
 1, 0, 0, 400, 100, 0, 0, 0, 4710, 0, 0, 418, 3158, 1526, 9940, 7744, 200, 6899,
 2289, 0, 724, 0, 0, 573, 0, 8815, 3264};
	int size2 = sizeof( temp2 ) / sizeof ( *temp2 );
	std::vector<int> ancestor2 (temp2, temp2+size2);

	save* tmp2 = new save();
	tmp2->dna=ancestor2;
	tmp2->name="highsomeSaurus";

	saves.push_back(tmp2);

	int temp3[] = {300, 350, 300};
	int size3 = sizeof( temp3 ) / sizeof ( *temp3 );
	std::vector<int> ancestor3 (temp3, temp3+size3);

	save* tmp3 = new save();
	tmp3->dna=ancestor3;
	tmp3->name="block";

	saves.push_back(tmp3);

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
					*result=text;
				delete text;
				}else{
					text = "new Creature";
					*result=text;
				}

				printf("name %s", result->c_str());
				EndDialog(hwnd,0);
				break;
			}
		case IDCANCEL:
			MessageBox(hwnd, "Bye!", "This is also a message",
				MB_OK | MB_ICONEXCLAMATION);
			TCHAR * text = "the Nameless One";
			*result=text;
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
		progressText = GetDlgItem(hwnd,IDC_STATIC_PROGRESS);
		maxText = GetDlgItem(hwnd,IDC_STATIC_MAX);
		meanText = GetDlgItem(hwnd,IDC_STATIC_MEAN);
		minText = GetDlgItem(hwnd,IDC_STATIC_MIN);
		medianText = GetDlgItem(hwnd,IDC_STATIC_MEDIAN);
		killedText = GetDlgItem(hwnd,IDC_STATIC_KILLED);
		deviationText = GetDlgItem(hwnd,IDC_STATIC_DEVIATION);
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

	*aList->theResult = pipeServerMain(aList->nC,aList->p,aList->nG,saves.at(aList->iI)->dna, aList->type,proInfo);

	return 1;
}

VOID CALLBACK update(){
	if(progress!=0){
		SendMessage(progress,PBM_SETPOS,proInfo->rounds,0);

		std::stringstream aStream;
		aStream <<"Current Progress: "<< proInfo->rounds<<" out of "<<noGenerations<<"\n";
		SetWindowText(progressText,aStream.str().c_str());

		aStream.str("");
		aStream <<proInfo->stats.min;
		SetWindowText(minText,aStream.str().c_str());


		aStream.str("");
		aStream << proInfo->stats.max;
		SetWindowText(maxText,aStream.str().c_str());
		
		aStream.str("");
		aStream <<proInfo->stats.mean;
		SetWindowText(meanText,aStream.str().c_str());
		
		aStream.str("");
		aStream <<proInfo->stats.median;
		SetWindowText(medianText,aStream.str().c_str());

		aStream.str("");
		aStream <<proInfo->stats.killed<<"%";
		SetWindowText(killedText,aStream.str().c_str());
		
		aStream.str("");
		aStream <<proInfo->stats.deviation;
		SetWindowText(deviationText,aStream.str().c_str());

		if(proInfo->rounds>=noGenerations){
			ShowWindow(okButton,SW_SHOW);
		}
	}
}