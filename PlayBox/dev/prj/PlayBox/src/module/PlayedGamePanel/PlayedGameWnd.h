#pragma once
#include "../../gui/CommonControl/BasicWnd.h"
#include "../../datainterface/ILocalGameData.h"

class CxSkinButton;
class PlayedGameListPanelWnd;
class MyWebBrowserWnd;
class CUserLogedInWnd;
class CUserLogedOutWnd;

class PlayedGameWnd : public CBasicWnd
{
	DECLARE_DYNAMIC(PlayedGameWnd)
public:
	PlayedGameWnd();
	virtual ~PlayedGameWnd();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnDestroy();
	afx_msg void	OnClickedTimerOrder();
	afx_msg void	OnClickedToWebGame();
	afx_msg void	OnClickedToFlashGame();
	afx_msg void	OnClickedToDownloadManager();
	afx_msg void	OnClickedToCollectedGame();
	afx_msg void	OnClickedToDelete();

private:
	CxSkinButton*	m_pBtnTimeOrder;
	CxSkinButton*	m_pBtnToWebGame;
	CxSkinButton*	m_pBtnToFlashGame;
	CxSkinButton*	m_pBtnToDownloadManager;
	CxSkinButton*	m_pBtnToCollectedGame;
	CxSkinButton*	m_pBtnToDelete;
	PlayedGameListPanelWnd*	m_pWndGameListWnd;
	MyWebBrowserWnd*	m_pWndRecommand;
	CUserLogedInWnd*	m_pWndLogedIn;
	CUserLogedOutWnd*	m_pWndLogedOut;
	LocalGameList	m_gameList;
};