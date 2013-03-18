#include "stdafx.h"
#include "resource.h"
#include "FlashGameStagePanelWnd.h"
#include "FlashGameStageLeft.h"
#include "..\WebInteract\MyWebBrowserWnd.h"
#include "OneFlashGameControl.h"

IMPLEMENT_DYNAMIC(CFlashGameStagePanelWnd, CBasicWnd)

CFlashGameStagePanelWnd::CFlashGameStagePanelWnd(COneFlashGameControl* pCtrl)
{
	m_pCtrl = pCtrl;
	m_pCtrl->SetStage(this);
	m_pStageLeft = new CFlashGameStageLeft(pCtrl);
	m_pIntro = new MyWebBrowserWnd();
	m_pCtrl->SetBrowserIntro(m_pIntro);
}

CFlashGameStagePanelWnd::~CFlashGameStagePanelWnd()
{
	delete m_pStageLeft;
	//delete m_pIntro;
}

BEGIN_MESSAGE_MAP(CFlashGameStagePanelWnd, CBasicWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CFlashGameStagePanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectNull(0,0,0,0);
	m_pStageLeft->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_FLASH_GAME_STAGE_LEFT_PANEL);
	m_pIntro->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_FLASH_GAME_INTRO);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();	
	pLayoutMgr->RegisterCtrl( this, "flashgamestageleft", m_pStageLeft );
	pLayoutMgr->RegisterCtrl( this, "flashgameintro", m_pIntro );

	pLayoutMgr->CreateControlPane( this, "flashgamestagepanel", "normal" );
	pLayoutMgr->CreateBmpPane( this,"flashgamestagepanel","normal" );

	m_pIntro->Navigate("http://www.baidu.com");

	return 0;
}
