#include "stdafx.h"
#include "resource.h"
#include "YL_StringUtil.h"
#include "WebGamePanelWnd.h"
#include "../WebInteract/MyWebBrowserWnd.h"
#include "../../AppConfig/config/ConfigAppDef.h"
#include "../WebInteract/WebManager.h"
#include "../../core/CDataManager.h"
#include "../../gui/CommonControl/xSkinButton.h"
#include "../../gui/CommonControl/xStaticText.h"
#include "../../Gui/CommonControl/EditEx.h"

IMPLEMENT_DYNAMIC(WebGamePanelWnd, CBasicWnd)
WebGamePanelWnd::WebGamePanelWnd()
{
	m_isMainWindowTopMost = false;
	m_bFullScreen = false;

	m_pWndWebGame	= new MyWebBrowserWnd();
	m_pBtnRefresh	= new CxSkinButton();
	m_pBtnToFull	= new CxSkinButton();
	m_pBtnExitFull	= new CxSkinButton();
	m_pBtnMute	= new CxSkinButton();
	m_pBtnUnMute	= new CxSkinButton();
	m_pBtnClearCache	= new CxSkinButton();
	m_pBtnSite	= new CxSkinButton();
	m_pBtnCustomService	= new CxSkinButton();
	m_pBtnPay	= new CxSkinButton();

	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*) this);
}

WebGamePanelWnd::~WebGamePanelWnd()
{
	delete m_pBtnRefresh;
	delete m_pBtnToFull;
	delete m_pBtnExitFull;
	delete m_pBtnMute;
	delete m_pBtnUnMute;
	delete m_pBtnClearCache;
	delete m_pBtnSite;
	delete m_pBtnCustomService;
	delete m_pBtnPay;
	//do not delete m_pWndWebGame
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*) this);
}

BEGIN_MESSAGE_MAP(WebGamePanelWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BTN_WEBGAME_REFRESH,OnClickedRefresh)
	ON_BN_CLICKED(IDC_BTN_WEBGAME_TO_FULL,OnClickedToFull)
	ON_BN_CLICKED(IDC_BTN_WEBGAME_EXIT_FULL,OnClickedExitFull)
	ON_BN_CLICKED(IDC_BTN_WEBGAME_MUTE,OnClickedMute)
	ON_BN_CLICKED(IDC_BTN_WEBGAME_UNMUTE,OnClickedUnMute)
	ON_BN_CLICKED(IDC_BTN_WEBGAME_CLEAR_CACHE,OnClickedClearCache)
	ON_BN_CLICKED(IDC_BTN_WEBGAME_SITE,OnClickedSite)
	ON_BN_CLICKED(IDC_BTN_WEBGAME_CUSTOM_SERVICE,OnClickedCustomService)
	ON_BN_CLICKED(IDC_BTN_WEBGAME_PAY,OnClickedPay)
END_MESSAGE_MAP()

int WebGamePanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	CRect rectNull(0,0,0,0);
	m_pWndWebGame->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_WND_WEBGAMEPLAYING );
	m_pBtnRefresh->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_REFRESH);
	m_pBtnToFull->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_TO_FULL);
	m_pBtnExitFull->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_EXIT_FULL);
	m_pBtnMute->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_MUTE);
	m_pBtnUnMute->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_UNMUTE);
	m_pBtnClearCache->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_CLEAR_CACHE);
	m_pBtnSite->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_SITE);
	m_pBtnCustomService->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_CUSTOM_SERVICE);
	m_pBtnPay->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_PAY);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->RegisterCtrl( this, "WebGameRefresh", m_pBtnRefresh );
	pLayoutMgr->RegisterCtrl( this, "WebGameToFull", m_pBtnToFull );
	pLayoutMgr->RegisterCtrl( this, "WebGameToSmall", m_pBtnExitFull );
	pLayoutMgr->RegisterCtrl( this, "WebGameMute", m_pBtnMute );
	pLayoutMgr->RegisterCtrl( this, "WebGameUnMute", m_pBtnUnMute );
	pLayoutMgr->RegisterCtrl( this, "WebGameClearCache", m_pBtnClearCache );
	pLayoutMgr->RegisterCtrl( this, "WebGameSite", m_pBtnSite );
	pLayoutMgr->RegisterCtrl( this, "WebGameCustomService", m_pBtnCustomService );
	pLayoutMgr->RegisterCtrl( this, "WebGamePay", m_pBtnPay );
	pLayoutMgr->RegisterCtrl( this, "WebGameBrowser", m_pWndWebGame );

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
	
	m_pWndWebGame->Navigate(m_webGame.strGameUrl);
}

void WebGamePanelWnd::Init()
{
	m_pWndWebGame->Init();
}

void WebGamePanelWnd::OnDestroy()
{
	m_pWndWebGame->DestroyWindow();
	__super::OnDestroy();
}

void WebGamePanelWnd::IPanelChangeOb_ToFullScreen( CWnd* pWnd )
{
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

void WebGamePanelWnd::OnClickedRefresh()
{
	m_pWndWebGame->Refresh();
}

void WebGamePanelWnd::OnClickedToFull()
{

}

void WebGamePanelWnd::OnClickedExitFull()
{

}

void WebGamePanelWnd::OnClickedMute()
{
	bool bIsMute = CSound::GetInstance()->GetMute( 1 );
	CSound::GetInstance()->SetMute( 1, !bIsMute );
	UpdateAllWnd();
}

void WebGamePanelWnd::OnClickedUnMute()
{
	bool bIsMute = CSound::GetInstance()->GetMute( 1 );
	CSound::GetInstance()->SetMute( 1, !bIsMute );
	UpdateAllWnd();
}

void WebGamePanelWnd::OnClickedClearCache()
{

}

void WebGamePanelWnd::OnClickedSite()
{

}

void WebGamePanelWnd::OnClickedCustomService()
{

}

void WebGamePanelWnd::OnClickedPay()
{

}
