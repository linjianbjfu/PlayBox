// PushBox.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CPushBoxApp:
// �йش����ʵ�֣������ PushBox.cpp

//

class CPushBoxApp : public CWinApp
{
public:
	CPushBoxApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPushBoxApp theApp;
