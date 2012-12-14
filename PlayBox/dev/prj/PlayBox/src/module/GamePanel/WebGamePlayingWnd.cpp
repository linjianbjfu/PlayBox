#include "stdafx.h"
#include "WebGamePlayingWnd.h"
#include "YL_StringUtil.h"
#include "tools.h"
#include "../../gui/CommonControl/xSkinButton.h"
#include "../../core/CDataManager.h"
#include "../WebInteract/MyWebBrowserWnd.h"

#define TEXT_TO_FULLSCREEN		"进入全屏"
#define TEXT_EXIT_FULLSCREEN	"退出全屏"

#define TEXT_OPEN_INFO			"打开说明"
#define TEXT_CLOSE_INFO			"关闭说明"

#define WIDTH_RIGHT_WEB			220

IMPLEMENT_DYNAMIC(WebGamePlayingWnd, CBasicWnd)
WebGamePlayingWnd::WebGamePlayingWnd()
{
	m_pBtnFullScreen	= new CxSkinButton();
	m_pBtnGameRight		= new CxSkinButton();
	m_pWndWebGame		= new MyWebBrowserWnd();
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_LAYOUTMGR,(ILayoutChangeObserver*) this);
}

WebGamePlayingWnd::~WebGamePlayingWnd()
{
	delete m_pBtnFullScreen;
	delete m_pBtnGameRight;

	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_LAYOUTMGR,(ILayoutChangeObserver*) this);
}

BEGIN_MESSAGE_MAP(WebGamePlayingWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_FULL_SCREEN,OnClickedFullScreen)
	ON_BN_CLICKED(IDC_BTN_GAMERIGHT,OnClickedGameRight)
END_MESSAGE_MAP()

int WebGamePlayingWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	YL_Log("WebGamePlayingWnd.txt", LOG_DEBUG, "WebGamePlayingWnd::OnCreate", "===IN");
	if(__super::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	CRect rectNull(0,0,0,0);

	m_pBtnFullScreen->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_FULL_SCREEN);
	m_pBtnGameRight->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_GAMERIGHT);

	m_pWndWebGame->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_WND_WEBGAME );

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	
	pLayoutMgr->RegisterCtrl( this, "WebGameView", m_pWndWebGame );
	pLayoutMgr->RegisterCtrl( this, "WebGameToFull", m_pBtnFullScreen );
	pLayoutMgr->RegisterCtrl( this, "WebGameInfo", m_pBtnGameRight );
	
	pLayoutMgr->CreateControlPane( this, "webgameplaying", "normal" );
	pLayoutMgr->CreateBmpPane( this,"webgameplaying","normal" );

	UpdateAllWnd();

	YL_Log("WebGamePlayingWnd.txt", LOG_DEBUG, "WebGamePlayingWnd::OnCreate", "===OUT");
	return 0;
}

void WebGamePlayingWnd::LoadSkin()
{
}

void WebGamePlayingWnd::Init()
{
	YL_Log("WebGamePlayingWnd.txt", LOG_DEBUG, "WebGamePlayingWnd::Init", "===IN");
	m_pWndWebGame->Init();
	//默认打开说明页
	CRect rc;
	AfxGetUIManager()->UIGetLayoutMgr()->GetPaneItemRect( GetParent(), "WebGameRight", rc );
	if( rc.IsRectEmpty() )
	{
		rc.left = rc.right - WIDTH_RIGHT_WEB;
		AfxGetUIManager()->UIGetLayoutMgr()->SetPaneSize( GetParent(), "WebGameRight", rc , false);
	}	
	YL_Log("WebGamePlayingWnd.txt", LOG_DEBUG, "WebGamePlayingWnd::Init", "===OUT");
}

void WebGamePlayingWnd::SetGameEntry( WEB_GAME wg )
{
	YL_Log("WebGamePlayingWnd.txt", LOG_DEBUG, "WebGamePlayingWnd::SetGameEntry", "===IN");

	m_webGame = wg;
	m_pWndWebGame->Navigate( m_webGame.strGameUrl );
	UpdateAllWnd();
	YL_Log("WebGamePlayingWnd.txt", LOG_DEBUG, "WebGamePlayingWnd::SetGameEntry", "===OUT");
}

