// EncConf.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CEncConfApp:
// �йش����ʵ�֣������ EncConf.cpp
//

class CEncConfApp : public CWinApp
{
public:
	CEncConfApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CEncConfApp theApp;
