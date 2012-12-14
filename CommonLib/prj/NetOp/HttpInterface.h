#pragma once

#include <Windows.h>

#include <map>
#include <set>
#include <list>
#include <string>
#include <vector>
#include <functional>

#include "../../include/YL_HTTPRequestExport.h"
#include "../../include/YL_HTTPDownFile.h"

using namespace std;

class HTTPObserver;

typedef enum _tagHTTP_DOWNLOAD_TASK_STATE
{
	HTTP_DOWNLOAD_TASK_STATE_NOT_START = 0,
	HTTP_DOWNLOAD_TASK_STATE_DOWNLOADING,
	HTTP_DOWNLOAD_TASK_STATE_FINISHED,

	HTTP_DOWNLOAD_TASK_STATE_MAX
}HTTP_DOWNLOAD_TASK_STATE;

typedef struct _tagHTTP_DOWNLOAD_TASK
{
	YL_CHTTPDownFile *pDownloader;
	HTTPObserver *pObserver;
	HTTP_DOWNLOAD_TASK_STATE enDownloadState;
}HTTP_DOWNLOAD_TASK;

typedef vector<HTTP_DOWNLOAD_TASK *> HTTP_DOWNLOAD_TASK_LIST;

class CHttpInterface
{
public:
	CHttpInterface(void);
	~CHttpInterface(void);

public:
	bool Init();
	void UnInit();

	bool Stop(void);

	// Insert Download Task Into Download Task.
	bool InsertDownload(const char* szUrl, const HTTPObserver *pHttpObserver);

	// Remove Download Task From Download Task.
	bool RemoveDownload(const char* szUrl, const HTTPObserver* pHttpObserver);

	void UpdateDownlaodState();

private:
	HTTP_DOWNLOAD_TASK_LIST m_vecTaskList;
	CRITICAL_SECTION		m_CriticalSection;
	bool					m_bInitialized;

	string GetTempFileByUrl(const char* szUrl);
};

extern CHttpInterface g_httpInterface;
