#include "stdafx.h"
#include "GameCtrlWnd.h"
#include "DownPercentWnd.h"
#include "YL_StringUtil.h"
#include "YL_FileInfo.h"
#include "tools.h"
#include "../../gui/CommonControl/xSkinButton.h"
#include "../../core/CDataManager.h"
#include "../WebInteract/MyWebBrowserWnd.h"
#include "../../util/Sound.h"
#include "../../Global/GlobalVariable.h"
#include "../../AppConfig/config/ConfigAppDef.h"


IMPLEMENT_DYNAMIC(CGameCtrlWnd, CBasicWnd)
CGameCtrlWnd::CGameCtrlWnd()
{
	m_pBtnRePlay		    = new CxSkinButton();
	m_pBtnToFullScreen		= new CxSkinButton();
	m_pBtnExitFullScreen	= new CxSkinButton();
	m_pBtnMute				= new CxSkinButton();
	m_pBtnUnMute			= new CxSkinButton();
	m_pBtnCut		        = new CxSkinButton();	
}

CGameCtrlWnd::~CGameCtrlWnd()
{
	delete m_pBtnRePlay;
	delete m_pBtnToFullScreen;
	delete m_pBtnExitFullScreen;
	delete m_pBtnMute;
	delete m_pBtnUnMute;
	delete m_pBtnCut;
}

BEGIN_MESSAGE_MAP(CGameCtrlWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_REPLAY,OnClickedReplay)
	ON_BN_CLICKED(IDC_BTN_FULL_SCREEN,OnClickedFullScreen)
	ON_BN_CLICKED(IDC_BTN_EXIT_FULL_SCREEN,OnClickedExitFullScreen)
	ON_BN_CLICKED(IDC_BTN_MUTE,OnClickedMute)
	ON_BN_CLICKED(IDC_BTN_UN_MUTE,OnClickedUnMute)
	ON_BN_CLICKED(IDC_BTN_CUT,OnClickedCut)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


int CGameCtrlWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	YL_Log("CGameCtrlWnd.txt", LOG_DEBUG, "CGameCtrlWnd::OnCreate", "===IN");
	if(__super::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	CRect rectNull(0,0,0,0);

	m_pBtnRePlay->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_REPLAY);
	m_pBtnToFullScreen->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_FULL_SCREEN);
	m_pBtnExitFullScreen->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_EXIT_FULL_SCREEN);
	m_pBtnMute->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_MUTE);
	m_pBtnUnMute->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_UN_MUTE);
	m_pBtnCut->Create (NULL, WS_VISIBLE, rectNull, this, IDC_BTN_CUT);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	
	pLayoutMgr->RegisterCtrl( this, "GameRePlay", m_pBtnRePlay );
	pLayoutMgr->RegisterCtrl( this, "ToFull", m_pBtnToFullScreen );
	pLayoutMgr->RegisterCtrl( this, "ExitFull", m_pBtnExitFullScreen );
	pLayoutMgr->RegisterCtrl( this, "Mute", m_pBtnMute );
	pLayoutMgr->RegisterCtrl( this, "UnMute", m_pBtnUnMute );
	pLayoutMgr->RegisterCtrl (this, "Cut", m_pBtnCut);
	
	pLayoutMgr->CreateControlPane( this, "GameCtrl", "normal" );
	pLayoutMgr->CreateBmpPane( this,"GameCtrl","normal" );

	UpdateAllWnd();
	YL_Log("CGameCtrlWnd.txt", LOG_DEBUG, "CGameCtrlWnd::OnCreate", "===OUT");
	return 0;
}

void CGameCtrlWnd::OnClickedReplay()
{
	YL_Log("CGameCtrlWnd.txt", LOG_DEBUG, "CGameCtrlWnd::OnClickedReplay", "===IN");
	GetParent()->PostMessage( WM_REPLAY );
	//SetGameEntry( m_swfGame );
	YL_Log("CGameCtrlWnd.txt", LOG_DEBUG, "CGameCtrlWnd::OnClickedReplay", "===OUT");
}

