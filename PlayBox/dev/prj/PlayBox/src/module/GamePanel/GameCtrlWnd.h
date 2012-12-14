#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../DataInterface/IHttpDownObserver.h"
#include "../../DataInterface/IPanelChangeObserver.h"
#include "../../LayoutMgr/ISkinMgr.h"

#define WM_REPLAY			WM_USER+3  //ÖØÍæ
#define WM_CUT_SCREEN	    WM_USER+4  //½ØÆÁ

class CShockwaveFlash;
class DownPercentWnd;
class MyWebBrowserWnd;
class CShowMenu;
class IPanelChange;
class CxSkinButton;

class CGameCtrlWnd : public CBasicWnd
{
	DECLARE_DYNAMIC(CGameCtrlWnd)
public:
	CGameCtrlWnd();
	virtual ~CGameCtrlWnd();
	string	m_strGameName;
	void	UpdateAllWnd();
	void	PlayMovie( string strID, string strPath );

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void	OnPaint();
	afx_msg void	OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnDestroy();

	afx_msg void	OnClickedReplay();
	afx_msg void	OnClickedFullScreen();
	afx_msg void	OnClickedExitFullScreen();
	afx_msg void	OnClickedMute();
	afx_msg void	OnClickedUnMute();
	afx_msg void	OnClickedCut ();
	afx_msg void	OnShowWindow(BOOL bShow, UINT nStatus);

private:
	CxSkinButton*		m_pBtnRePlay;
	CxSkinButton*		m_pBtnMute;
	CxSkinButton*		m_pBtnUnMute;
	CxSkinButton*		m_pBtnToFullScreen;
	CxSkinButton*		m_pBtnExitFullScreen;
	CxSkinButton*		m_pBtnCut;

	SWF_GAME			m_swfGame;
	bool				m_bSwfInLocal;
};