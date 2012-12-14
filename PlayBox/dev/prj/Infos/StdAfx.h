/******************************************************************
***
***
***				FREE WINDOWLESS FLASH CONTROL
***
***					   by Makarov Igor
***
***		for questions and remarks mailto: mak_july@list.ru
***
***
*******************************************************************/
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

//#define WINVER 0x0500
#define _WIN32_WINDOWS 0x0500
#define _WIN32_WINNT 0x0500

// Windows Header Files:
#include <windows.h>
#if defined(WINVER) && (WINVER < 0x0500)
	#pragma message( "**********************************************************\nThis should be built using Platform SDK February 2003 or later (WINVER 0x0500)\n**********************************************************" )
#endif

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>

#include <ddraw.h>

#ifndef DEFINE_GUID2
#define DEFINE_GUID2(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        const GUID name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#endif	   

// Local Header Files

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
