#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../DataInterface/IPanelChangeObserver.h"
#include "../../LayoutMgr/ISkinMgr.h"
#include "Global/GlobalVariable.h"
#include "src/module/Esc2ExitFullScrPanel/ESCFullDlg.h"

class MyWebBrowserWnd;
class CxSkinButton;
class ESCFullDlg;

#if! defined(__ESC_FULL_SCR_TIP_TIMER_DEF__)
#define __ESC_FULL_SCR_TIP_TIMER_DEF__

#define ID_TIMER_ESCFULL_TIP	10086
#define TIME_TIMER_ESCFULL_TIP	3000

#endif // __ESC_FULL_SCR_TIP_TIMER_DEF__

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
	void	SetMainWindow(bool isTopMost);
	void	Recycle();
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
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
	BOOL	PreTranslateMessage(MSG* pMsg);
	void	IPanelChangeOb_ToFullScreen( CWnd* pWnd );
	void	IPanelChangeOb_ExitFullScreen( CWnd* pWnd );

	void	ShowHideEseFull(bool isShow);
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

	ESCFullDlg*			m_pEscFullTipDlg;

	WEB_GAME			m_webGame;
	bool				m_isMainWindowTopMost;
	bool				m_bFullScreen;	//ÊÇ·ñÈ«ÆÁ
	CRect				m_rectBeforeFull;
	CRect				m_rectFullScreen;
	CWnd*				m_pWndParent;

	bool GenerateFlag(OUT std::string& strFlag,
		IN const std::string& id,
		IN const std::string& svrid,
		IN const std::string& userName);
};