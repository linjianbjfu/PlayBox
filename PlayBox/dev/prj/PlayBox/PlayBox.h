#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������
#define WM_PLAYBOX_PROTOCOL               WM_USER + 100

class CPlayBoxApp : public CWinApp
{
public:
	CPlayBoxApp();

	HANDLE	m_hMutexhandle;
	BOOL    m_bHasGdiPlus;
	virtual BOOL InitInstance();

	int		GetProcessNum();
	int		KillOtherInstance();
	void	SendCmdLineMsg(HWND hKwMusic);

	CString m_strCmdLine;
	DECLARE_MESSAGE_MAP()

	virtual int ExitInstance();
};

extern CPlayBoxApp theApp;
