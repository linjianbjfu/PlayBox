#include "stdafx.h"
#include "TabWndFactory.h"
#include "../GamePanel/GamePanelWnd.h"
#include "../GamePanel/WebGamePanelWnd.h"
#include "../WebInteract/MyWebBrowserWnd.h"
#include "../PlayedGamePanel/PlayedGameWnd.h"
#include "../BrowserPanel/BrowserPanelWnd.h"

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
	//目前m_vecWnd里面是空的，每次使用完都会即时销毁
	//for( vector<CWnd*>::iterator it1 = m_vecWnd.begin(); 
	//	it1 != m_vecWnd.end(); it1++ )
	//{
	//	CWnd* pWnd = (CWnd*)(*it1);
	//	if( dynamic_cast<PlayedGameWnd*>(pWnd) )
	//	{
	//		PlayedGameWnd* pWnd1 = (PlayedGameWnd*)pWnd;
	//		pWnd1->DestroyWindow();
	//		delete pWnd1;
	//	}else
	//	if( dynamic_cast<GamePanelWnd*>(pWnd) )
	//	{
	//		GamePanelWnd* pWnd1 = (GamePanelWnd*)pWnd;
	//		pWnd1->DestroyWindow();
	//		delete pWnd1;
	//	}else
	//	if( dynamic_cast<WebGamePanelWnd*>(pWnd) )
	//	{
	//		WebGamePanelWnd* pWnd1 = (WebGamePanelWnd*)pWnd;
	//		pWnd1->DestroyWindow();
	//		delete pWnd1;
	//	}else
	//	if( dynamic_cast<MyWebBrowserWnd*>(pWnd) )
	//	{
	//		MyWebBrowserWnd* pWnd1 = (MyWebBrowserWnd*)pWnd;
	//		pWnd1->DestroyWindow();
	//	}
	//}
}

void TabWndFactory::SetParent(CWnd* pWnd)
{
	m_pWndParent = pWnd;
}

PlayedGameWnd* TabWndFactory::CreateWndPlayedGame( )
{
	PlayedGameWnd* pWnd = NULL;
	for( vector<CWnd*>::iterator it1 = m_vecWnd.begin();
		it1 != m_vecWnd.end(); it1++ )
	{
		if( dynamic_cast<PlayedGameWnd*>(*it1) )
		{
			pWnd = (PlayedGameWnd*)(*it1);
			m_vecWnd.erase( it1 );
			break;
		}
	}

	CRect rc;
	m_pWndParent->GetClientRect( &rc );

	rc.top += 24;

	if( pWnd )
	{
		pWnd->MoveWindow( &rc );
		pWnd->ShowWindow( SW_SHOW ); 
	}else
	{
		pWnd = new PlayedGameWnd();
		pWnd->Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,rc,m_pWndParent, m_idWnd++);
	
		pWnd->ShowWindow (SW_SHOW);
	}
	return pWnd;
}

GamePanelWnd* TabWndFactory::CreateWndGamePanel( )
{
	GamePanelWnd* pWnd = NULL;
	for( vector<CWnd*>::iterator it1 = m_vecWnd.begin();
		it1 != m_vecWnd.end(); it1++ )
	{
		if( dynamic_cast<GamePanelWnd*>(*it1) )
		{
			pWnd = (GamePanelWnd*)(*it1);
			m_vecWnd.erase( it1 );
			break;
		}
	}

	CRect rc;
	m_pWndParent->GetClientRect( &rc );
	if( pWnd )
	{
		pWnd->MoveWindow( &rc );
		pWnd->ShowWindow( SW_SHOW ); 
	}else
	{
		pWnd = new GamePanelWnd();
		pWnd->Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,rc,m_pWndParent, m_idWnd++);
	}
	return pWnd;
}

