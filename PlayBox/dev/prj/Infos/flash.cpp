/******************************************************************
***
***
***				FREE WINDOWLESS FLASH CONTROL
***
***					   by Makarov Igor
***
***		for questions and remarks mailto: mak_july@list.ru
***
***
*******************************************************************/
// flash.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "FlashWnd.h"
#include "LhFlashImg.h"
#include "../../../../CommonLib/Prj/Log/KwLogSvr.h"
#include "tools.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL				InitFlash();
BOOL				IniProcess();

#pragma   data_seg("SHARED")
HWND	  g_hWnd = NULL;
#pragma   data_seg()
#pragma   comment(linker,   "/section:SHARED,RWS")

HINSTANCE	g_hInst		= NULL;
CFlashWnd*	g_flashWnd	= NULL;
BOOL		bExpire		= FALSE;

//flash window position and size
unsigned int nGroup;
unsigned int nUserID;

BOOL InitProcess()
{
	YL_Log("flashtips.log", LOG_DEBUG, "flashTip", "InitProcess Begin.");
	//判断是否还有其他进程
	SECURITY_DESCRIPTOR     sd_wideopen;
	InitializeSecurityDescriptor(&sd_wideopen, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(
		&sd_wideopen,	// addr of SD
		TRUE,		    // TRUE = DACL present
		NULL,		    // ... but it's empty (wide open)
		FALSE);		    // DACL explicitly set, not defaulted

	SECURITY_ATTRIBUTES     sa;
	memset(&sa, 0, sizeof sa);
	sa.nLength              = sizeof sa;
	sa.lpSecurityDescriptor = &sd_wideopen;
	sa.bInheritHandle       = FALSE;
	HANDLE m_hMutexhandle = ::CreateMutex(&sa, FALSE, "Young_PB_PushMsg_2011-3-22");
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//判断是否已经存在实例
		if (g_hWnd != NULL)
		{
			YL_Log("flashtips.log", LOG_DEBUG, "flashTip", "InitProcess SendMessage.");
			PostMessage(g_hWnd, WM_COMMAND, IDC_RESET, 0);
		}

		YL_Log("flashtips.log", LOG_DEBUG, "flashTip", "InitProcess CloseHandle.");
		CloseHandle( m_hMutexhandle );
		return FALSE;
	}
	return TRUE;
}

BOOL InitFlash()
{
	YL_Log("flashtips.log", LOG_DEBUG, "flashTip", "InitFlash Begin.");
	if(nGroup >= CLhFlashImg::GetFlashMessageNum())
	{
		YL_Log("flashtips.log", LOG_DEBUG, "flashTip", "InitFlash Begin. GetFlashMessageNum is %u, nGroup is %u.", CLhFlashImg::GetFlashMessageNum(), nGroup);
		return FALSE;
	}
	int nTop = 0, nLeft = 0, nWidth = 0, nHeight = 0;
	unsigned int nTimer = 0;
	string strId;
	if(!CLhFlashImg::GetFlashMsgInfo(nUserID, nGroup, nTop, nLeft, nWidth, nHeight, nTimer, strId, g_flashWnd->m_strMovie))
		return FALSE;
	YL_Log("flashtips.log", LOG_DEBUG, "flashTip", "InitFlash SetWindowPos.");
	SetWindowPos(g_hWnd, NULL, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER|SWP_NOACTIVATE);

	SetTimer(g_hWnd, IDC_FLASH_TIMER, nTimer, NULL);
	bExpire = FALSE;
	nGroup++;
	CLhFlashImg::SetFlashPopGroupIndex(nGroup);

	strId = "ID:" + strId + "|GROUP:" + uint2str(nGroup);
	LogRealMsg("FLASH_TIPS", strId.c_str());
	return TRUE;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	YL_Log("flashtips.log", LOG_DEBUG, "flashTip", lpCmdLine);
	if(!InitProcess())
		return 1;
	else
	{
		LogRealMsg("START_FLASH", "");
		if(CLhFlashImg::GetFlashPopDate() != GetCurrentDate())
		{
			nGroup = 0;
			CLhFlashImg::SetFlashPopDate(GetCurrentDate());
		}
		else
		{
			nGroup = CLhFlashImg::GetFlashPopGroupIndex();
		}
	}
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_FLASH, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	nUserID = str2uint(lpCmdLine);
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		YL_Log("flashtips.log", LOG_DEBUG, "flashTip", "InitInstance Failed.");
		return 1;
	}

	g_hInst = hInst;
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_FLASH);
	OleInitialize(NULL);
	LogInit( "YPFlashTips" );

	g_flashWnd = new CFlashWnd;
	if(!InitFlash())
	{
		delete g_flashWnd;
		return 1;
	}
	YL_Log("flashtips.log", LOG_DEBUG, "flashTip", "WinMain Create Flash Wnd.");
