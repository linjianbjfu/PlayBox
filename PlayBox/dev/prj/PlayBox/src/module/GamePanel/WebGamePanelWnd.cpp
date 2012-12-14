#include "stdafx.h"
#include "YL_StringUtil.h"
#include "WebGamePanelWnd.h"
#include "../WebInteract/MyWebBrowserWnd.h"
#include "../../AppConfig/config/ConfigAppDef.h"
#include "WebGamePlayingWnd.h"
#include "../../core/CDataManager.h"
#include "../WebInteract/WebManager.h"

IMPLEMENT_DYNAMIC(WebGamePanelWnd, CBasicWnd)
WebGamePanelWnd::WebGamePanelWnd()
{
	m_isMainWindowTopMost = false;
	m_bFullScreen = false;

	m_pWndGameRight		= new MyWebBrowserWnd();
	m_pWndGamePlaying	= new WebGamePlayingWnd();

	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*) this);
}

WebGamePanelWnd::~WebGamePanelWnd()
{
	delete m_pWndGamePlaying;
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*) this);
}

BEGIN_MESSAGE_MAP(WebGamePanelWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


int WebGamePanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	CRect rectNull(0,0,0,0);

	m_pWndGameRight->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,rectNull,this,ID_WND_WEBGAMERIGHT );
	m_pWndGamePlaying->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,rectNull,this,ID_WND_WEBGAMEPLAYING );

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	
	pLayoutMgr->RegisterCtrl( this, "WebGamePlaying", m_pWndGamePlaying );
	pLayoutMgr->RegisterCtrl( this, "WebGameRight", m_pWndGameRight );

	pLayoutMgr->CreateControlPane( this, "webgamepanel", "normal" );
	pLayoutMgr->CreateBmpPane( this,"webgamepanel","normal" );

	return 0;
}

void WebGamePanelWnd::LoadSkin()
{
}

void WebGamePanelWnd::SetTabItem( TAB_ITEM ti )
{
	m_webGame.strID = CWebManager::GetInstance()->GetValue( ti.strParam, "id" );
	m_webGame.strName = ti.strName;
	m_webGame.strGameUrl = CWebManager::GetInstance()->GetValue( ti.strParam, "url" );
	
	string strRightUrl;
	m_pWndGameRight->Navigate( strRightUrl );
	m_pWndGamePlaying->SetGameEntry( m_webGame );
}

void WebGamePanelWnd::Init()
{
	m_pWndGameRight->Init();
	m_pWndGamePlaying->Init();
}

void WebGamePanelWnd::OnDestroy()
{
	m_pWndGamePlaying->DestroyWindow();
	m_pWndGameRight->DestroyWindow();
	__super::OnDestroy();
}

void WebGamePanelWnd::IPanelChangeOb_ToFullScreen( CWnd* pWnd )
{
	YL_Log("WebGamePanelWnd.txt", LOG_DEBUG, "WebGamePanelWnd::IPanelChangeOb_ToFullScreen", "===IN");
	if( pWnd != this )
		return;

	m_isMainWindowTopMost = false;
	bool curState;
	AfxGetUserConfig()->GetConfigBoolValue( CONF_APP_MODULE_NAME,CONF_APP_MAINWND_HOLD,curState);
	if(curState)
	{
		SetMainWindow(false);
		m_isMainWindowTopMost=true;
	}

	m_pWndParent = GetParent();
	GetWindowRect(&m_rectBeforeFull);
	m_pWndParent->ScreenToClient(&m_rectBeforeFull);

	//去掉子窗口风格,这样WebGamePanelWnd才能全屏时获得焦点
	long style = ::GetWindowLong(m_hWnd,GWL_STYLE);
	style &= ~WS_CHILD;
	//style &= ~WS_CLIPCHILDREN;
	::SetWindowLong(m_hWnd, GWL_STYLE, style);

	long lExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	lExStyle &= ~WS_EX_APPWINDOW; 
	lExStyle |= WS_EX_TOOLWINDOW; 
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, lExStyle);

	SetParent(GetDesktopWindow());
	GetDesktopWindow()->GetWindowRect(&m_rectFullScreen);

	SetWindowPos(&wndTopMost,m_rectFullScreen.left,m_rectFullScreen.top,
		m_rectFullScreen.Width(),m_rectFullScreen.Height(),SWP_SHOWWINDOW);
	m_bFullScreen = true;

	YL_Log("WebGamePanelWnd.txt", LOG_DEBUG, "WebGamePanelWnd::IPanelChangeOb_ToFullScreen", "===OUT");
}

void WebGamePanelWnd::SetMainWindow(bool isTopMost)
{
	YL_Log("WebGamePanelWnd.txt", LOG_DEBUG, "WebGamePanelWnd::SetMainWindow", "===IN");
	if(isTopMost)
	{
		AfxGetMainWindow()->SetWindowPos(&wndTopMost,-1,-1,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
		AfxGetUserConfig()->SetConfigBoolValue( CONF_APP_MODULE_NAME,CONF_APP_MAINWND_HOLD,true);
	}else
	{
		AfxGetMainWindow()->SetWindowPos(&wndNoTopMost,-1,-1,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
		AfxGetUserConfig()->SetConfigBoolValue( CONF_APP_MODULE_NAME,CONF_APP_MAINWND_HOLD,false);
	}
	YL_Log("WebGamePanelWnd.txt", LOG_DEBUG, "WebGamePanelWnd::SetMainWindow", "===OUT");
}

void WebGamePanelWnd::IPanelChangeOb_ExitFullScreen( CWnd* pWnd )
{
	YL_Log("WebGamePanelWnd.txt", LOG_DEBUG, "WebGamePanelWnd::IPanelChangeOb_ExitFullScreen", "===IN");
	if( pWnd != this )
		return;

	m_bFullScreen = false;

	//加上子窗口风格
	long style = ::GetWindowLong(m_hWnd,GWL_STYLE);
	style |= WS_CHILD;
	//style |= WS_CLIPCHILDREN; //去掉WS_CLIPCHILDREN后，WebGamePanelWnd刷新时就会有剧烈闪烁
	::SetWindowLong(m_hWnd, GWL_STYLE, style);
	
	long lExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	lExStyle |= WS_EX_APPWINDOW; 
	lExStyle &= ~WS_EX_TOOLWINDOW; 
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, lExStyle);

	SetParent(m_pWndParent);
	MoveWindow(&m_rectBeforeFull);

	if(m_isMainWindowTopMost)
	{
		SetMainWindow(true);
		m_isMainWindowTopMost = false;
	}

	bool bHold = false;
	AfxGetUserConfig()->GetConfigBoolValue( CONF_APP_MODULE_NAME,CONF_APP_MAINWND_HOLD,bHold);
	if( bHold )
	{
		AfxGetMainWindow()->SetWindowPos(&wndTopMost,-1,-1,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	}
	YL_Log("WebGamePanelWnd.txt", LOG_DEBUG, "WebGamePanelWnd::IPanelChangeOb_ExitFullScreen", "===OUT");
}

BOOL WebGamePanelWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void WebGamePanelWnd::Recycle()
{
	Init();
}