WebGamePanelWnd* TabWndFactory::CreateWndWebGamePanel( )
{
	WebGamePanelWnd* pWnd = NULL;
	for( vector<CWnd*>::iterator it1 = m_vecWnd.begin();
		it1 != m_vecWnd.end(); it1++ )
	{
		if( dynamic_cast<WebGamePanelWnd*>(*it1) )
		{
			pWnd = (WebGamePanelWnd*)(*it1);
			m_vecWnd.erase( it1 );
			break;
		}
	}

	CRect rc;
	m_pWndParent->GetClientRect( &rc );
	if( pWnd )
	{
		pWnd->MoveWindow( &rc );
		pWnd->ShowWindow( SW_SHOW ); 
	}else
	{
		pWnd = new WebGamePanelWnd();
		pWnd->Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,rc,m_pWndParent, m_idWnd++);
	}
	return pWnd;
}

MyWebBrowserWnd* TabWndFactory::CreateWndMyWebBrowser( )
{
	MyWebBrowserWnd* pWnd = NULL;
	for( vector<CWnd*>::iterator it1 = m_vecWnd.begin();
		it1 != m_vecWnd.end(); it1++ )
	{
		if( dynamic_cast<MyWebBrowserWnd*>(*it1) )
		{
			pWnd = (MyWebBrowserWnd*)(*it1);
			m_vecWnd.erase( it1 );
			break;
		}
	}

	CRect rc;
	m_pWndParent->GetClientRect( &rc );
	if( pWnd == NULL )
	{
		pWnd = new MyWebBrowserWnd();
		CRect rcNull(0,0,0,0);
		pWnd->Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,rcNull,m_pWndParent, m_idWnd++);
	}
	pWnd->MoveWindow( &rc );
	return pWnd;
}

BrowserPanelWnd* TabWndFactory::CreateWndBrowserPanel( )
{
	BrowserPanelWnd* pWnd = NULL;
	for( vector<CWnd*>::iterator it1 = m_vecWnd.begin();
		it1 != m_vecWnd.end(); it1++ )
	{
		if( dynamic_cast<BrowserPanelWnd*>(*it1) )
		{
			pWnd = (BrowserPanelWnd*)(*it1);
			m_vecWnd.erase( it1 );
			break;
		}
	}

	CRect rc;
	m_pWndParent->GetClientRect( &rc );
	if( pWnd == NULL )
	{
		pWnd = new BrowserPanelWnd();
		CRect rcNull(0,0,0,0);
		pWnd->Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,rcNull,m_pWndParent, m_idWnd++);
	}
	pWnd->MoveWindow( &rc );
	return pWnd;
}

void TabWndFactory::Recycle( CWnd* pWnd )
{
	//目前m_vecWnd里面是空的，每次使用完都会即时销毁
	if( dynamic_cast<GamePanelWnd*>(pWnd) )
	{
		GamePanelWnd* pWnd1 = (GamePanelWnd*)pWnd;
		//pWnd1->Recycle();
		pWnd1->DestroyWindow();
		delete pWnd1;
	}else
	if( dynamic_cast<WebGamePanelWnd*>(pWnd) )
	{
		WebGamePanelWnd* pWnd1 = (WebGamePanelWnd*)pWnd;
		//pWnd1->Recycle();
		pWnd1->DestroyWindow();
		delete pWnd1;
	}else
	if( dynamic_cast<MyWebBrowserWnd*>(pWnd) )
	{
		MyWebBrowserWnd* pWnd1 = (MyWebBrowserWnd*)pWnd;
		//pWnd1->Recycle();
		pWnd1->DestroyWindow();
		//不能delete，否则出错
	}else
	if( dynamic_cast<PlayedGameWnd*>(pWnd) )
	{
		PlayedGameWnd* pWnd1 = (PlayedGameWnd*)pWnd;
		//pWnd1->Recycle();
		pWnd1->DestroyWindow();
		delete pWnd1;
	}
	//m_vecWnd.push_back( pWnd );
	//pWnd->MoveWindow(0,0,0,0);
}