#pragma once
#include "stdafx.h"
#include <afxmt.h>
#include "IHttpExeDownObserver.h"
#include <string>
#include <map>
using std::string;
using std::map;

class YL_CHTTPDownFile;

class PcGameDownCtrl : public CWnd
{
public:
	PcGameDownCtrl(void);
	~PcGameDownCtrl(void);
private:
	CMutex					m_cs;
	map<string,YL_CHTTPDownFile*>	m_mapDownThread;

	void NotifyDownStart( string& strID );
	void NotifyDownFinish( string& strID);
	void NotifyDownFailed( string& strID, HTTP_DOWN_FAILED_REASON r );
	void NotifyDownCompress (string & strID);
	void NotifyDownProgress( string& strID, double dPercent, unsigned int unFileSize, unsigned int unSpeed);
	void NotifyDownQuickPlay (string &strID);

// 	HANDLE m_eDecompress[MAX_PATH];
// 	enum DecompressOperator { DO_QUICKPLAY, DO_FINISH };
// 	map<DWORD, pair<string, DecompressOperator> > m_DecompressCache;
// 	int m_curEventIndex;
public:
	static PcGameDownCtrl*	GetInstance();
	static void				DelInstance();
	static PcGameDownCtrl*	m_pCtr;
	void StartDown( string strID);
	void StopDown( string strID );

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
};