#include "stdafx.h"
#include "JudgeFirewall.h"
#include "YL_DirInfo.h"

static	int	s_iHTTPRes = -1;


DWORD WINAPI Ping_Thread(void* pArg)
{
	STARTUPINFO    		 si;
	PROCESS_INFORMATION  pi;
	char cmd[512];
	int nRet;

	memset(&si, 0, sizeof (STARTUPINFO)) ;
	si.cb = sizeof (STARTUPINFO) ;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	memset(&pi, 0, sizeof (PROCESS_INFORMATION));
	memset(cmd, 0, 512);

	string strDir;
	YL_DirInfo::GetWindowsDir(strDir);
	strDir += "\\system32\\ping.exe www.baidu.com";

	nRet = CreateProcess (NULL,(LPSTR)(strDir.c_str()), NULL, NULL,
		FALSE, (DWORD) NORMAL_PRIORITY_CLASS, NULL,NULL, &si, &pi);

	DWORD dw = GetLastError();

	if(nRet==0)
	{
		return 0;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	BOOL bExit = GetExitCodeProcess( pi.hProcess,&dw );
	CloseHandle(pi.hProcess);		
	return 0;
}

static void		Test_Ping()
{
	static BOOL		s_bTestKwMusic	= FALSE;
	CloseHandle( CreateThread(0,0,Ping_Thread,NULL,0,0) );
	s_bTestKwMusic = TRUE;
}

void CJudgeFirewall::SetHTTPRes( HTTP_REQUEST_TYPE hrt,int iRes )
{
	if( hrt == HTTP_SEARCH )
	{
		if( s_iHTTPRes == 0 && iRes == 0 )
			Test_Ping();
	}	

	if( hrt == HTTP_WEB_COMMAND )
		s_iHTTPRes = iRes;
}