#include "httpinterface.h"
#include "./../Log/KwLogSvr.h"
#include "md5.h"

string HTTPDLL_LOG = "HTTPDll.log";			//日志文件路径

CHttpInterface::CHttpInterface(void)
{
	m_bInitialized = false;
}

CHttpInterface::~CHttpInterface(void)
{
	UnInit();
}

bool CHttpInterface::Init()
{
	InitializeCriticalSection(&m_CriticalSection);
	m_bInitialized = true;
	return true;
}

void CHttpInterface::UnInit()
{
	if (m_bInitialized)
	{
		EnterCriticalSection(&m_CriticalSection);
		LeaveCriticalSection(&m_CriticalSection);
		DeleteCriticalSection(&m_CriticalSection);
		m_bInitialized = false;
	}
}

bool CHttpInterface::InsertDownload(const char* szUrl, const HTTPObserver *pHttpObserver)
{
	if (!m_bInitialized)
	{
		return false;
	}


	if ((pHttpObserver == NULL) || (szUrl == NULL))
	{
		return false;
	}

	RemoveDownload(szUrl, pHttpObserver);

	string strLocalFile = GetTempFileByUrl(szUrl);
	if (strLocalFile == "")
	{
		return false;
	}

	HTTP_DOWNLOAD_TASK *pstTask = new HTTP_DOWNLOAD_TASK;
	if (pstTask == NULL)
	{
		return false;
	}
	pstTask->pDownloader = new YL_CHTTPDownFile;
	if (pstTask->pDownloader == NULL)
	{
		delete pstTask;
		return false;
	}
	pstTask->pObserver = (HTTPObserver *)pHttpObserver;
	pstTask->enDownloadState = HTTP_DOWNLOAD_TASK_STATE_NOT_START;

	EnterCriticalSection(&m_CriticalSection);
	m_vecTaskList.push_back(pstTask);
	//YL_Log(HTTPDLL_LOG, LOG_DEBUG, "CHttpInterface", "InsertDownload :%s, HTTPObserver:%u, TaskList: %d", szUrl, pHttpObserver, m_vecTaskList.size());
	bool bResult = pstTask->pDownloader->AsyncDownloadFile(szUrl, strLocalFile.c_str());

	string strInfo;
	strInfo = bResult ? "AsyncDownloadFile return true" : "AsyncDownloadFile return false";
	// MessageBox(NULL, strInfo.c_str(), "", 0);
	LeaveCriticalSection(&m_CriticalSection);

	/*
	DWORD   dwThreadID;	
	HANDLE hDownloadThread = CreateThread(
		NULL,            // Pointer to thread security attributes
		0,               // Initial thread stack size, in bytes
		HttpDownloadThread,  // Pointer to thread function
		pHttpDownloader,     // The argument for the new thread
		0,               // Creation flags
		&dwThreadID      // Pointer to returned thread identifier
		);

	if (hDownloadThread == NULL)
	{
		delete pHttpDownloader;
		return false;
	}

	m_vecTaskList.push_back(hDownloadThread);
	*/

	// YL_Log(HTTPDLL_LOG, LOG_DEBUG, "CHttpInterface", "InsertDownload Out");

	return true;
}

bool CHttpInterface::RemoveDownload(const char* szUrl, const HTTPObserver* pHttpObserver)
{
	if (!m_bInitialized)
	{
		return false;
	}

	EnterCriticalSection(&m_CriticalSection);

	//YL_Log(HTTPDLL_LOG, LOG_DEBUG, "CHttpInterface", "RemoveDownload -- IN");
	HTTP_DOWNLOAD_TASK_LIST::iterator iter;
	for (iter = m_vecTaskList.begin(); iter != m_vecTaskList.end(); iter++)
	{
		if (m_vecTaskList.size() <= 0)
		{
			break;
		}
		HTTP_DOWNLOAD_TASK *pTask = (HTTP_DOWNLOAD_TASK *)(*iter);
		if (pTask != NULL)
		{
			if ((pTask->pDownloader->GetParamUrl() == szUrl) && (pHttpObserver == pTask->pObserver))
			{
				pTask->pDownloader->Disconnect();
				Sleep(100);
				delete pTask->pDownloader;
				delete pTask;
				iter = m_vecTaskList.erase(iter);
				iter--;
				//YL_Log(HTTPDLL_LOG, LOG_DEBUG, "CHttpInterface", "RemoveDownload :%s, HTTPObserver:%u, TaskList: %d", szUrl, pHttpObserver, m_vecTaskList.size());
			}
		}
	}

	//YL_Log(HTTPDLL_LOG, LOG_DEBUG, "CHttpInterface", "RemoveDownload -- OUT");
	LeaveCriticalSection(&m_CriticalSection);
	return true;
}

bool CHttpInterface::Stop()
{
	if (!m_bInitialized)
	{
		return false;
	}

	EnterCriticalSection(&m_CriticalSection);

	HTTP_DOWNLOAD_TASK_LIST::iterator iter;
	for (iter = m_vecTaskList.begin(); iter != m_vecTaskList.end(); iter++)
	{
		if (m_vecTaskList.size() <= 0)
		{
			break;
		}
		HTTP_DOWNLOAD_TASK *pTask = (HTTP_DOWNLOAD_TASK *)(*iter);
		if (pTask != NULL)
		{
			if (pTask->pDownloader != NULL)
			{
				delete pTask->pDownloader;
			}
			delete pTask;
		}
	}
	m_vecTaskList.clear();
	LeaveCriticalSection(&m_CriticalSection);
	return true;
}