void CGameCtrlWnd::OnClickedFullScreen()
{
	YL_Log("CGameCtrlWnd.txt", LOG_DEBUG, "CGameCtrlWnd::OnClickedFullScreen", "===IN");

	if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
	{
		GLOBAL_PANELCHANGEDATA->IPanelChange_ExitFullScreen();
	}else
	{
		GLOBAL_PANELCHANGEDATA->IPanelChange_ToFullScreen( GetParent() );
	}	
	YL_Log("CGameCtrlWnd.txt", LOG_DEBUG, "CGameCtrlWnd::OnClickedFullScreen", "===OUT");
}
//和上边的函数内容完全相同
void CGameCtrlWnd::OnClickedExitFullScreen()
{
	YL_Log("CGameCtrlWnd.txt", LOG_DEBUG, "CGameCtrlWnd::OnClickedFullScreen", "===IN");

	if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
	{
		GLOBAL_PANELCHANGEDATA->IPanelChange_ExitFullScreen();
	}else
	{
		GLOBAL_PANELCHANGEDATA->IPanelChange_ToFullScreen( GetParent() );
	}	
	YL_Log("CGameCtrlWnd.txt", LOG_DEBUG, "CGameCtrlWnd::OnClickedFullScreen", "===OUT");
}

void CGameCtrlWnd::UpdateAllWnd()
{
	YL_Log("CGameCtrlWnd.txt", LOG_DEBUG, "CGameCtrlWnd::UpdateAllWnd", "===IN");
	//显示重玩
	m_pBtnRePlay->ShowWindow( SW_SHOW );
	m_pBtnRePlay->Invalidate();
	//声音
	if( CSound::GetInstance()->GetMute( 1 ) )
	{
		m_pBtnMute->ShowWindow( SW_HIDE );
		m_pBtnUnMute->ShowWindow( SW_SHOW );
	}else
	{
		m_pBtnMute->ShowWindow( SW_SHOW );
		m_pBtnUnMute->ShowWindow( SW_HIDE );
	}
	//全屏
	if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
	{
		m_pBtnToFullScreen->ShowWindow( SW_HIDE );
		m_pBtnExitFullScreen->ShowWindow( SW_SHOW );
	}else
	{
		m_pBtnToFullScreen->ShowWindow( SW_SHOW );
		m_pBtnExitFullScreen->ShowWindow( SW_HIDE );
	}
	
	YL_Log("CGameCtrlWnd.txt", LOG_DEBUG, "CGameCtrlWnd::UpdateAllWnd", "===OUT");
}

void CGameCtrlWnd::OnClickedMute()
{
	YL_Log("CGameCtrlWnd.txt", LOG_DEBUG, "CGameCtrlWnd::OnClickedMute", "===IN");
	if( CSound::GetInstance()->GetMute( 1 ) )
	{
		CSound::GetInstance()->SetMute( 1, false );
	}else
	{
		CSound::GetInstance()->SetMute( 1, true );
	}
	UpdateAllWnd();
	YL_Log("CGameCtrlWnd.txt", LOG_DEBUG, "CGameCtrlWnd::OnClickedMute", "===OUT");
}

//和上边的函数内容完全相同
void CGameCtrlWnd::OnClickedUnMute()
{
	YL_Log("CGameCtrlWnd.txt", LOG_DEBUG, "CGameCtrlWnd::OnClickedMute", "===IN");
	if( CSound::GetInstance()->GetMute( 1 ) )
	{
		CSound::GetInstance()->SetMute( 1, false );
	}else
	{
		CSound::GetInstance()->SetMute( 1, true );
	}
	UpdateAllWnd();
	YL_Log("CGameCtrlWnd.txt", LOG_DEBUG, "CGameCtrlWnd::OnClickedMute", "===OUT");
}

void CGameCtrlWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	__super::OnShowWindow(bShow, nStatus);
	UpdateAllWnd();
}

void CGameCtrlWnd::OnClickedCut()
{
	GetParent()->PostMessage( WM_CUT_SCREEN );
}