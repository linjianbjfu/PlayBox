#include "windows.h"
#include "../../include/YL_HTTPDownFile.h"

#include "HttpInterface.h"

HANDLE g_hExitEvent = NULL;
HANDLE g_hKWHDThread = NULL;
CHttpInterface g_httpInterface;

DWORD WINAPI KWHDThread(VOID* pPar)
{
	while (WaitForSingleObject(g_hExitEvent, 0) != WAIT_OBJECT_0)
	{
		g_httpInterface.UpdateDownlaodState();
		Sleep(500);
	}
	return 0;
}

void StartKWHD(const char* pszAppName)
{
	if (g_hKWHDThread != NULL)
	{
		// 已经启动了！
		return;
	}

	if (!g_httpInterface.Init())
	{
		return;
	}

	g_hExitEvent = CreateEvent(NULL, TRUE, FALSE, "KWHD_EXIT");
	DWORD   dwThreadID;	

	//	YL_Log( LOG_FILE,LOG_NOTICE,"Request","Request Start! url:%s,timerout:%d,%u",lpszURL,iTimeOut,this );
	g_hKWHDThread = CreateThread(
		NULL,            // Pointer to thread security attributes
		0,               // Initial thread stack size, in bytes
		KWHDThread,  // Pointer to thread function
		NULL,     // The argument for the new thread
		0,               // Creation flags
		&dwThreadID      // Pointer to returned thread identifier
		);
}

void CloseKWHD()
{
	if (g_hExitEvent != NULL)
	{
		SetEvent(g_hExitEvent);
		Sleep(100);
		CloseHandle(g_hExitEvent);
	}
	if (g_hKWHDThread != NULL)
	{
		CloseHandle(g_hKWHDThread);
		g_hKWHDThread = NULL;
	}
}

bool StartHttpDown(const char* szUrl, HTTPObserver *pHTTPObserver)
{
	bool bResult = g_httpInterface.InsertDownload(szUrl, pHTTPObserver);
	if (!bResult)
	{
		// 启动下载失败，直接发送下载失败消息
		pHTTPObserver->HTTPOb_DownFailed(szUrl, (HTTP_DOWN_FAILED_REASON)0);
	}
	return bResult;
}

void StopHttpDown(const char* szUrl, HTTPObserver* pHTTPObserver) 
{
	g_httpInterface.RemoveDownload(szUrl, pHTTPObserver);
}

void StopAllHttp()
{
	g_httpInterface.Stop();
}

