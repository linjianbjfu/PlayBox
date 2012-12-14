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

#pragma comment( lib, "Urlmon.lib" ) 

// CPlayBoxApp

BEGIN_MESSAGE_MAP(CPlayBoxApp, CWinApp)
	//ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPlayBoxApp ����

CPlayBoxApp::CPlayBoxApp()
{
	m_bHasGdiPlus = FALSE;
}

CPlayBoxApp theApp;

BOOL CPlayBoxApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControls()�����򣬽��޷��������ڡ�
	HANDLE curMutexHandle = CreateMutex(NULL,true,"PBMutex");

	AfxOleInit();
	InitCommonControls();
	CWinApp::InitInstance();

	//���webbrowser�����ӵ�������ֹ
	
	//CoInternetSetFeatureEnabled( FEATURE_DISABLE_NAVIGATION_SOUNDS, SET_FEATURE_ON_THREAD, TRUE );


	m_strCmdLine = GetCommandLine();
	YL_Log("debug.txt",LOG_DEBUG,"commandline","%s",(LPCTSTR)m_strCmdLine);
	m_strCmdLine.MakeLower();

	bool bStartupRun = m_strCmdLine.Find ("startuprun") != -1;

	// ��ʼ�� gdi+
	HINSTANCE lib = LoadLibrary("GdiPlus.dll");
	if( lib != NULL)
	{
		FreeLibrary(lib);
		m_bHasGdiPlus = TRUE;
	}
	ULONG_PTR m_gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	if(m_bHasGdiPlus)
	{
		if( GdiplusStartup(&m_gdiplusToken,&gdiplusStartupInput,NULL) != Ok )
		{
			//LogUserActMsg("GdiplusStartupInput","FAILED:0");
		}
		else
		{
			//LogUserActMsg("GdiplusStartupInput","FAILED:1");
		}
	}
	HWND hKwMusic = g_hSharedWnd;
	//�ж��Ƿ��Ѿ�����ʵ��
	if (hKwMusic != NULL)
	{
		ReleaseMutex(curMutexHandle);

		//��ʾԭ���Ľ���
		if( ::IsIconic( hKwMusic ) ||
			!::IsWindowVisible (hKwMusic))//��С��
		{
			PostMessage(hKwMusic, WM_COMMAND, ID_OPEN, 0);
		}else
		{
			::SetForegroundWindow( hKwMusic );
		}
		//��ȥ�Ĵ��룬���ID_OPEN�������Ϸ������ʾ���ͻ���С���������С�����ͻ���ʾ
		//if(!::IsWindowVisible(hKwMusic))
		//	PostMessage(hKwMusic, WM_COMMAND, ID_OPEN, 0); //ID_WAKE_UP ��ģ�����˫������

		SendCmdLineMsg( g_hSharedWnd );
		return FALSE;
	}	
	//�ж��Ƿ�������������
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
		//�ж��Ƿ��Ѿ�����ʵ��
		if (hKwMusic != NULL)
		{
			//��ʾԭ���Ľ���
			::SetForegroundWindow( hKwMusic );
			SendCmdLineMsg( g_hSharedWnd );
			ReleaseMutex(curMutexHandle);
			return FALSE;
		}
		ReleaseMutex(curMutexHandle);
		KillOtherInstance();
	}

	AfxEnableControlContainer();

	LogInit( "PlayGame" );
	AfxGetUserConfig()->Config_AppStart();
	AfxGetDataManager2()->DataManAppStart();	
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CPlayBoxDlg dlg (bStartupRun);
	dlg.Create(IDD_PLAYBOX_DIALOG,NULL);

	//��¼�Ƿ񿪻�����
	bool bAutoRun = GlobalFunc::IsAutoRun();
	string strLog;
	YL_StringUtil::Format( strLog, "AutoRun:%d", bAutoRun );
	LogRealMsg( "AppStart", strLog.c_str() );

	m_pMainWnd = &dlg;

	MSG   Msg;
	while(GetMessage(&Msg,NULL,0,0))
	{
		//AfxPreTranslateMessage(&Msg);
		if (!AfxPreTranslateMessage(&Msg))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		if(dlg.m_bExit)
		{
			break;
		}
	}

	AfxGetDataManager2()->DataManAppExit();
	AfxGetUserConfig()->Config_AppExit();

	if(m_bHasGdiPlus)
		GdiplusShutdown( m_gdiplusToken );
	//ɾ��Ƥ���ļ�
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

int CPlayBoxApp::ExitInstance()
{
	////�ж��Ƿ�������������
	//SECURITY_DESCRIPTOR     sd_wideopen;
	//InitializeSecurityDescriptor(&sd_wideopen, SECURITY_DESCRIPTOR_REVISION);
	//SetSecurityDescriptorDacl(
	//	&sd_wideopen,	// addr of SD
	//	TRUE,		    // TRUE = DACL present
	//	NULL,		    // ... but it's empty (wide open)
	//	FALSE);		    // DACL explicitly set, not defaulted

	//SECURITY_ATTRIBUTES     sa;
	//memset(&sa, 0, sizeof sa);
	//sa.nLength              = sizeof sa;
	//sa.lpSecurityDescriptor = &sd_wideopen;
	//sa.bInheritHandle       = FALSE;
	//HANDLE handle = ::CreateMutex(&sa, FALSE, "YPLAYBOX_2011_3_3");
	//::ReleaseMutex(handle);
	//CloseHandle(handle);
	//
	//g_hSharedWnd = NULL;

	//win32exec("PlayBox.exe", GetExePath().c_str(), false);
	return __super::ExitInstance();
}