#include "Client.h"

#define _UNICODE

#define IDR_CONNECT 1
#define IDR_OPTIONS 2
#define IDR_EXIT 	3

#define IDC_EDITWRITE 		4
#define IDC_EDITSHOW 		5
#define IDC_EDITCLIENTS 	6
#define IDC_EDITSERVERADDR 	7
#define IDC_EDITNICKNAME 	8
#define IDC_BUTTONOK 		9
#define IDC_BUTTONCANCEL 	10

#define SMALL_ICON 			L"resources//ICON.ico"
#define WELCOME_SOUND 		L"resources//welcome.wav"
#define BACKGROUND_IMG		L"resources//BACKGROUND.bmp"
#define DISCONNECTED_SOUND  L"resources//disconnected.wav"
#define CONNECTED_SOUND 	L"resources//connected.wav"

#define TIMER_ID 999

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK NewEditWriteProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DialogProcConnect(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DialogProcOptions(HWND, UINT, WPARAM, LPARAM);

HWND AddDialogBox(HWND, const wchar_t*, const wchar_t*, int, int, int, int);
void AddDialogConnectControllers(HWND);
ATOM RegisterDialogBox(HWND, WNDPROC, const wchar_t*);

void AddMenuBar(HWND);
void AddMainControllers(HWND);

HWND hDialogConnect;
HWND hDialogOptions;

HWND hEditWrite;
HWND hEditDisplay;
//HWND hEditShowClients;

HWND hDialogEditNickname;
HWND hDialogEditServer;
HWND hButtonOk;
HWND hButtonCancel;

WNDPROC prevEditProc;
//Client: Intializing
static Client mainClient;

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPWSTR cmd, int mode)
{
	HWND hWnd;
	MSG msg;
	WNDCLASSW wc;
	
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.hIcon = (HICON)LoadImageW(NULL, SMALL_ICON, IMAGE_ICON,
								 0, 0, LR_LOADFROMFILE);
	wc.hbrBackground = 0;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInst;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = L"GeneralWindow";
	
	RegisterClassW(&wc);
	
	hWnd = CreateWindowW(L"GeneralWindow", L"Network chat", 
	WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW | WS_VISIBLE,
	CW_USEDEFAULT, CW_USEDEFAULT, 500, 600, 
	NULL, NULL, 0, NULL);
	UpdateWindow(hWnd);
	
	while(GetMessageW(&msg, 0, 0, 0))
	{
		if(!IsDialogMessage(GetAncestor(msg.hwnd, GA_ROOT), &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	
	return 0;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int xwin, ywin;
	static bool flag = TRUE;
	static HBITMAP hBitmap;
	HDC hDc;
	static HDC hDcCompatible;
	PAINTSTRUCT Ps;
	static BITMAP Bitmap;
	
	switch(message)
	{
		case WM_CREATE:
			//play welcome sound
			PlaySound(WELCOME_SOUND,
					  NULL,
					  SND_ASYNC | SND_FILENAME);
			//Set a timer to receive messages from server
			SetTimer(hWnd, TIMER_ID, 700, NULL);
			
			hBitmap = (HBITMAP)LoadImageW(NULL, BACKGROUND_IMG,
										IMAGE_BITMAP,
										0, 0,
										LR_LOADFROMFILE | LR_CREATEDIBSECTION);
				GetObject(hBitmap, sizeof(Bitmap), &Bitmap);
				hDc = GetDC(hWnd);
				hDcCompatible = CreateCompatibleDC(hDc);
				SelectObject(hDcCompatible, hBitmap);
				ReleaseDC(hWnd, hDc);
			
			AddMainControllers(hWnd);
			SendMessageW(hEditWrite,
						 EM_SETLIMITTEXT,
						 (WPARAM)500,
						 (LPARAM)0);
			
			AddMenuBar(hWnd);
			//register dialog boxes (Connect and Options)
			RegisterDialogBox(hWnd, DialogProcConnect, L"DialogConnect");
			RegisterDialogBox(hWnd, DialogProcOptions, L"DialogOptions");
			
			//changing the hEditWrite procedure 
			//for the treatment of pressing Enter key
			prevEditProc = (WNDPROC)GetWindowLongPtr(hEditWrite, GWLP_WNDPROC);
			SetWindowLongPtr(hEditWrite, 
				GWLP_WNDPROC, (LPARAM)NewEditWriteProc);
			
			break;
		case WM_TIMER:
			//Client: receive messages from server 
			if(mainClient.is_connected())
			{
				
				SendMessageW(hEditDisplay, EM_SETSEL, 0, -1);
				SendMessageW(hEditDisplay, EM_SETSEL, -1, 0);
				
				SendMessageW(hEditDisplay,
							 EM_REPLACESEL, 
							 0, 
							 (LPARAM)mainClient.RecvMessageTCP());
			}
			break;
		
		case WM_PAINT:
			hDc = BeginPaint(hWnd, &Ps);
				
				StretchBlt(hDc, 0, 0, xwin, ywin, 
					hDcCompatible, 0, 0, Bitmap.bmWidth,
					Bitmap.bmHeight, SRCCOPY);
			
			EndPaint(hWnd, &Ps);
			
			UpdateWindow(hEditWrite);
			UpdateWindow(hEditDisplay);
			//UpdateWindow(hEditShowClients);
			break;
		
		case WM_SIZE:
			xwin = LOWORD(lParam);
			ywin = HIWORD(lParam);
			
			MoveWindow(hEditWrite, 0, ywin - ywin/8, 
						xwin, ywin/8, TRUE);
			MoveWindow(hEditDisplay, 0, ywin - 3 * (ywin/8),
						xwin, (ywin/5), TRUE);
			/*MoveWindow(hEditShowClients, 0, 5,
						xwin, (ywin/2), TRUE);*/
			break;
		
		case WM_COMMAND:
		
			if(LOWORD(wParam) == IDC_EDITWRITE 
				&& HIWORD(wParam) == EN_SETFOCUS
				&& flag == TRUE)
			{
				SetWindowTextW(hEditWrite, (LPCWSTR)NULL);
				flag = FALSE;
			}
		
			switch(LOWORD(wParam))
			{
				case IDR_CONNECT: 
					hDialogConnect = AddDialogBox(hWnd,
												  L"DialogConnect",
												  L"Connect",
												  100, 100, 240, 190);
					break;
				
				case IDR_OPTIONS:
					hDialogOptions = AddDialogBox(hWnd,
												  L"DialogOptions",
												  L"Options",
												  100, 100, 350, 250);
					break;
				
				case IDR_EXIT:
					DestroyWindow(hWnd);
					break;
			}
			break;
		
		case WM_CTLCOLORSTATIC:
			//change the background color of the "ES_READONLY" Edit's 
			HDC hEditColor;
			hEditColor = (HDC)wParam;
			SetTextColor(hEditColor, RGB(0, 0, 0));
			SetBkColor(hEditColor, RGB(255, 255, 255));
			return (INT_PTR)GetStockObject(WHITE_BRUSH);
			break;
		
		case WM_DESTROY:
			//play disconnected sound
			PlaySound(DISCONNECTED_SOUND, NULL, SND_SYNC | SND_FILENAME);
			
			KillTimer(hWnd, TIMER_ID);
			DeleteObject(hBitmap);
			DeleteDC(hDcCompatible);
			SetWindowLongPtr(hEditWrite, GWLP_WNDPROC, (LPARAM)prevEditProc);
			PostQuitMessage(0);
			break;
		
		default: return DefWindowProcW(hWnd, message, wParam, lParam);
	}
	
	return 0;
}

LRESULT CALLBACK NewEditWriteProc(HWND hWnd, UINT msg, WPARAM wP, LPARAM lP)
{	
	
	if(msg == WM_CHAR && wP == VK_RETURN)
	{
		if(mainClient.is_connected())
		{
			//Initializing a buffer to send a message to the server
			wchar_t textbuf[TEXT_BUFFER];
			
			GetWindowTextW(hEditWrite, textbuf, sizeof(textbuf));
			
			SendMessageW(hEditDisplay, EM_SETSEL, 0, -1);
			SendMessageW(hEditDisplay, EM_SETSEL, -1, 0);
			SendMessageW(hEditDisplay,
						 EM_REPLACESEL,
						 0, 
						 (LPARAM)mainClient.GetNickname());
			SendMessageW(hEditDisplay, EM_REPLACESEL, 0, (LPARAM)textbuf);
			SendMessageW(hEditDisplay, EM_REPLACESEL, true, (LPARAM)L"\r\n");
			//Client: sending the text message to the server
			mainClient.SendMessageTCP(textbuf);
		
		}	
			
		SendMessageW(hEditWrite, WM_SETTEXT, 0, (LPARAM)L"");
		
		return 0;
	}
			
	return CallWindowProc(prevEditProc, hWnd, msg, wP, lP);
}

LRESULT CALLBACK DialogProcConnect(HWND hWnd,
								   UINT msg,
								   WPARAM wParam, 
								   LPARAM lParam)
{
	
	switch(msg)
	{
		case WM_CREATE:
		
			AddDialogConnectControllers(hWnd);
			SendMessageW(hDialogEditNickname,
						 EM_SETLIMITTEXT,
						 (WPARAM)29,
						 (LPARAM)0);
			SendMessageW(hDialogEditServer,
						 EM_SETLIMITTEXT,
						 (WPARAM)29,
						 (LPARAM)0);
			break;
			
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_BUTTONOK:
				//Client: get an addr and a Nickname for connection 
					char addr[30];
					wchar_t nick[30];
					
					GetWindowTextA(hDialogEditServer, addr, sizeof(addr));
					GetWindowTextW(hDialogEditNickname, nick, sizeof(nick));
					
					mainClient.Connect(addr, nick);
					
					if(mainClient.is_connected())
					{
						mainClient.StartThreadUDP();
						PlaySound(CONNECTED_SOUND,
								  NULL,
								  SND_ASYNC | SND_FILENAME);
						SendMessageW(hEditDisplay,
								 WM_SETTEXT,
								 0,
								 (LPARAM)L"Connected\r\n");
					}
					
					
					
					DestroyWindow(hWnd);
					break;
				case IDC_BUTTONCANCEL:
					DestroyWindow(hWnd);
					break;
			}
			break;
		case WM_DESTROY:
			DestroyWindow(hWnd);
			break;
		case WM_CTLCOLORSTATIC:
			//change a background color of "STATIC" controllers
			HDC hEditColor;
			hEditColor = (HDC)wParam;
			SetTextColor(hEditColor, RGB(0, 0, 0));
			SetBkColor(hEditColor, RGB(255, 255, 255));
			return (INT_PTR)GetStockObject(WHITE_BRUSH);
			break;
		default: return DefWindowProcW(hWnd, msg, wParam, lParam);
	}
	
	return 0;
}

LRESULT CALLBACK DialogProcOptions(HWND hWnd,
								   UINT msg,
								   WPARAM wParam, 
								   LPARAM lParam)
{
	switch(msg)
	{
		case WM_CREATE:
		
			break;
			
		case WM_COMMAND:
			
			break;
		case WM_DESTROY:
			DestroyWindow(hWnd);
			break;
		default: return DefWindowProcW(hWnd, msg, wParam, lParam);
	}
	
	return 0;
}

HWND AddDialogBox(HWND hWnd,
				  const wchar_t* lpClassName,
				  const wchar_t* lpWindowName,
				  int x, 
				  int y,
				  int nWidth,
				  int nHeight)
{
	return CreateWindowExW(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,
		lpClassName, lpWindowName, 
		WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN,
		x, y, nWidth, nHeight,
		NULL, NULL, 
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),  NULL);
}

void AddDialogConnectControllers(HWND hWnd)
{
	::hDialogEditServer = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", NULL, 
		WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | WS_TABSTOP,
		20, 30, 160, 20,
		hWnd, (HMENU)IDC_EDITSERVERADDR, 
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
	::hDialogEditNickname = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", NULL,
		WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | WS_TABSTOP,
		20, 90, 160, 20,
		hWnd, (HMENU)IDC_EDITNICKNAME, 
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
	
	CreateWindowW(L"STATIC",  L"Server address:", 
		WS_CHILD | WS_VISIBLE,
		20, 10, 160, 20,
		hWnd, NULL, 
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
	CreateWindowW(L"STATIC",  L"Your nickname:", 
		WS_CHILD | WS_VISIBLE,
		20, 70, 160, 20,
		hWnd, NULL, 
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
	
	::hButtonOk = CreateWindowW(L"BUTTON", L"OK", 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 
		100, 120, 40, 30,
		hWnd, (HMENU) IDC_BUTTONOK, 
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
	::hButtonCancel = CreateWindowW(L"BUTTON", L"Cancel", 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 
		150, 120, 60, 30,
		hWnd, (HMENU) IDC_BUTTONCANCEL, 
		(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
}

ATOM RegisterDialogBox(HWND hWnd, 
					   WNDPROC lpfnWndProc,
					   const wchar_t* lpszClassName)
{
	WNDCLASSW wc;
	
	wc.style = 0;
	wc.cbClsExtra = 0;
	wc.hIcon = NULL;
	wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	wc.lpfnWndProc = lpfnWndProc;
	wc.lpszClassName = lpszClassName;
	
	return RegisterClassW(&wc);
}

void AddMenuBar(HWND hWnd)
{
	HMENU hMenu;
	HMENU hMenuContent;
	
	if(((hMenu = CreateMenu()) == NULL) || 
		((hMenuContent = CreateMenu()) == NULL))
	{
		MessageBoxW(hWnd, L"CreateMenu ERROR", L"Error info", MB_OK);
		return;
	}
	
	AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hMenuContent, L"Menu");             
	AppendMenuW(hMenuContent, MF_STRING, IDR_CONNECT, L"Connect");
	AppendMenuW(hMenuContent, MF_STRING, IDR_OPTIONS, L"Options");
	AppendMenuW(hMenuContent, MF_STRING, IDR_EXIT, L"Exit");
	
	SetMenu(hWnd, hMenu);
}

void AddMainControllers(HWND hWnd)
{
	::hEditWrite = CreateWindowW(L"EDIT", L"Enter your message...", 
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | 
				ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
				0, 0, 0, 0,
				hWnd, (HMENU)IDC_EDITWRITE, 
				(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
	::hEditDisplay = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", NULL, 
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | 
				ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
				0, 0, 0, 0,
				hWnd, (HMENU)IDC_EDITSHOW, 
				(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
	/*::hEditShowClients = CreateWindowW(L"EDIT", NULL, 
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | 
				ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
				0, 0, 0, 0,
				hWnd, (HMENU)IDC_EDITCLIENTS, 
				(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);*/
}


