#pragma once
#include <vector>
using std::vector;

class PlayedGameWnd;
class GamePanelWnd;
class WebGamePanelWnd;
class BrowserPanelWnd;
class MyWebBrowserWnd;

class TabWndFactory
{
private:
	CWnd*				m_pWndParent;
	int					m_idWnd;	//´°¿ÚID
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
	MyWebBrowserWnd*	CreateWndGameCenterPanel( );
	BrowserPanelWnd*	CreateWndBrowserPanel( );
	void Recycle( CWnd* pWnd );
};