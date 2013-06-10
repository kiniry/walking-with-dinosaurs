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
	

	//simulations window
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

	//selection listbox
	hWndList = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("listbox"), "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL|LBS_NOTIFY,0, 0, listWidth, listHeight, hWnd,  (HMENU)IDC_LISTBOX, GetModuleHandle(NULL), NULL);
	for(int i=0;i<(int)saves.size();i++){
		SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)saves.at(i)->name.c_str());
	}
	SendMessage(hWndList,LB_SETCURSEL,0,0);

	//init creature/world
	WWDPhysics = new Physics();
	readDNA(&saves.at(0)->dna,WWDPhysics);
	//WWDPhysics->solveGroundConflicts();
	//WWDPhysics->relaxCreature();
	WWDPhysics->runSimStartUp();
	WWDPhysics->reshape(simWidth,simHeight);



	//settings area
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
	WWDPhysics->setFitnesFunction(move);


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

	HWND hWndNoGS=CreateWindowEx(NULL,TEXT("EDIT"),	"10",WS_CHILD|WS_VISIBLE|ES_NUMBER,	col2, row2, 100, 18,
		hWnd, (HMENU)IDC_NOG_EDIT, GetModuleHandle(NULL),	NULL);

	HWND hWndPopS=CreateWindowEx(NULL,TEXT("STATIC"),	"Population",WS_CHILD|WS_VISIBLE,	col3, row1, 100, 18,
		hWnd, (HMENU)IDC_POP_STATIC, GetModuleHandle(NULL),	NULL);
	HWND hWndPop=CreateWindowEx(NULL,TEXT("EDIT"),	"10",WS_CHILD|WS_VISIBLE |ES_NUMBER,	col4, row1, 100, 18,
		hWnd, (HMENU)IDC_POP_EDIT, GetModuleHandle(NULL),	NULL);

	HWND hWndViewRate=CreateWindowEx(NULL,TEXT("STATIC"),	"Simulation view precision",WS_CHILD|WS_VISIBLE,	col3, row2, 100, 18,
		hWnd, (HMENU)IDC_VIEW_STATIC, GetModuleHandle(NULL),	NULL);

	HWND hWndViewRateB=CreateWindowEx(NULL,TEXT("BUTTON"),	"",WS_CHILD|WS_VISIBLE | BS_CHECKBOX,	col4, row2, 100, 18,
		hWnd, (HMENU)IDC_VIEW_CHECKBOX, GetModuleHandle(NULL),	NULL);
	
	MSG msg = messageLoop(hDC, hRC);
	return msg.wParam;
}

// program main loop
MSG messageLoop(	HDC hDC, HGLRC hRC){
	BOOL quit = FALSE;
	MSG msg;
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
		//glClearColor( .7f, 0.7f, 0.7f, 1.f );

		WWDPhysics->clientMoveAndDisplay(fixedSteps, hDC);


	}

	// shutdown OpenGL
	DisableOpenGL( blank, hDC, hRC );

	// destroy the window explicitly
	DestroyWindow( hWnd );

	delete WWDPhysics;

	for(int i =0;i<saves.size();i++){
		delete saves.at(i);
	}

	return msg;
}



