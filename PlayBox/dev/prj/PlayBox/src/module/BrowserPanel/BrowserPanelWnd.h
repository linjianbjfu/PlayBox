#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"

class MyWebBrowserWnd;
class CxSkinButton;
class CEditEx;

class BrowserPanelWnd : public CBasicWnd
{
	DECLARE_DYNAMIC(BrowserPanelWnd)
public:
	BrowserPanelWnd();
	virtual ~BrowserPanelWnd();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void	OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnDestroy();
	afx_msg void	OnClickedBack();
	afx_msg void	OnClickedForward();
	afx_msg void	OnClickedRefresh();
	afx_msg void	OnClickedFav();	
private:
	CxSkinButton*		m_pBtnBack;
	CxSkinButton*		m_pBtnForward;
	CxSkinButton*		m_pBtnRefresh;
	CxSkinButton*		m_pBtnFav;
	CEditEx*			m_pEditAddress;
	MyWebBrowserWnd*	m_pWndBrowser;
};