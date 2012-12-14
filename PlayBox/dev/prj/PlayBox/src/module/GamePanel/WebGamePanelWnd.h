#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../DataInterface/IPanelChangeObserver.h"
#include "../../LayoutMgr/ISkinMgr.h"

class WebGamePlayingWnd;
class MyWebBrowserWnd;
class IPanelChange;

class WebGamePanelWnd : public CBasicWnd,
						public IPanelChangeObserver
{
	DECLARE_DYNAMIC(WebGamePanelWnd)
public:
	WebGamePanelWnd();
	virtual ~WebGamePanelWnd();
	void	LoadSkin();
	void	SetTabItem( TAB_ITEM ti );
	void	Init();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void	OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnDestroy();
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
public:
	virtual void	IPanelChangeOb_ToFullScreen( CWnd* pWnd );
	virtual void	IPanelChangeOb_ExitFullScreen( CWnd* pWnd );
	void	SetMainWindow(bool isTopMost);
private:
	MyWebBrowserWnd*	m_pWndGameRight;
	WebGamePlayingWnd*	m_pWndGamePlaying;
	WEB_GAME			m_webGame;

	string	GetRightWebUrl( string strId );

	bool				m_isMainWindowTopMost;
	bool				m_bFullScreen;	//ÊÇ·ñÈ«ÆÁ
	CRect				m_rectBeforeFull;
	CRect				m_rectFullScreen;
	CWnd*				m_pWndParent;
public:
	void	Recycle();
	
};