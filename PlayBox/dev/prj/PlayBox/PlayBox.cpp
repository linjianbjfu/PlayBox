#include "stdafx.h"
#include "PlayBox.h"
#include "PlayBoxDlg.h"
#include "tools.h"
#include "Urlmon.h"
#include "YL_FileInfo.h"
#include "YL_StringUtil.h"
#include "YL_EncFileReg.h"
#include "YL_FileZip.h"
#include "./src/Global/GlobalFunction.h"
#include <Tlhelp32.h>
#include <process.h>
#include "../module/TopPanel/TopPanel_Control.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma   data_seg("SHARED")
HWND	  g_hSharedWnd = NULL;
#pragma   data_seg()
#pragma   comment(linker,   "/section:SHARED,RWS")
#pragma   comment( lib, "Urlmon.lib" ) 

// CPlayBoxApp

BEGIN_MESSAGE_MAP(CPlayBoxApp, CWinApp)
	//ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

// CPlayBoxApp 构造

CPlayBoxApp::CPlayBoxApp()
{
	m_bHasGdiPlus = FALSE;
}

CPlayBoxApp theApp;

BOOL CPlayBoxApp::InitInstance()
{
	HANDLE curMutexHandle = CreateMutex(NULL,true,"PBMutex");
	AfxOleInit();
	InitCommonControls();
	CWinApp::InitInstance();

	//点击webbrowser里链接的声音禁止
	//CoInternetSetFeatureEnabled( FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_THREAD, TRUE );
	//Get the command when started 
	/*m_strCmdLine = GetCommandLine();
	m_strCmdLine.MakeLower();*/
	int nArgs = 0;
	LPWSTR *szArgList = CommandLineToArgvW(GetCommandLineW(), &nArgs);;
	bool bStartupRun =false;

	if(szArgList != NULL && nArgs > 1)
	{
		if(!wcscmp(szArgList[1], L"TASKBARPIN"))
		{
			TaskBarPin();
			return FALSE;
		} else
		if(!wcscmp(szArgList[1], L"TASKBARUNPIN"))
		{
			TaskBarUnPin();
			return FALSE;
		} else
		if(!wcscmp(szArgList[1], L"INSTALL"))
		{
			SendInstallMsgToSvr(0);
			return FALSE;
		} else
		if(!wcscmp(szArgList[1], L"UNINSTALL"))
		{
			SendInstallMsgToSvr(2);
			return FALSE;
		} else
		if(!wcscmp(szArgList[1], L"OVERWRITEINSTALL"))
		{
			SendInstallMsgToSvr(4);
			return FALSE;
		} else
		if(!wcscmp(szArgList[1], L"startuprun"))
			bStartupRun = true;
		LocalFree(szArgList);
	}
	// 初始化 gdi+
	HINSTANCE lib = LoadLibrary("GdiPlus.dll");
	if(lib)
	{
		FreeLibrary(lib);
		m_bHasGdiPlus = TRUE;
	}
	ULONG_PTR m_gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	if(m_bHasGdiPlus)
		GdiplusStartup(&m_gdiplusToken,&gdiplusStartupInput,NULL);
	
	HWND hKwMusic = g_hSharedWnd;
	//判断是否已经存在实例
	if (hKwMusic)
	{
		ReleaseMutex(curMutexHandle);
		//显示原来的界面
		if( ::IsIconic( hKwMusic ) || !::IsWindowVisible (hKwMusic))//最小化
			PostMessage(hKwMusic, WM_COMMAND, ID_OPEN, 0);
		else
			::SetForegroundWindow( hKwMusic );

		//过去的代码，点击ID_OPEN，如果游戏盒正显示，就会最小化，如果最小化，就会显示
		//if(!::IsWindowVisible(hKwMusic))
		//	PostMessage(hKwMusic, WM_COMMAND, ID_OPEN, 0); //ID_WAKE_UP 是模拟鼠标双击托盘
		SendCmdLineMsg( g_hSharedWnd );
		return FALSE;
	}	
	//判断是否还有其他主进程
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
	m_hMutexhandle = ::CreateMutex(&sa, FALSE, "YPLAYBOX_2011_3_3");
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		HANDLE curMutexHandle=CreateMutex(NULL,false,"PBMutex");
		WaitForSingleObject(curMutexHandle,INFINITE);
		hKwMusic = g_hSharedWnd;
		//判断是否已经存在实例
		if (hKwMusic != NULL)
		{
			//显示原来的界面
			::SetForegroundWindow( hKwMusic );
			SendCmdLineMsg( g_hSharedWnd );
			ReleaseMutex(curMutexHandle);
			return FALSE;
		}
		ReleaseMutex(curMutexHandle);
		KillOtherInstance();
	}
	AfxEnableControlContainer();
	AfxGetUserConfig()->Config_AppStart();
	AfxGetDataManager2()->DataManAppStart();	
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CPlayBoxDlg dlg (bStartupRun);
	dlg.Create(IDD_PLAYBOX_DIALOG,NULL);
	m_pMainWnd = &dlg;

	MSG   Msg;
	while(GetMessage(&Msg,NULL,0,0))
	{
		if (!AfxPreTranslateMessage(&Msg))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		if(dlg.m_bExit)
			break;
	}
	AfxGetDataManager2()->DataManAppExit();
	AfxGetUserConfig()->Config_AppExit();
	if(m_bHasGdiPlus)
		GdiplusShutdown( m_gdiplusToken );
	//删除皮肤文件
