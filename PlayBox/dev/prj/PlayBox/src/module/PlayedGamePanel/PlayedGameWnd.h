#pragma once
#include "../../gui/CommonControl/BasicWnd.h"
#include "../../datainterface/IGameData.h"
#include "../../datainterface/IUserMsgObserver.h"
#include "../../datainterface/IGameDataObserver.h"

class CxSkinButton;
class PlayedGameListPanelWnd;
class MyWebBrowserWnd;
class CUserLogedInWnd;
class CUserLogedOutWnd;

class PlayedGameWnd : public CBasicWnd, 
					  public IUserMsgObserver,
					  public IGameDataObserver
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
	afx_msg void	OnPaint();
	afx_msg void	OnDestroy();
	//这个函数的意义发生变化，不是按时间排序，而是现实所有的最近玩过的游戏
	afx_msg void	OnClickedTimerOrder();
	afx_msg void	OnClickedToWebGame();
	afx_msg void	OnClickedToFlashGame();
	afx_msg void	OnClickedToCollectedGame();

private:
	enum TextType
	{
		RECENT_PLAY = 0,
		FLASH_GAME,
		WEB_GAME,
		COLLECT_GAME,
		TT_END,
	};

	CDibBitmap*	m_pTextBmp[TT_END];
	TextType m_textType;

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

	void UserMsg_Login();
	void UserMsg_LogOut();
	void SetOnlyOneBtnCheckedAndValidate(CxSkinButton* pBtn);
	void ValidateInterface();
	void IGameData_Changed();
};