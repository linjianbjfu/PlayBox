////////////////////////////////////////////////////////////////
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual C++ 6.0 on Windows 9x/NT/XP
//
#include "stdafx.h"
#include "trayicon.h"
#include <afxpriv.h>		// for AfxLoadString
#include "menuxp.h"
#include "../util/BalloonHelp.h"
#include "../util/ShowMenu.h"
#include "../util/ShowMenu.h"
#include "../../Core/AfxGlobals.h"
#include "../../../Resource.h"
#include "tools.h"

#define countof(x)	(sizeof(x)/sizeof(x[0]))
//extern CMyApp theApp;
//////////////////
// Windows sends this message when the taskbar is created. This can happen
// if it crashes and Windows has to restart it. CTrayIcon responds by
// re-installing its icon.
//
const UINT WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));

IMPLEMENT_DYNAMIC(CTrayIcon, CCmdTarget)

CTrayIcon::CTrayIcon(UINT uID)
{
	// Initialize NOTIFYICONDATA
	memset(&m_nid, 0 , sizeof(m_nid));
	m_nid.cbSize = sizeof(m_nid);
	m_nid.uID = uID;	// never changes after construction

	m_notifyHook.m_pTrayIcon = this; // notification window hook
	m_parentHook.m_pTrayIcon = this;	// parent window hook
	m_bDbClicked = false;
	m_bRClick = false;
	m_pMenu = NULL;
	// Use resource string as tip if there is one
	AfxLoadString(uID, m_nid.szTip, sizeof(m_nid.szTip));	
}

CTrayIcon::~CTrayIcon()
{
	SetIcon(0); // remove icon from system tray
}

//////////////////
// Set notification window. It must created already.
//
void CTrayIcon::SetNotificationWnd(CWnd* pNotifyWnd, UINT uCbMsg)
{
	// If the following assert fails, you're probably
	// calling me before you created your window. Oops.
	ASSERT(pNotifyWnd==NULL || ::IsWindow(pNotifyWnd->GetSafeHwnd()));
	m_nid.hWnd = pNotifyWnd->GetSafeHwnd();

	ASSERT(uCbMsg==0 || uCbMsg>=WM_USER);
	m_nid.uCallbackMessage = uCbMsg;

	CWnd* pParentWnd = pNotifyWnd ? pNotifyWnd->GetTopLevelParent() : NULL;

	// Install window hooks. Must be different because
	// taskbar creation message only goes to top-level parent.
	m_notifyHook.HookWindow(pNotifyWnd);
	if (pParentWnd!=pNotifyWnd)
		m_parentHook.HookWindow(pParentWnd);
	m_tipPosition.InitializePositionTracking(m_nid.hWnd,m_nid.uID);
}

//////////////////
// This is the main variant for setting the icon.
// Sets both the icon and tooltip from resource ID
// To remove the icon, call SetIcon(0)
//
BOOL CTrayIcon::SetIcon(UINT uID)
{ 
	HICON hicon=NULL;
	if (uID) {
		AfxLoadString(uID, m_nid.szTip, sizeof(m_nid.szTip));
		hicon = AfxGetApp()->LoadIcon(uID);
	}
	return SetIcon(hicon, NULL);
}

//////////////////
// Common SetIcon for all overloads. 
//
BOOL CTrayIcon::SetIcon(HICON hicon, LPCTSTR lpTip) 
{
	UINT msg;
	m_nid.uFlags = 0;

	// Set the icon
	if (hicon) {
		// Add or replace icon in system tray
		msg = m_nid.hIcon ? NIM_MODIFY : NIM_ADD;
		m_nid.hIcon = hicon;
		m_nid.uFlags |= NIF_ICON;
	} else { // remove icon from tray
		if (m_nid.hIcon==NULL)
			return TRUE;		// already deleted
		msg = NIM_DELETE;
	}

	// Use the tip, if any
	if (lpTip)
	{
		_snprintf(m_nid.szTip,countof(m_nid.szTip)-1,"%s",lpTip);
		m_nid.szTip[ countof(m_nid.szTip)-1 ] = '\0';
//		_tcsncpy(m_nid.szTip, lpTip, countof(m_nid.szTip));
	}
	if (m_nid.szTip[0])
		m_nid.uFlags |= NIF_TIP;

	// Use callback if any
	if (m_nid.uCallbackMessage && m_nid.hWnd)
		m_nid.uFlags |= NIF_MESSAGE;

	// Do it
	BOOL bRet = Shell_NotifyIcon(msg, &m_nid);
	if (msg==NIM_DELETE || !bRet)
		m_nid.hIcon = NULL;	// failed
	return bRet;
}

