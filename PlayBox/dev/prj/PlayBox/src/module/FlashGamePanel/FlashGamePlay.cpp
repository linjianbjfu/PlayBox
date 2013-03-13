#include "stdafx.h"
#include "resource.h"
#include "FlashGamePlay.h"
#include "..\..\Gui\CommonControl\shockwaveflash.h"
#include "..\..\Gui\CommonControl\xSkinButton.h"

IMPLEMENT_DYNAMIC(CFlashGamePlay, CBasicWnd)

CFlashGamePlay::CFlashGamePlay()
{
	m_pShockwave = new CShockwaveFlash();
	m_pBtnIntroCtrl = new CxSkinButton();
	m_pBtnRecommandCtrl = new CxSkinButton();
}

CFlashGamePlay::~CFlashGamePlay()
{
	delete m_pShockwave;
	delete m_pBtnIntroCtrl;
	delete m_pBtnRecommandCtrl;
}

BEGIN_MESSAGE_MAP(CFlashGamePlay, CBasicWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CFlashGamePlay::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectNull(0,0,0,0);
	m_pShockwave->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_FLASH_GAME_SHOCK_WAVE);
	m_pBtnIntroCtrl->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_FLASH_GAME_INTRO_CTRL);
	m_pBtnRecommandCtrl->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_FLASH_GAME_RECOMMAND_CTRL);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();	
	pLayoutMgr->RegisterCtrl( this, "FlashGameShockwave", m_pShockwave );
	pLayoutMgr->RegisterCtrl( this, "FlashGameBtnIntroCtrl", m_pBtnIntroCtrl );
	pLayoutMgr->RegisterCtrl( this, "FlashGameBtnRecommandCtrl", m_pBtnRecommandCtrl );

	pLayoutMgr->CreateControlPane( this, "flashgameplaypanel", "normal" );
	pLayoutMgr->CreateBmpPane( this,"flashgameplaypanel","normal" );

	return 0;
}