void CHttpInterface::UpdateDownlaodState()
{
	if (!m_bInitialized)
	{
		return;
	}

	EnterCriticalSection(&m_CriticalSection);
	//YL_Log(HTTPDLL_LOG, LOG_DEBUG, "CHttpInterface", "UpdateDownlaodState -- IN  %d", m_vecTaskList.size());

	HTTP_DOWNLOAD_TASK_LIST::iterator iter;
	for (iter = m_vecTaskList.begin(); iter != m_vecTaskList.end(); iter++)
	{
		if (m_vecTaskList.size() <= 0)
		{
			break;
		}
		HTTP_DOWNLOAD_TASK *pTask  = (HTTP_DOWNLOAD_TASK *)(*iter);
		if ((pTask == NULL) || (pTask->pDownloader == NULL) || (pTask->pObserver == NULL))
		{
			iter = m_vecTaskList.erase(iter);
			iter--;
			continue;
		}
		YL_CHTTPDownFile *pDownloader = pTask->pDownloader;
		unsigned int unDownloadPercent = 0;
		switch(pTask->enDownloadState) {
			case HTTP_DOWNLOAD_TASK_STATE_NOT_START:
				// 还没开始：
				// 首先判断是否已经失败：
				if (pDownloader->IsDownloadFailed())
				{
					pTask->pObserver->HTTPOb_DownFailed(pDownloader->GetParamUrl().c_str(), pDownloader->GetFailedReason());
					pTask->enDownloadState = HTTP_DOWNLOAD_TASK_STATE_FINISHED;
					break;
				}
				// 检查是否已经获取到了文件大小。如果已经获得了，则发送Start消息；并且修改为DOWNLOADING状态
				if (pDownloader->GetFileSize() <= 0)
				{
					break;
				}
				pTask->pObserver->HTTPOb_DownStart(pDownloader->GetParamUrl().c_str(), pDownloader->GetParamLocalFile().c_str(), pDownloader->GetFileSize());
				pTask->enDownloadState = HTTP_DOWNLOAD_TASK_STATE_DOWNLOADING;
				// break;		// 进入Downloading状态，可以再发送一次progress消息
			case HTTP_DOWNLOAD_TASK_STATE_DOWNLOADING:
				unDownloadPercent = int(pDownloader->GetCurrentDownloadStatus() * 100);
				if (pDownloader->IsDownloadFailed())
				{
					pTask->pObserver->HTTPOb_DownFailed(pDownloader->GetParamUrl().c_str(), pDownloader->GetFailedReason());
					pTask->enDownloadState = HTTP_DOWNLOAD_TASK_STATE_FINISHED;
				}
				else if (unDownloadPercent >= 100)
				{
					// 下载完成
					pTask->pObserver->HTTPOb_DownFinish(pDownloader->GetParamUrl().c_str());
					pTask->enDownloadState = HTTP_DOWNLOAD_TASK_STATE_FINISHED;
					// 后面没有break，直接进入FINISHED状态
				}
				else
				{
					pTask->pObserver->HTTPOb_DownProgress(pDownloader->GetParamUrl().c_str(), int(pDownloader->GetCurrentDownloadStatus() * 100), pDownloader->GetSpeed()*1000);
					break;
				}
			case HTTP_DOWNLOAD_TASK_STATE_FINISHED:
				// 清除任务
				// MessageBox(NULL, pTask->pDownloader->GetParamUrl().c_str(), "ClearTask", 0);
				delete pTask->pDownloader;
				delete pTask;
				iter = m_vecTaskList.erase(iter);
				iter--;
				break;
			default:
				break;
		}
	}

	//YL_Log(HTTPDLL_LOG, LOG_DEBUG, "CHttpInterface", "UpdateDownlaodState -- OUT");
	LeaveCriticalSection(&m_CriticalSection);
}

string CHttpInterface::GetTempFileByUrl(const char* szUrl)
{
	char szBuffer[MAX_PATH];
	strcpy(szBuffer, szUrl);
	char szOutput[33];
	memset(szOutput, 0, 33);
	MD5String(szBuffer, szOutput);
	string strFilename = szOutput;

	// 首先获取系统临时目录
	if (!GetTempPath(MAX_PATH, szBuffer))
	{
		if (!GetModuleFileName(NULL, szBuffer, MAX_PATH))
		{
			return "";
		}
		TCHAR *p = strrchr(szBuffer, '\\');
		if (p != NULL)
		{
			*p = '\0';
		}
	}
	if (szBuffer[strlen(szBuffer)-1] != '\\')
	{
		strcat(szBuffer, "\\");
	}

	// 获取URL中的后缀
	string strSuffix;
	char *p = strrchr(szUrl, '.');
	if ((p != NULL) && (strlen(p) <= 7))
	{
		strSuffix = p;
	}
	else
	{
		return "";
	}
	// 根据URL生成唯一的文件名
	strFilename = szBuffer + strFilename + strSuffix;
	return strFilename;
}