void CTrayIcon::ShowTrayIconBalloon( CString strTitle, CString str, unsigned int unTimeOut )
{
	CTrayIconPosition::TrackType eTrackType = CTrayIconPosition::UseBothTechniquesDirectPrefered;
	//use m_tipPosition.Invalidate(); if you want to avoid few-seconds postion cashing (visual scan mode only)
	CPoint ptIcon;
	BOOL bIconFound = m_tipPosition.GetTrayIconPosition(ptIcon, eTrackType);

	if(bIconFound == -1)	
		return;

	m_tipPosition.RestoreTrayIcon(AfxGetApp()->LoadIcon(m_nid.uID));
	CBalloonHelp::LaunchBalloon( strTitle, str, ptIcon, 
		IDI_INFORMATION, CBalloonHelp::unSHOW_CLOSE_BUTTON|CBalloonHelp::unDISABLE_XP_SHADOW|CBalloonHelp::unSHOW_TOPMOST,
		CWnd::FromHandle(::GetDesktopWindow()), "", unTimeOut );
}
//////////////////
// Same hook class used for both notifcation window and toplevel
// parent; hook function determines which.
//
LRESULT CTrayIcon::CTrayHook::WindowProc(UINT msg, WPARAM wp, LPARAM lp)
{
	if (msg==m_pTrayIcon->m_nid.uCallbackMessage &&
		 m_hWnd==m_pTrayIcon->m_nid.hWnd) {

		m_pTrayIcon->OnTrayNotify(wp, lp);

	} else if (msg==WM_TASKBARCREATED) {
		m_pTrayIcon->OnTaskBarCreate(wp, lp);
	}
	return CSubclassWnd::WindowProc(msg, wp, lp);
}

/////////////////
// Default event handler handles right-menu and doubleclick.
// Override to do something different.
//
LRESULT CTrayIcon::OnTrayNotify(WPARAM wID, LPARAM lEvent)
{

	if (wID!=m_nid.uID ||
		(lEvent!=WM_RBUTTONUP && lEvent != WM_LBUTTONUP && lEvent!=WM_LBUTTONDBLCLK && lEvent != WM_LBUTTONDOWN))
		return 0;

	if(lEvent == WM_LBUTTONDBLCLK)
	{
		m_bDbClicked = true;
	}
	else
	{
		m_bDbClicked = false;
	}

	if ( (lEvent == WM_RBUTTONUP) ) {
		m_bRClick = true;
		::SetTimer(m_nid.hWnd , TIMER_FOR_TRAY ,GetDoubleClickTime() / 3  ,NULL); 
	
	}
	else if(lEvent == WM_LBUTTONUP)
	{
		m_bRClick = false;
		::SetTimer(m_nid.hWnd , TIMER_FOR_TRAY ,GetDoubleClickTime() / 3 ,NULL); 
	
	}

	return 1; // handled
}

//////////////////
// Explorer had to restart the taskbar: add icons again
//
LRESULT CTrayIcon::OnTaskBarCreate(WPARAM wp, LPARAM lp)
{
	// Reinstall taskbar icon
	HICON hIcon = m_nid.hIcon;
	m_nid.hIcon = NULL;
	if (hIcon)
		SetIcon(hIcon, NULL); // will reuse current tip
	return 0;
}

int CTrayIcon::CreatePopup(HWND hwnd)
{
	m_pMenu = new CShowMenu(CWnd::FromHandle(hwnd),IDR_MENU_TRAYICON);

	if( !AfxGetUIManager()->UICanExit() )
	{
		m_pMenu->DisableItem(ID_MEMU_SETTING);
		m_pMenu->DisableItem(ID_APP_EXIT);
	}
	//////////////////////////////////
	CPoint point;
	GetCursorPos(&point);
	::SetForegroundWindow(hwnd);	
	m_pMenu->ShowMenu(point);
	delete m_pMenu;
	m_pMenu = NULL;
	return 0;
}
void CTrayIcon::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if( m_pMenu )
	{
		m_pMenu->MeasureItem(lpMeasureItemStruct);
	}
}