//  create windowless control
	g_flashWnd->Create(ShockwaveFlashObjects::CLSID_ShockwaveFlash,
		WS_EX_LAYERED, WS_POPUP | WS_CLIPSIBLINGS, g_hWnd, g_hInst);

//  to create a windowed control uncomment this
//	g_flashWnd->Create(ShockwaveFlashObjects::CLSID_ShockwaveFlash,
//		0, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, g_hWnd, g_hInst);
	::SetWindowPos(g_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
	if(IsWindow(g_flashWnd->GetHWND()))
	{
		g_flashWnd->WndProc(g_flashWnd->GetHWND(), WM_PAINT, 0, 0);
		ShowNoActiveTopWindow(g_flashWnd->GetHWND());
	}
	
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	delete g_flashWnd;
	OleUninitialize();
	return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_FLASH);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_FLASH;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		return FALSE;
	}

	DWORD dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
	dwExStyle = dwExStyle | WS_EX_TOOLWINDOW;
	SetWindowLong(hWnd, GWL_EXSTYLE, dwExStyle);
	ShowWindow(hWnd, SW_HIDE);
	UpdateWindow(hWnd);

	g_hWnd = hWnd;
	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_EXIT:
				    DestroyWindow(hWnd);
				    break;
				case IDC_RESET:
					LogRealMsg("FLASH_RESET", "nGroup = 0");
					break;
				default:
				    return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			RECT rt;
			GetClientRect(hWnd, &rt);
			DrawText(hdc, szHello, strlen(szHello), &rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_MOVE:
			{
				RECT r;
				GetWindowRect(hWnd, &r);
				if (g_flashWnd)
					SetWindowPos(g_flashWnd->GetHWND(), NULL, r.left, r.top, 0, 0, SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
			}
			break;
		case WM_SIZE:
			{
				RECT r;
				GetWindowRect(hWnd, &r);
				if (g_flashWnd)
					SetWindowPos(g_flashWnd->GetHWND(), NULL, 0, 0, (r.right-r.left), (r.bottom-r.top), SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
			}
			break;
		case IDC_NEXT_FLASH:
			{
				g_flashWnd->StopFlash();
				ShowWindow(g_flashWnd->GetHWND(), SW_HIDE);
				if(nGroup >= CLhFlashImg::GetFlashMessageNum())
				{
					PostMessage(g_hWnd, WM_DESTROY, 0, 0);
					return 0;
				}
				if(bExpire)
				{
					SetTimer(g_hWnd, IDC_FLASH_TIMER, 60 * 1000, NULL);
					bExpire = FALSE;
				}
			}
			break;
		case WM_TIMER:
			{
				if(wParam == IDC_FLASH_TIMER)
				{
					bExpire = TRUE;
					KillTimer(g_hWnd, IDC_FLASH_TIMER);
					if(!IsWindowVisible(g_flashWnd->GetHWND()))
					{
						if(!InitFlash())
						{
							PostMessage(g_hWnd, WM_DESTROY, 0, 0);
							return 0;
						}
						g_flashWnd->OnAfterShowingContent();
						g_flashWnd->WndProc(g_flashWnd->GetHWND(), WM_PAINT, 0, 0);
						ShowNoActiveTopWindow(g_flashWnd->GetHWND());
					}
				}
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}
