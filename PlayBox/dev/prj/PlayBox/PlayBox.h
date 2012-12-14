#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"		// 主符号
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
