#pragma once
#include "stdafx.h"
#include <afxmt.h>
#include "IHttpDownObserver.h"
#include <string>
#include <map>
using std::string;
using std::map;

class YL_CHTTPDownFile;


class HttpDownCtrl : public CWnd
{
public:
	HttpDownCtrl(void);
	~HttpDownCtrl(void);
private:
	CMutex					m_cs;
	map<string,YL_CHTTPDownFile*>	m_mapDownThread;

	void NotifyDownStart( string& strID );
	void NotifyDownFinish( string& strID, string& strSwfPath );
	void NotifyDownFailed( string& strID, HTTP_DOWN_FAILED_REASON r );
	void NotifyDownProgress( string& strID, double dPercent, unsigned int unFileSize, unsigned int unSpeed);
public:
	static HttpDownCtrl*	GetInstance();
	static void				DelInstance();
	static HttpDownCtrl*	m_pCtr;
	void StartDown( string strID, string strSwfUrl );
	void StopDown( string strID );

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
};