//Main Window Procedure
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
					printf("off\n");
					break;

				case BST_UNCHECKED:
					SendMessage(check, BM_SETCHECK, BST_CHECKED,0);
					fixedSteps=true;
					printf("on\n");
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
						//WWDPhysics->solveGroundConflicts();
						//WWDPhysics->relaxCreature();
						WWDPhysics->runSimStartUp();
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
				//WWDPhysics->solveGroundConflicts();
				//WWDPhysics->relaxCreature();
				WWDPhysics->runSimStartUp();
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
						if(itemIndex==saves.size()){
							itemIndex--;
						}
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
		0,75,0,2,0,0,3,100,0,10,100,400,100,0,
		//leg2
		0,0,0,0,0,0,0,0,0,
		0,0,0,
		0,25,0,2,0,0,3,100,0,10,100,400,100,0,
		//Tail1
		0,0,0,0,0,0,0,0,0,
		0,0,0,
		0,0,0,5,0,0,0,100,100,5,150,150,400,1,
		//Tail2
		0,0,0,0,0,0,0,0,0,
		0,0,0,
		0,0,0,5,0,0,0,100,100,5,130,130,300,0,
		//UpperBody
		0,0,0,0,0,0,0,0,0,
		0,0,0,
		0,0,0,3,0,0,2,0,0,0,300,350,300,3,
		//Head
		0,0,0,0,0,0,0,0,0,
		0,0,0,
		0,0,0,3,0,0,2,20,20,20,230,230,230,0,
		//arm1
		0,0,0,0,0,0,0,0,0,
		0,0,0,
		0,25,40,0,0,0,5,50,50,50,70,70,200,0,
		//arm1
		0,0,0,0,0,0,0,0,0,
		0,0,0,
		0,75,40,0,0,0,5,50,50,50,70,70,200,0
	};
	int size = sizeof( temp ) / sizeof ( *temp );
	std::vector<int> ancestor (temp, temp+size);

	save* tmp = new save();
	tmp->dna=ancestor;
	tmp->name="DefDino";

	saves.push_back(tmp);

	int temp2[] = {1107, 36, 5999, 9613, 0, 2342, 5954, 9965, 3063, 9508, 1783, 4408, 8814, 8874, 5197, 5, 7127, 5284, 8075, 7537, 1152, 655, 5663, 5000, 637, 7899, 9037, 7478, 803, 1589, 3024, 8689, 9693, 0, 0, 0, 7922, 5945, 3641, 4413, 2411, 4193, 2637, 7123, 590, 8995, 141, 2187, 1957, 8900, 608, 9052, 1040, 5729, 3307, 7211, 8773, 2342, 5954, 9965, 4148, 3346, 1783, 4408, 8814, 8874, 5197, 5, 4911, 9471, 6404,
9195, 2220, 1955, 1702, 2487, 2517, 4956, 5311, 6171, 4159, 6692, 1828, 3497, 2597, 2790, 6595, 4859, 3725, 1660, 3823, 6188, 7837, 2341, 1023, 8055, 3961, 852,
 9805, 1620, 592, 1155, 1736, 8462, 3771, 5434, 7278, 4705, 3513, 5623, 8890, 2003, 7857, 8789, 5978, 6278, 4580, 3488, 7797, 8161, 4905, 889, 2737, 9319, 4869,
 7649, 4064, 5912, 8780, 9329, 1548, 9372, 9071, 5278, 5309, 2636, 9783, 3065, 6884, 4674, 4139, 8555, 6527, 996, 4496, 4429, 1107, 7116, 9745, 6030, 5826, 2139, 3542, 826, 2117, 9125, 7339, 7509, 2186, 5594, 5674, 796, 8753, 1548, 3347, 3782, 8085, 5626, 1647, 1597, 4822, 5272, 68, 666, 218, 923, 2330, 802, 107, 975,
5889, 8727, 4968, 2292, 4708, 2174, 9275, 0, 1254, 7650, 7279, 7678, 8820, 1067,
 138, 2538, 717, 3885, 1360, 602, 2068, 1734, 1377, 8382, 647, 6886, 0, 9328, 9053, 6047, 844, 2230, 9294, 7392, 7453, 2828, 1370, 4457, 6856, 6214, 4199, 8291,
 8975, 8312, 6576, 2477, 851, 1090, 4335, 8005, 8908, 9606, 8155, 447, 7181, 4369, 6050, 1498, 7569, 5985, 1708, 2739, 262, 3383, 8626, 590, 2492, 9937, 4195, 4275, 3703, 7423, 6484, 7126, 1655, 7376, 6409, 3776, 3374, 8493, 6281, 305, 8122, 604, 867, 455, 8635, 9483, 4285, 2486, 5942, 4943, 2425, 239, 8797, 9767, 2232, 5830, 6223, 8696, 1984, 4586, 9821, 8370, 3156, 1923, 993, 8622, 6577, 6490, 7626, 2856, 7435, 4976, 4979, 5470, 4998, 7214, 1320, 7950, 1302, 9630, 9561, 4130, 1200, 4869, 6258, 3726, 7305, 7488, 1187, 5524, 1468, 6891, 4224, 7581, 3811,
 7023, 8012, 6239, 2894, 5888, 4285, 6425, 175, 6080, 1266, 7217, 6833, 1141, 7909, 125, 3663, 6020, 8642, 1036, 2750, 2986, 8612, 4055, 3228, 2792, 8877, 7176,
 7191, 255, 5706, 6588, 9449, 9624, 6368, 3931, 6186, 6499, 3082, 4031, 8430, 318, 5954, 5472, 858, 9920, 3423, 7460, 6658, 261, 5738, 6148, 8778, 4936, 9000, 1620, 9709, 8082, 9896, 5092, 8631, 6252, 9365, 4596, 7639, 229, 2223, 4472, 4380, 3737, 5154, 4510, 6422, 2887, 2681, 2218, 2296, 4812, 4358, 4800, 2042, 2576,
7960, 7688, 4872, 4526, 2996, 5067, 3328, 5202, 1682, 1671, 1003, 741, 1148, 6061, 2388, 3035, 2028, 281, 915, 2328, 319, 5820, 2418, 1286, 5309, 7725, 1218, 1537, 5280, 4406, 6788, 1655, 9978, 693, 1444, 8727, 9483, 2293, 8288, 5528, 6567,
 3046, 7071, 8185, 2487, 7137, 6943, 9498, 1739};
	int size2 = sizeof( temp2 ) / sizeof ( *temp2 );
	std::vector<int> ancestor2 (temp2, temp2+size2);

	save* tmp2 = new save();
	tmp2->dna=ancestor2;
	tmp2->name="Superdupercreature";

	saves.push_back(tmp2);



	int temp3[] = {9189, 36, 1903, 9613, 0, 6726, 8814, 8894, 5197, 5587, 5018, 6726, 8814, 8894, 5197, 3305, 1156, 4859, 3725, 4191, 7636, 6188, 1367, 2273, 1023, 8874, 5197, 2039, 9370, 5284, 8075, 5945, 9096, 4413, 4474, 7537, 1152, 655, 5663, 9460, 637, 7899, 9037, 7478, 803, 1589, 6718, 8689, 9693, 4400, 35, 0, 7922, 5945, 9096, 4413, 4474, 6726, 8814, 8894, 5197, 3305, 1156, 8995, 141, 2187, 1957, 8900, 753, 2318, 1040, 6726, 8814, 8894, 5197, 5587, 1156, 6726, 8814, 8894, 5197, 6854, 1156, 8874, 5197, 5, 9370, 5284, 8075, 6726, 8814, 8894, 5197, 5587, 1156, 4859, 3725, 1660, 3823, 6188, 4371, 2273, 1023, 5945, 9096, 4413, 4474, 4859, 3725, 4191, 7636, 7388, 1367, 2273, 1023, 1433, 3725, 1660, 3823, 6188, 4371, 2273, 1023,
6726, 8814, 8894, 5580, 3305, 1156, 6726, 8814, 8894, 5197, 5, 1156, 6726, 8814,
 8894, 5197, 5, 1156, 6726, 1058, 636, 6048, 5413, 5578, 4859, 3725, 1660, 4209,
 6188, 1367, 2273, 1023, 8874, 5197, 8118, 7127, 5284, 8075, 6736, 9096, 4413, 2121, 4859, 3725, 1660, 2204, 5628, 1367, 877, 1023, 2342, 6048, 9965, 3063, 1337, 1783, 7065, 8814, 6587, 5197, 5, 7127, 8349, 8075, 9224, 1783, 713, 8814, 5902, 4719, 5, 9101, 5284, 8075, 4859, 3778, 4121, 3823, 6188, 7837, 2341, 6744, 2310, 6784, 1660, 3823, 6188, 7837, 279, 6744, 5623, 9246, 247, 8132, 1099, 1428, 6144, 4580, 380, 109, 8161, 4905, 4600, 2737, 211, 4869, 8152, 4064, 5912, 8780,
8599, 3867, 9925, 9071, 5278, 916, 2440, 7194, 8311, 6884, 5489, 4139, 73, 6527,
 1362, 3622, 66, 70, 1877, 9480, 6030, 5826, 3382, 3542, 7231, 6481, 8179, 2683,
 2172, 468, 5594, 5674, 9779, 9135, 6595, 9705, 3782, 8085, 5776, 1647, 5067, 4822, 9692, 3192, 3595, 7884, 6425, 6860, 802, 579, 7408, 5889, 4963, 7941, 4547,
4708, 9275, 9275, 8136, 1731, 2128, 95, 9127, 4756, 1067, 138, 1393, 717, 7433,
5604, 602, 3018, 8744, 1377, 8594, 6870, 3488, 3203, 6232, 9053, 6047, 844, 4229, 9294, 2864, 7453, 8672, 7977, 4457, 1450, 9226, 9246, 8291, 4888, 8312, 7275,
8732, 851, 4008, 7704, 8005, 4440, 7645, 8155, 447, 4057, 5035, 4355, 4423, 4349, 5382, 9013, 2739, 2509, 9060, 1368, 4565, 3752, 7874, 4195, 4275, 3416, 2542,
6484, 7126, 393, 1722, 1202, 104, 3374, 8493, 6281, 3228, 8122, 3098, 4374, 3820, 8635, 9483, 5985, 6687, 5568, 8121, 9966, 5614, 8797, 9767, 2232, 5830, 6223,
8696, 7229, 4586, 9821, 1010, 3156, 1923, 2221, 3966, 6577, 6490, 7626, 4531, 9053, 4976, 4979, 9149, 3675, 9061, 1320, 1714, 7700, 9630, 9561, 8297, 1200, 4869, 6258, 1997, 7305, 9027, 3348, 3279, 1468, 6629, 2282, 2674, 3811, 7023, 5455,
6239, 135, 5888, 3645, 2657, 7502, 1792, 1053, 6566, 8141, 4840, 607, 125, 3558,
 6020, 7190, 945, 2245, 2986, 8612, 1424, 4482, 9846, 8877, 5489, 7191, 9527, 8543, 9476, 9186, 9624, 6368, 4717, 6186, 6499, 3082, 4031, 5737, 318, 5954, 5472,
 9171, 9920, 1654, 6936, 6658, 261, 4771, 6148, 612, 4936, 3272, 8657, 9709, 9486, 9896, 5200, 8631, 1738, 7382, 4596, 7639, 229, 3928, 4472, 4380, 3737, 7474,6643, 6422, 2887, 2681, 2218, 2296, 527, 6812, 4800, 8854, 2576, 1175, 7688, 647, 4526, 3862, 2215, 6598, 5202, 1682, 730, 4147, 741, 9045, 9581, 712, 3740, 4328, 4958, 9484, 6506, 2160, 9947, 2418, 1286, 6133, 7725, 1218, 1537, 9696, 120,
6788, 9950, 9978, 95, 1444, 8727, 9483, 8502, 5690, 5528, 2769, 5582, 5879, 5454, 2487, 555, 2301, 3707, 9012};
	int size3 = sizeof( temp3 ) / sizeof ( *temp3 );
	std::vector<int> ancestor3 (temp3, temp3+size3);

	save* tmp3 = new save();
	tmp3->dna=ancestor3;
	tmp3->name="super evolved thingy";

	saves.push_back(tmp3);



	int temp4[] = {300, 350, 300};
	int size4 = sizeof( temp4 ) / sizeof ( *temp4 );
	std::vector<int> ancestor4 (temp4, temp4+size4);

	save* tmp4 = new save();
	tmp4->dna=ancestor4;
	tmp4->name="block";

	saves.push_back(tmp4);
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
		aStream <<proInfo->stats.killed/((float)proInfo->stats.population)*100.f<<"%";
		SetWindowText(killedText,aStream.str().c_str());

		aStream.str("");
		aStream <<proInfo->stats.deviation;
		SetWindowText(deviationText,aStream.str().c_str());

		if(proInfo->rounds>=noGenerations){
			ShowWindow(okButton,SW_SHOW);
		}
	}
}