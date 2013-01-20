#pragma once
#include "../../gui/CommonControl/BasicWnd.h"
#include "../../datainterface/IGameData.h"
#include "../../datainterface/IUserMsgObserver.h"

class CxSkinButton;
class PlayedGameListPanelWnd;
class MyWebBrowserWnd;
class CUserLogedInWnd;
class CUserLogedOutWnd;

class PlayedGameWnd : public CBasicWnd, public IUserMsgObserver
{
	DECLARE_DYNAMIC(PlayedGameWnd)
public:
	PlayedGameWnd();
	virtual ~PlayedGameWnd();
protected:
	time_t str2time(const string & strTime); // string like: "2013-1-13 12:50"
	bool m_bSortTimeByAsc;

	DECLARE_MESSAGE_MAP()
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnDestroy();
	afx_msg void	OnClickedTimerOrder();
	afx_msg void	OnClickedToWebGame();
	afx_msg void	OnClickedToFlashGame();
	afx_msg void	OnClickedToCollectedGame();

private:
	CxSkinButton*	m_pBtnTimeOrder;
	CxSkinButton*	m_pBtnToWebGame;
	CxSkinButton*	m_pBtnToFlashGame;
	CxSkinButton*	m_pBtnToCollectedGame;
	PlayedGameListPanelWnd*	m_pWndGameListWnd;
	MyWebBrowserWnd*	m_pWndRecommand;
	CUserLogedInWnd*	m_pWndLogedIn;
	CUserLogedOutWnd*	m_pWndLogedOut;
	GameList	m_gameList;
	int m_iGameType;

	void RefreshData();
	void UserMsg_Login();
	void UserMsg_LogOut();
};