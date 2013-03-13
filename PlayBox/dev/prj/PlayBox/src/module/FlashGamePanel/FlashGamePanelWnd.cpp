#include "stdafx.h"
#include "resource.h"
#include "FlashGamePanelWnd.h"

IMPLEMENT_DYNAMIC(CFlashGamePanelWnd, CBasicWnd)

CFlashGamePanelWnd::CFlashGamePanelWnd() : m_curPageType(PT_MAIN_PAGE)
{
	m_pGameCtrlPanelWnd = new CFlashGameCtrlPanelWnd();
	m_pGameCenterPanelWnd = new CFlashGameCenterPanelWnd();
}

CFlashGamePanelWnd::~CFlashGamePanelWnd()
{
	delete m_pGameCtrlPanelWnd;
	delete m_pGameCenterPanelWnd;
}

BEGIN_MESSAGE_MAP(CFlashGamePanelWnd, CBasicWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CFlashGamePanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectNull(0,0,0,0);
	m_pBtnMainPage->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_GAMECENTER_MAIN_PAGE);
	m_pBtnFlashGame->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_GAMECENTER_FLASHGAME);
	m_pBtnWebGame->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_GAMECENTER_WEBGAME);
	m_pBtnSearchBtn->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_GAMECENTER_SEARCH);
	m_pEditSearch->Create(WS_VISIBLE,rectNull,this,IDC_EDIT_GAMECENTER_SEARCH);
	m_pWndBrowser->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,IDC_WND_GAMECENTER_BROWSER );

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();	
	pLayoutMgr->RegisterCtrl( this, "GameCenterMainPage", m_pBtnMainPage );
	pLayoutMgr->RegisterCtrl( this, "GameCenterFlashGame", m_pBtnFlashGame );
	pLayoutMgr->RegisterCtrl( this, "GameCenterWebGame", m_pBtnWebGame );
	pLayoutMgr->RegisterCtrl( this, "GameCenterSearchBtn", m_pBtnSearchBtn );
	pLayoutMgr->RegisterCtrl( this, "GameCenterSearchEdit", m_pEditSearch );
	pLayoutMgr->RegisterCtrl( this, "GameCenterBrowserWnd", m_pWndBrowser );

	pLayoutMgr->CreateControlPane( this, "gamecenterpanel", "normal" );
	pLayoutMgr->CreateBmpPane( this,"gamecenterpanel","normal" );

	return 0;
}
