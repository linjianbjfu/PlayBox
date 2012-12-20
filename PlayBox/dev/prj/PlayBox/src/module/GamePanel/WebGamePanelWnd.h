#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../DataInterface/IPanelChangeObserver.h"
#include "../../LayoutMgr/ISkinMgr.h"

class MyWebBrowserWnd;
class CxSkinButton;

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
	afx_msg void	OnClickedRefresh();
	afx_msg void	OnClickedToFull();
	afx_msg void	OnClickedExitFull();
	afx_msg void	OnClickedMute();
	afx_msg void	OnClickedUnMute();
	afx_msg void	OnClickedClearCache();
	afx_msg void	OnClickedSite();
	afx_msg void	OnClickedCustomService();
	afx_msg void	OnClickedPay();

	afx_msg void	OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnDestroy();
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
public:
	virtual void	IPanelChangeOb_ToFullScreen( CWnd* pWnd );
	virtual void	IPanelChangeOb_ExitFullScreen( CWnd* pWnd );
	void	SetMainWindow(bool isTopMost);
private:
	MyWebBrowserWnd*	m_pWndWebGame;
	CxSkinButton*		m_pBtnRefresh;
	CxSkinButton*		m_pBtnToFull;
	CxSkinButton*		m_pBtnExitFull;
	CxSkinButton*		m_pBtnMute;
	CxSkinButton*		m_pBtnUnMute;
	CxSkinButton*		m_pBtnClearCache;
	CxSkinButton*		m_pBtnSite;
	CxSkinButton*		m_pBtnCustomService;
	CxSkinButton*		m_pBtnPay;

	WEB_GAME			m_webGame;
	bool				m_isMainWindowTopMost;
	bool				m_bFullScreen;	//ÊÇ·ñÈ«ÆÁ
	CRect				m_rectBeforeFull;
	CRect				m_rectFullScreen;
	CWnd*				m_pWndParent;
public:
	void	Recycle();
	
};