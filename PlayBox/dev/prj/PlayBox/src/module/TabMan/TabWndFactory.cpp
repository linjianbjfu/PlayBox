#include "stdafx.h"
#include "TabWndFactory.h"
#include "../GamePanel/GamePanelWnd.h"
#include "../GamePanel/WebGamePanelWnd.h"
#include "../WebInteract/MyWebBrowserWnd.h"
#include "../PlayedGamePanel/PlayedGameWnd.h"
#include "../BrowserPanel/BrowserPanelWnd.h"
#include "../GameCenterPanel/GameCenterPanelWnd.h"

TabWndFactory * TabWndFactory::m_pSelf = NULL;

TabWndFactory* TabWndFactory::GetInstance()
{
	if( m_pSelf == NULL )
	{
		m_pSelf = new TabWndFactory;
	}
	return m_pSelf;
}

void TabWndFactory::DelInstance()
{
	if( m_pSelf != NULL )
	{
		delete m_pSelf;
		m_pSelf = NULL;
	}
}

TabWndFactory::TabWndFactory()
{
	m_idWnd = 1000;
}

TabWndFactory::~TabWndFactory()
{
}

void TabWndFactory::SetParent(CWnd* pWnd)
{
	m_pWndParent = pWnd;
}

PlayedGameWnd* TabWndFactory::CreateWndPlayedGame( )
{
	CRect rc;
	m_pWndParent->GetClientRect( &rc );
	rc.top += 24;

	PlayedGameWnd* pWnd = new PlayedGameWnd();
	pWnd->Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,rc,m_pWndParent, m_idWnd++);
	pWnd->ShowWindow (SW_SHOW);
	return pWnd;
}

#define CREATE_SPECIAL_PANEL(PANEL_TYPE) \
	CRect rc; \
	m_pWndParent->GetClientRect( &rc ); \
	PANEL_TYPE* pWnd = new PANEL_TYPE(); \
	pWnd->Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,rc,m_pWndParent, m_idWnd++); \
	return pWnd;

GamePanelWnd* TabWndFactory::CreateWndGamePanel( )
{
	CREATE_SPECIAL_PANEL(GamePanelWnd);
}

WebGamePanelWnd* TabWndFactory::CreateWndWebGamePanel( )
{
	CREATE_SPECIAL_PANEL(WebGamePanelWnd);
}

GameCenterPanelWnd* TabWndFactory::CreateWndGameCenterPanel( )
{
	CREATE_SPECIAL_PANEL(GameCenterPanelWnd);
}

BrowserPanelWnd* TabWndFactory::CreateWndBrowserPanel( )
{
	CRect rc;
	m_pWndParent->GetClientRect( &rc );
	BrowserPanelWnd* pWnd = new BrowserPanelWnd();
	CRect rcNull(0,0,0,0);
	pWnd->Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,rcNull,m_pWndParent, m_idWnd++);
	pWnd->MoveWindow( &rc );
	return pWnd;
}

void TabWndFactory::Recycle( CWnd* pWnd )
{
	pWnd->DestroyWindow();
	if( dynamic_cast<GamePanelWnd*>(pWnd) )
	{
		GamePanelWnd* pWnd1 = (GamePanelWnd*)pWnd;
		delete pWnd1;
	}else
	if( dynamic_cast<WebGamePanelWnd*>(pWnd) )
	{
		WebGamePanelWnd* pWnd1 = (WebGamePanelWnd*)pWnd;
		delete pWnd1;
	}else
	if( dynamic_cast<MyWebBrowserWnd*>(pWnd) )
	{
		MyWebBrowserWnd* pWnd1 = (MyWebBrowserWnd*)pWnd;
		//不能delete，否则出错
	}else
	if( dynamic_cast<PlayedGameWnd*>(pWnd) )
	{
		PlayedGameWnd* pWnd1 = (PlayedGameWnd*)pWnd;
		delete pWnd1;
	}else
	if( dynamic_cast<BrowserPanelWnd*>(pWnd) )
	{
		BrowserPanelWnd* pWnd1 = (BrowserPanelWnd*)pWnd;
		delete pWnd1;
	}
}