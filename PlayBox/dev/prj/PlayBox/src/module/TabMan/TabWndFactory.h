#pragma once
#include <vector>
using std::vector;

class PlayedGameWnd;
class GamePanelWnd;
class WebGamePanelWnd;
class MyWebBrowserWnd;
class BrowserPanelWnd;

class TabWndFactory
{
private:
	vector<CWnd*>		m_vecWnd;	//Wnd»º´æ
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
	MyWebBrowserWnd*	CreateWndMyWebBrowser( );
	BrowserPanelWnd*	CreateWndBrowserPanel( );
	void Recycle( CWnd* pWnd );
};