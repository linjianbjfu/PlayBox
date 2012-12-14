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
	//����KwMV
	void StartKWMV(  const char* pszKwMVPath, const char* pszAppName = ""  );

	//�˳�KwMV
	void CloseKWMV();

	//ע�����P2P��Ϣ����
	void AttachP2PListener(const P2PObserver* pP2PGObserver);

	//��ע����P2P��Ϣ����
	void DetachP2PListener(const P2PObserver* pP2PGObserver);

	//����������Դ
	bool StartDown(const string& strID, const P2PObserver* pP2PObserver, bool bContinueDown);

	//ֹͣ������Դ
	void StopDown(const string& strID, P2PObserver* pP2PObserver);

	//ɾ��������Դ
	void DelRes(const string& strID, P2PObserver* pP2PObserver);

	//��ȡ��Դ��Ϣ
	void GetResInfo(const string& strID, unsigned int& uiRate, string& strFilePath);

	//ֹͣ����
	void StopAll(void);

	//ע��ϵͳ�㲥��Ϣ���մ���
	void SetSysMsgWnd(HWND hWnd);
};