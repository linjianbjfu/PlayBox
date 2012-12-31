#include "stdafx.h"
#include "resource.h"
#include "PlayedGameWnd.h"
#include "..\UserMan\UserLogedInPanelWnd.h"
#include "..\UserMan\UserLogedOutPanelWnd.h"
#include "xSkinButton.h"
#include "PlayedGameListPanelWnd.h"
#include "..\WebInteract\MyWebBrowserWnd.h"

IMPLEMENT_DYNAMIC(PlayedGameWnd, CBasicWnd)
PlayedGameWnd::PlayedGameWnd()
{
	m_pBtnTimeOrder = new CxSkinButton();
	m_pBtnToWebGame = new CxSkinButton();
	m_pBtnToFlashGame = new CxSkinButton();
	m_pBtnToDownloadManager = new CxSkinButton();
	m_pBtnToCollectedGame = new CxSkinButton();
	m_pBtnToDelete = new CxSkinButton();
	m_pWndGameListWnd = new PlayedGameListPanelWnd();
	m_pWndRecommand = new MyWebBrowserWnd();
	m_pWndLogedIn	= new CUserLogedInWnd();
	m_pWndLogedOut	= new CUserLogedOutWnd();
}

PlayedGameWnd::~PlayedGameWnd()
{
	delete m_pBtnTimeOrder;
	delete m_pBtnToWebGame;
	delete m_pBtnToFlashGame;
	delete m_pBtnToDownloadManager;
	delete m_pBtnToCollectedGame;
	delete m_pBtnToDelete;
	delete m_pWndGameListWnd;
	delete m_pWndLogedIn;
	delete m_pWndLogedOut;
	//do not delete m_pWndRecommand
}

BEGIN_MESSAGE_MAP(PlayedGameWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_BN_CLICKED(ID_BTN_PLAYED_GAME_TIME_ORDER,OnClickedTimerOrder)
	ON_BN_CLICKED(ID_BTN_PLAYED_GAME_TO_WEB_GAME,OnClickedToWebGame)
	ON_BN_CLICKED(ID_BTN_PLAYED_GAME_TO_FLASH_GAME,OnClickedToFlashGame)
	ON_BN_CLICKED(ID_BTN_PLAYED_GAME_TO_DOWNLOAD_MANAGER,OnClickedToDownloadManager)
	ON_BN_CLICKED(ID_BTN_PLAYED_GAME_TO_COLLECTED_GAME,OnClickedToCollectedGame)
	ON_BN_CLICKED(ID_BTN_PLAYED_GAME_TO_DELETE,OnClickedToDelete)
END_MESSAGE_MAP()

int PlayedGameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectNULL(0,0,0,0);
	m_pBtnTimeOrder->Create(NULL,NULL,rectNULL,this,ID_BTN_PLAYED_GAME_TIME_ORDER);
	m_pBtnToWebGame->Create(NULL,NULL,rectNULL,this,ID_BTN_PLAYED_GAME_TO_WEB_GAME);
	m_pBtnToFlashGame->Create(NULL,NULL,rectNULL,this,ID_BTN_PLAYED_GAME_TO_FLASH_GAME);
	m_pBtnToDownloadManager->Create(NULL,NULL,rectNULL,this,ID_BTN_PLAYED_GAME_TO_DOWNLOAD_MANAGER);
	m_pBtnToCollectedGame->Create(NULL,NULL,rectNULL,this,ID_BTN_PLAYED_GAME_TO_COLLECTED_GAME);
	m_pBtnToDelete->Create(NULL,NULL,rectNULL,this,ID_BTN_PLAYED_GAME_TO_DELETE);
	m_pWndGameListWnd->Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,rectNULL,this,ID_WND_PLAYED_GAME_LIST);
	m_pWndRecommand->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNULL,this,ID_WND_PLAYED_GAME_RECOMMAND);
	m_pWndLogedIn->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNULL,this,ID_WND_USER_LOGED_IN);
	m_pWndLogedOut->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNULL,this,ID_WND_USER_LOGED_OUT);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->RegisterCtrl(this,"PlayedGameTimeOrder",m_pBtnTimeOrder);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameToWebGame",m_pBtnToWebGame);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameToFlashGame",m_pBtnToFlashGame);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameToDownloadManager",m_pBtnToDownloadManager);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameToCollectedGame",m_pBtnToCollectedGame);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameDelete",m_pBtnToDelete);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameGameListPanel",m_pWndGameListWnd);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameRecommandWebPanel",m_pWndRecommand);
	pLayoutMgr->RegisterCtrl(this,"UserMan_LogedIn",m_pWndLogedIn);
	pLayoutMgr->RegisterCtrl(this,"UserMan_LogedOut",m_pWndLogedOut);

	pLayoutMgr->CreateControlPane( this,"playedgamepanel","normal");
	pLayoutMgr->CreateBmpPane( this,"playedgamepanel","normal" );
	return 0;
}

void PlayedGameWnd::OnDestroy()
{
	m_pWndGameListWnd->DestroyWindow();
	m_pWndLogedIn->DestroyWindow();
	m_pWndLogedOut->DestroyWindow();
	m_pWndRecommand->DestroyWindow();
	__super::OnDestroy();
}

void PlayedGameWnd::OnClickedTimerOrder()
{

}

void PlayedGameWnd::OnClickedToWebGame()
{

}

void PlayedGameWnd::OnClickedToFlashGame()
{

}

void PlayedGameWnd::OnClickedToDownloadManager()
{

}

void PlayedGameWnd::OnClickedToCollectedGame()
{

}

void PlayedGameWnd::OnClickedToDelete()
{

}