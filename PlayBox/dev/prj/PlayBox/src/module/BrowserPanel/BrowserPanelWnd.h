#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"

class MyWebBrowserWnd;
class CxSkinButton;
class CEditEx;
class CLocalSearchEdit;
class CFavUrlMenuDlg;

class BrowserPanelWnd : public CBasicWnd
{
	DECLARE_DYNAMIC(BrowserPanelWnd)
public:
	BrowserPanelWnd();
	virtual ~BrowserPanelWnd();

	void Navigate(string strUrl);
	void SetTabItem(TAB_ITEM& ti);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void	OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnDestroy();
	afx_msg void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void	OnClickedBack();
	afx_msg void	OnClickedForward();
	afx_msg void	OnClickedRefresh();
	afx_msg void	OnClickedFav();	
	afx_msg LRESULT OnPageChanging(WPARAM wParam, LPARAM lParam);

	// WM_MENU_CICKED : (WM_USER+120)
	afx_msg LRESULT OnClickeFavMenuItem(WPARAM wParam, LPARAM lParam);
	// WM_NEWPAGE
	afx_msg LRESULT OnNewPageWindow(WPARAM, LPARAM);

	void	ShowFavUrlMenu();

	//////////////////////////////////////////////////////////////////////////
	// input:
	//		int &nStartItemID		-- menu item ID
	//		const char* szPath		-- .url files' path
	//		CMenu* pMenu			-- pointer to menu
	// return:
	//		int		num of items added
	int AddFavUrlToMenu(int &nStartItemID, int nStartPos, const char* szPath, CMenu* pMenu);
private:
	CxSkinButton*		m_pBtnBack;
	CxSkinButton*		m_pBtnForward;
	CxSkinButton*		m_pBtnRefresh;
	CxSkinButton*		m_pBtnFav;
	CLocalSearchEdit*	m_pEditAddress;
	CFont				m_editFont;
	MyWebBrowserWnd*	m_pWndBrowser;
	CFavUrlMenuDlg*		m_pFavUrlDlg;
	TAB_ITEM			m_tabItem;

	vector<string>		m_arrFavMenuItemParam;

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};