#ifndef _DEBUG 
	GlobalFunc::DelSkinXml();
#endif
	return FALSE;
}

int CPlayBoxApp::GetProcessNum()
{
	int num = 0;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hSnapshot == INVALID_HANDLE_VALUE)
	{
		return 1;
	}

	BOOL bProcess = Process32First(hSnapshot, &pe32);
	while(bProcess)
	{
		if(strstr(pe32.szExeFile, "PlayBox.exe") != NULL)
		{
			num++;
			if(num > 2)
				break;
		}
		bProcess = Process32Next(hSnapshot, &pe32);
	}

	CloseHandle( hSnapshot );
	return num;
}

int CPlayBoxApp::KillOtherInstance()
{
	int num = 0;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hSnapshot == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	BOOL bProcess = Process32First(hSnapshot, &pe32);
	HANDLE hProcess;
	while(bProcess)
	{
		if(strstr(pe32.szExeFile, "PlayBox.exe") != NULL)
		{
			if(pe32.th32ProcessID != GetCurrentProcessId())
			{
				PROCESS_INFORMATION piProcInfoGPS;
				STARTUPINFO siStartupInfo;
				SECURITY_ATTRIBUTES saProcess, saThread;
				ZeroMemory( &siStartupInfo, sizeof(siStartupInfo) );
				siStartupInfo.cb = sizeof(siStartupInfo);
				saProcess.nLength = sizeof(saProcess);
				saProcess.lpSecurityDescriptor = NULL;
				saProcess.bInheritHandle = true;
				saThread.nLength = sizeof(saThread);
				saThread.lpSecurityDescriptor = NULL;
				saThread.bInheritHandle = true;
				char szBuffer[128];
				sprintf(szBuffer,"taskkill /f /pid %d",pe32.th32ProcessID);
				::CreateProcess( NULL,szBuffer , &saProcess,&saThread,
					false,CREATE_NO_WINDOW, NULL, NULL,&siStartupInfo,
					&piProcInfoGPS );

				DuplicateHandle(GetCurrentProcess(), 
					(HANDLE)pe32.th32ProcessID, 
					GetCurrentProcess(),
					&hProcess, 
					0,
					FALSE,
					DUPLICATE_SAME_ACCESS);
				TerminateProcess(&hProcess, 0);
				WaitForSingleObject(hProcess,2000);
				CloseHandle(hProcess);
			}
		}
		bProcess = Process32Next(hSnapshot, &pe32);
	}

	CloseHandle( hSnapshot );
	return 1;
}


void CPlayBoxApp::SendCmdLineMsg(HWND hKwMusic)
{
	if(!IsWindow(hKwMusic))
		return;
	
	switch(__argc)
	{
		DWORD result;
	case 2:
		{
			COPYDATASTRUCT copy;
			copy.dwData = WM_PLAYBOX_PROTOCOL;
			copy.cbData = (DWORD)strlen(__targv[1]) + 1;
			copy.lpData = (void*)__targv[1];
			SendMessageTimeout(hKwMusic, WM_COPYDATA, NULL, (LPARAM)&copy, SMTO_NORMAL, 1000, &result);
		}
		break;
	default:
		break;
	}
}