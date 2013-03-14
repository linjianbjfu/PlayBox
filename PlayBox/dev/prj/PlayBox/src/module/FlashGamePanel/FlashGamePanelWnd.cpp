#include "stdafx.h"
#include "resource.h"
#include "FlashGamePanelWnd.h"
#include "FlashGameCtrlPanel.h"
#include "FlashGameStagePanelWnd.h"

IMPLEMENT_DYNAMIC(CFlashGamePanelWnd, CBasicWnd)

CFlashGamePanelWnd::CFlashGamePanelWnd()
{
	m_pGameCtrlPanelWnd = new CFlashGameCtrlPanelWnd();
	m_pGameStagePanelWnd = new CFlashGameStagePanelWnd();
}

CFlashGamePanelWnd::~CFlashGamePanelWnd()
{
	delete m_pGameCtrlPanelWnd;
	delete m_pGameStagePanelWnd;
}

BEGIN_MESSAGE_MAP(CFlashGamePanelWnd, CBasicWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CFlashGamePanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectNull(0,0,0,0);
	m_pGameCtrlPanelWnd->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_FLASH_GAME_CTRL_PANEL);
	m_pGameStagePanelWnd->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_FLASH_GAME_STAGE_PANEL);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();	
	pLayoutMgr->RegisterCtrl( this, "flashgamectrl", m_pGameCtrlPanelWnd );
	pLayoutMgr->RegisterCtrl( this, "flashgamestage", m_pGameStagePanelWnd );

	pLayoutMgr->CreateControlPane( this, "flashgamepanel", "normal" );
	pLayoutMgr->CreateBmpPane( this,"flashgamepanel","normal" );

	return 0;
}

void CFlashGamePanelWnd::SetTabItem( TAB_ITEM ti )
{

}