#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"

class MyWebBrowserWnd;
class CxSkinButton;
class CEditEx;

class GameCenterPanelWnd : public CBasicWnd
{
	DECLARE_DYNAMIC(GameCenterPanelWnd)
public:
	GameCenterPanelWnd();
	virtual ~GameCenterPanelWnd();

	void Navigate();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnDestroy();
	afx_msg void	OnClickedMainPage();
	afx_msg void	OnClickedFlashGame();
	afx_msg void	OnClickedWebGame();
	afx_msg void	OnClickedSearchBtn();	
private:
	CxSkinButton*		m_pBtnMainPage;
	CxSkinButton*		m_pBtnFlashGame;
	CxSkinButton*		m_pBtnWebGame;
	CxSkinButton*		m_pBtnSearchBtn;
	CEditEx*			m_pEditSearch;
	MyWebBrowserWnd*	m_pWndBrowser;

	string		m_strUrlMainPage;
	string		m_strUrlFlashGame;
	string		m_strUrlWebGame;
	string		m_strUrlSearch;
	enum PageType
	{
		PT_MAIN_PAGE = 0,
		PT_FLASH_GAME,
		PT_WEB_GAME,
		PT_SEARCH
	};
	PageType	m_curPageType;
	void GoToURL(PageType pt);
};