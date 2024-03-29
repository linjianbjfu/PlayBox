#include "stdafx.h"
#include "resource.h"
#include "FlashGameStageLeft.h"
#include "FlashGamePlay.h"
#include "FlashDownloadWnd.h"
#include "..\WebInteract\MyWebBrowserWnd.h"
#include "OneFlashGameControl.h"

IMPLEMENT_DYNAMIC(CFlashGameStageLeft, CBasicWnd)

CFlashGameStageLeft::CFlashGameStageLeft(COneFlashGameControl* pCtrl)
{
	m_pCtrl = pCtrl;
	m_pCtrl->SetStageLeft(this);
	m_pPlay = new CFlashGamePlay(pCtrl);
	m_pRecommand = new MyWebBrowserWnd();
	m_pCtrl->SetBrowserRecommand(m_pRecommand);
}

CFlashGameStageLeft::~CFlashGameStageLeft()
{
	delete m_pPlay;
}

BEGIN_MESSAGE_MAP(CFlashGameStageLeft, CBasicWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CFlashGameStageLeft::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectNull(0,0,0,0);
	m_pPlay->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_FLASH_GAME_PLAY);
	m_pRecommand->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_FLASH_GAME_RECOMMAND);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();	
	pLayoutMgr->RegisterCtrl( this, "flashgameplay", m_pPlay );
	pLayoutMgr->RegisterCtrl( this, "flashgamerecommand", m_pRecommand );
	pLayoutMgr->CreateControlPane( this, "flashgamestageleftpanel", "normal" );
	pLayoutMgr->CreateBmpPane( this,"flashgamestageleftpanel","normal" );
	return 0;
}