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
	m_pBtnIntroCtrl = new CxSkinButton();
	m_pBtnRecommandCtrl = new CxSkinButton();
	m_pDownload = new CFlashGameDownloadWnd(pCtrl);
}

CFlashGamePlay::~CFlashGamePlay()
{
	delete m_pShockwave;
	delete m_pBtnIntroCtrl;
	delete m_pBtnRecommandCtrl;
	delete m_pDownload;
}

BEGIN_MESSAGE_MAP(CFlashGamePlay, CBasicWnd)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_FLASH_GAME_INTRO_CTRL, OnIntroClicked)
	ON_BN_CLICKED(IDC_BTN_FLASH_GAME_RECOMMAND_CTRL, OnRecommandClicked)
END_MESSAGE_MAP()

int CFlashGamePlay::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectNull(0,0,0,0);
	m_pShockwave->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_FLASH_GAME_SHOCK_WAVE);
	m_pBtnIntroCtrl->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_FLASH_GAME_INTRO_CTRL);
	m_pBtnRecommandCtrl->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_FLASH_GAME_RECOMMAND_CTRL);
	m_pDownload->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_FLASH_GAME_DOWNLOAD);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();	
	pLayoutMgr->RegisterCtrl( this, "FlashGameShockwave", m_pShockwave );
	pLayoutMgr->RegisterCtrl( this, "FlashGameBtnIntroCtrl", m_pBtnIntroCtrl );
	pLayoutMgr->RegisterCtrl( this, "FlashGameBtnRecommandCtrl", m_pBtnRecommandCtrl );
	pLayoutMgr->CreateControlPane( this, "flashgameplaypanel", "normal" );
	pLayoutMgr->CreateBmpPane( this,"flashgameplaypanel","normal" );
	InitFlashParams();
	return 0;
}

void CFlashGamePlay::InitFlashParams()
{
	m_pShockwave->SetBackgroundColor( -1 );//无背景色
	m_pShockwave->SetAlignMode( 2 );       //
	m_pShockwave->SetMenu( FALSE );        //无扩展的弹出菜单 
	m_pShockwave->SetScale( "ExactFit" );  //缩放模式为能显示动画所有内容
	m_pShockwave->SetWMode( "Window" );    //设置窗口模式
	m_pShockwave->SetQuality( 1 );         //设置播放动画质量为高 
	m_pShockwave->SetEmbedMovie(TRUE);     //动画是内嵌的 
}

void CFlashGamePlay::ShowButtons(bool bShow)
{
	m_pBtnIntroCtrl->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	m_pBtnRecommandCtrl->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
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

void CFlashGamePlay::OnIntroClicked()
{
	m_pCtrl->ShowIntro(!m_pCtrl->IsIntroShow());
}

void CFlashGamePlay::OnRecommandClicked()
{
	m_pCtrl->ShowRecommand(!m_pCtrl->IsRecommandShow());
}

void CFlashGamePlay::ILayoutChangeOb_UpdateLayout(HWND hWnd)
{
	if (hWnd == GetSafeHwnd())
	{
	}
}