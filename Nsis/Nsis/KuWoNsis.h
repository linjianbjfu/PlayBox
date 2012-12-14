// KuWoNsis.h : main header file for the KuWoNsis DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CKuWoNsisApp
// See KuWoNsis.cpp for the implementation of this class
//

class CKuWoNsisApp : public CWinApp
{
public:
	CKuWoNsisApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
