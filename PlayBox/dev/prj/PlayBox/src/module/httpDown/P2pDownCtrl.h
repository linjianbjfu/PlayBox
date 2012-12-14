#pragma once
#include "stdafx.h"
#include <afxmt.h>
#include "IHttpDownObserver.h"
#include "YL_P2PInt.h"
#include <string>
#include <map>
using std::string;
using std::map;

class YL_CHTTPDownFile;


class P2pDownCtrl : public CWnd
{
public:
	P2pDownCtrl(void);
	~P2pDownCtrl(void);
private:
	CMutex					m_cs;
	map<string,YL_CHTTPDownFile*>	m_mapDownThread;

	void NotifyDownStart( string& strID );
	void NotifyDownFinish( string& strID, string& strSwfPath );
	void NotifyDownFailed( string& strID, HTTP_DOWN_FAILED_REASON r );
	void NotifyDownProgress( string& strID, double dPercent, unsigned int unFileSize, unsigned int unSpeed);
public:
	static P2pDownCtrl*	GetInstance();
	static void				DelInstance();
	static P2pDownCtrl*	m_pCtr;
	void StartDown( string strID, string strSwfUrl );
	void StopDown( string strID );

	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT nIDEvent);
public:
	//启动KwMV
	void StartKWMV(  const char* pszKwMVPath, const char* pszAppName = ""  );

	//退出KwMV
	void CloseKWMV();

	//注册接收P2P消息的类
	void AttachP2PListener(const P2PObserver* pP2PGObserver);

	//反注接收P2P消息的类
	void DetachP2PListener(const P2PObserver* pP2PGObserver);

	//启动下载资源
	bool StartDown(const string& strID, const P2PObserver* pP2PObserver, bool bContinueDown);

	//停止下载资源
	void StopDown(const string& strID, P2PObserver* pP2PObserver);

	//删除下载资源
	void DelRes(const string& strID, P2PObserver* pP2PObserver);

	//获取资源信息
	void GetResInfo(const string& strID, unsigned int& uiRate, string& strFilePath);

	//停止下载
	void StopAll(void);

	//注册系统广播消息接收窗口
	void SetSysMsgWnd(HWND hWnd);
};