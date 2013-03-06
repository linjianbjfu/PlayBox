#include "stdafx.h"
#include "GameCenterWelcomePanelWnd.h"
#include "resource.h"
#include "..\..\Gui\CommonControl\xSkinButton.h"
#include "..\..\Core\CDataManager.h"

IMPLEMENT_DYNAMIC(CGameCenterWelcomeWnd, CBasicWnd)

CGameCenterWelcomeWnd::CGameCenterWelcomeWnd()
{
	m_pBtnGoToGameCenter = new CxSkinButton();
}

CGameCenterWelcomeWnd::~CGameCenterWelcomeWnd() 
{
	delete m_pBtnGoToGameCenter;
}

BEGIN_MESSAGE_MAP(CGameCenterWelcomeWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_BN_CLICKED(ID_BTN_GO_TO_GAME_CENTER,OnClickedGotoGameCenter)
END_MESSAGE_MAP()

int CGameCenterWelcomeWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectNULL(0,0,0,0);
	m_pBtnGoToGameCenter->Create(NULL,NULL,rectNULL,this,ID_BTN_GO_TO_GAME_CENTER);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->RegisterCtrl(this,"PlayedGameGoToGameCenter",m_pBtnGoToGameCenter);
	pLayoutMgr->CreateControlPane( this,"playedgamewelcomepanel","normal");
	pLayoutMgr->CreateBmpPane( this,"playedgamewelcomepanel","normal" );
	return 0;
}

void CGameCenterWelcomeWnd::OnClickedGotoGameCenter()
{
	TAB_ITEM tiGameCenter;
	tiGameCenter.strTitle = TAB_GAMECENTER_TITLE;
	tiGameCenter.enumType  = TAB_HOME;
	GLOBAL_TABBARDATA->ITabBar_ChangeTab( tiGameCenter );
}