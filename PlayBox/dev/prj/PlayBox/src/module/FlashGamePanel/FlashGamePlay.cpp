#include "stdafx.h"
#include "resource.h"
#include "FlashGamePlay.h"
#include "..\..\Gui\CommonControl\shockwaveflash.h"
#include "..\..\Gui\CommonControl\xSkinButton.h"
#include "OneFlashGameControl.h"
#include "FlashDownloadWnd.h"

IMPLEMENT_DYNAMIC(CFlashGamePlay, CBasicWnd)

CFlashGamePlay::CFlashGamePlay(COneFlashGameControl* pCtrl)
{
	m_pCtrl = pCtrl;
	m_pCtrl->SetFlashPlay(this);
	m_pShockwave = new CShockwaveFlash();
	m_pCtrl->SetFlashCore(m_pShockwave);
	m_pBtnIntroCtrlShow = new CxSkinButton();
	m_pBtnIntroCtrlHide = new CxSkinButton();
	m_pDownload = new CFlashGameDownloadWnd(pCtrl);
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_LAYOUTMGR,(ILayoutChangeObserver*) this);
}

CFlashGamePlay::~CFlashGamePlay()
{
	delete m_pShockwave;
	delete m_pBtnIntroCtrlShow;
	delete m_pBtnIntroCtrlHide;
	delete m_pDownload;
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_LAYOUTMGR,(ILayoutChangeObserver*) this);
}

BEGIN_MESSAGE_MAP(CFlashGamePlay, CBasicWnd)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_FLASH_GAME_INTRO_CTRL_SHOW, OnIntroShowClicked)
	ON_BN_CLICKED(IDC_BTN_FLASH_GAME_INTRO_CTRL_HIDE, OnIntroHideClicked)
END_MESSAGE_MAP()

int CFlashGamePlay::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectNull(0,0,0,0);
	m_pShockwave->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		rectNull,this,ID_FLASH_GAME_SHOCK_WAVE);
	m_pBtnIntroCtrlShow->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_FLASH_GAME_INTRO_CTRL_SHOW);
	m_pBtnIntroCtrlHide->Create(NULL,WS_CHILD,rectNull,this,IDC_BTN_FLASH_GAME_INTRO_CTRL_HIDE);
	m_pDownload->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		rectNull,this,ID_FLASH_GAME_DOWNLOAD);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();	
	pLayoutMgr->RegisterCtrl( this, "FlashGameShockwave", m_pShockwave );
	pLayoutMgr->RegisterCtrl( this, "FlashGameDownload", m_pDownload );
	pLayoutMgr->RegisterCtrl( this, "FlashGameBtnIntroShow", m_pBtnIntroCtrlShow );
	pLayoutMgr->RegisterCtrl( this, "FlashGameBtnIntroHide", m_pBtnIntroCtrlHide );
	pLayoutMgr->CreateControlPane( this, "flashgameplaypanel", "normal" );
	pLayoutMgr->CreateBmpPane( this,"flashgameplaypanel","normal" );
	InitFlashParams();

	return 0;
}

void CFlashGamePlay::InitFlashParams()
{
	//参考http://www.nowamagic.net/librarys/veda/detail/1615
	m_pShockwave->SetBackgroundColor( 0 );
	//m_pShockwave->SetAlignMode( 2 );       //
	//m_pShockwave->SetMenu( FALSE );        //无扩展的弹出菜单 
	//m_pShockwave->SetScale( "ExactFit" );  //缩放模式为能显示动画所有内容
	//m_pShockwave->SetWMode( "opaque" );    //设置窗口模式
	//m_pShockwave->SetQuality2( "high" );   //设置播放动画质量为高 
	//m_pShockwave->SetEmbedMovie(TRUE);     //动画是内嵌的 
}

void CFlashGamePlay::ShowButtons()
{
	if (m_pCtrl->IsFullScreen())
	{
		m_pBtnIntroCtrlShow->ShowWindow(SW_HIDE);
		m_pBtnIntroCtrlHide->ShowWindow(SW_HIDE);
	} else 
	{
		m_pBtnIntroCtrlShow->ShowWindow(m_pCtrl->IsIntroShow() ? SW_HIDE : SW_SHOW);
		m_pBtnIntroCtrlHide->ShowWindow(m_pCtrl->IsIntroShow() ? SW_SHOW : SW_HIDE);
	}
	CRect r;
	m_pBtnIntroCtrlShow->GetWindowRect(&r);
	ScreenToClient(&r);
	InvalidateRect(&r);
	if(m_pShockwave->IsWindowVisible())
	{
		if (m_pCtrl->IsDownloading())
			m_pShockwave->ShowWindow(SW_HIDE);
	} else
	{
		if (!m_pCtrl->IsDownloading())
			m_pShockwave->ShowWindow(SW_SHOW);
	}
	
	if(m_pDownload->IsWindowVisible())
	{
		if (!m_pCtrl->IsDownloading())
			m_pDownload->ShowWindow(SW_HIDE);
	} else
	{
		if (m_pCtrl->IsDownloading())
			m_pDownload->ShowWindow(SW_SHOW);
	}
}

BOOL CFlashGamePlay::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		m_pCtrl->ExitFullScreen();
		return TRUE;
	}
	return __super::PreTranslateMessage(pMsg);
}

void CFlashGamePlay::OnIntroShowClicked()
{
	m_pCtrl->ShowIntro(!m_pCtrl->IsIntroShow());
}

void CFlashGamePlay::OnIntroHideClicked()
{
	m_pCtrl->ShowIntro(!m_pCtrl->IsIntroShow());
}

void CFlashGamePlay::ILayoutChangeOb_UpdateLayout(HWND hWnd)
{
	if (hWnd == GetSafeHwnd())
		ShowButtons();
}