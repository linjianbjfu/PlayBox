#pragma once
#include <vector>
using std::vector;

class PlayedGameWnd;
class GamePanelWnd;
class WebGamePanelWnd;
class GameCenterPanelWnd;
class BrowserPanelWnd;

class TabWndFactory
{
private:
	CWnd*				m_pWndParent;
	int					m_idWnd;	//����ID
public:
	static TabWndFactory*	m_pSelf;
	static TabWndFactory*	GetInstance();
	static void				DelInstance();
	
	TabWndFactory();
	~TabWndFactory();

public:
	void	SetParent( CWnd* pWndParent );
	PlayedGameWnd*		CreateWndPlayedGame( );
	GamePanelWnd*		CreateWndGamePanel( );
	WebGamePanelWnd*	CreateWndWebGamePanel( );
	GameCenterPanelWnd*	CreateWndGameCenterPanel( );
	BrowserPanelWnd*	CreateWndBrowserPanel( );
	void Recycle( CWnd* pWnd );
};