void WebGamePlayingWnd::OnClickedFullScreen()
{
	YL_Log("WebGamePlayingWnd.txt", LOG_DEBUG, "WebGamePlayingWnd::OnClickedFullScreen", "===IN");
	m_pWndWebGame->SetFocus();
	
	if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
	{
		GLOBAL_PANELCHANGEDATA->IPanelChange_ExitFullScreen();
	}else
	{
		GLOBAL_PANELCHANGEDATA->IPanelChange_ToFullScreen( GetParent() );
	}	
	YL_Log("WebGamePlayingWnd.txt", LOG_DEBUG, "WebGamePlayingWnd::OnClickedFullScreen", "===OUT");
}

void WebGamePlayingWnd::OnDestroy()
{
	YL_Log("WebGamePlayingWnd.txt", LOG_DEBUG, "WebGamePlayingWnd::OnDestroy", "===IN");
	m_pWndWebGame->DestroyWindow();
	__super::OnDestroy();
	YL_Log("WebGamePlayingWnd.txt", LOG_DEBUG, "WebGamePlayingWnd::OnDestroy", "===OUT");
}

void WebGamePlayingWnd::OnClickedGameRight()
{
	CRect rc;
	AfxGetUIManager()->UIGetLayoutMgr()->GetPaneItemRect( GetParent(), "WebGameRight", rc );
	if( !rc.IsRectEmpty() )
	{
		rc.left = rc.right;
	}else
	{
		rc.left = rc.right - WIDTH_RIGHT_WEB;
	}
	AfxGetUIManager()->UIGetLayoutMgr()->SetPaneSize( GetParent(), "WebGameRight", rc , false);
	AfxGetUIManager()->UIGetLayoutMgr()->UpdateLayout( GetParent()->GetSafeHwnd() );
	UpdateAllWnd();
}

void WebGamePlayingWnd::UpdateAllWnd()
{
	YL_Log("WebGamePlayingWnd.txt", LOG_DEBUG, "WebGamePlayingWnd::UpdateAllWnd", "===IN");

	//全屏
	if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
	{
		m_pBtnFullScreen->SetWindowText( TEXT_EXIT_FULLSCREEN );
	}else
	{
		m_pBtnFullScreen->SetWindowText( TEXT_TO_FULLSCREEN );
	}
	//说明
	CRect rc;
	AfxGetUIManager()->UIGetLayoutMgr()->GetPaneItemRect( GetParent(), "WebGameRight", rc );
	if( !rc.IsRectEmpty() )
	{
		m_pBtnGameRight->SetWindowText( TEXT_CLOSE_INFO );
		m_pBtnGameRight->SetCheck( 1 );
	}else
	{
		m_pBtnGameRight->SetWindowText( TEXT_OPEN_INFO );
		m_pBtnGameRight->SetCheck( 0 );
	}

	YL_Log("WebGamePlayingWnd.txt", LOG_DEBUG, "WebGamePlayingWnd::UpdateAllWnd", "===OUT");
}


void WebGamePlayingWnd::ILayoutChangeOb_InitFinished()
{
	YL_Log("GamePlayingWnd.txt", LOG_DEBUG, "GamePlayingWnd::ILayoutChangeOb_InitFinished", "===IN");
	UpdateAllWnd();
	YL_Log("GamePlayingWnd.txt", LOG_DEBUG, "GamePlayingWnd::ILayoutChangeOb_InitFinished", "===OUT");
}
void WebGamePlayingWnd::ILayoutChangeOb_SkinChanged(string oldSkinPath, string newSkinPath)
{
	YL_Log("GamePlayingWnd.txt", LOG_DEBUG, "GamePlayingWnd::ILayoutChangeOb_SkinChanged", "===IN");
	UpdateAllWnd();
	YL_Log("GamePlayingWnd.txt", LOG_DEBUG, "GamePlayingWnd::ILayoutChangeOb_SkinChanged", "===OUT");
}

void WebGamePlayingWnd::ILayoutChangeOb_UpdateLayout(HWND hWnd)
{
	YL_Log("GamePlayingWnd.txt", LOG_DEBUG, "GamePlayingWnd::ILayoutChangeOb_UpdateLayout", "===IN");
	if( hWnd == GetSafeHwnd() )
	{
		UpdateAllWnd();
	}
	YL_Log("GamePlayingWnd.txt", LOG_DEBUG, "GamePlayingWnd::ILayoutChangeOb_UpdateLayout", "===OUT");
}