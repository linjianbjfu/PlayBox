#include "stdafx.h"
#include "resource.h"
#include "FlashGameStageLeft.h"
#include "FlashGamePlay.h"
#include "FlashDownloadWnd.h"
#include "..\WebInteract\MyWebBrowserWnd.h"

IMPLEMENT_DYNAMIC(CFlashGameStageLeft, CBasicWnd)

CFlashGameStageLeft::CFlashGameStageLeft()
{
	m_pPlay = new CFlashGamePlay();
	m_pDownload = new CFlashGameDownloadWnd();
	m_pRecommand = new MyWebBrowserWnd();
}

CFlashGameStageLeft::~CFlashGameStageLeft()
{
	delete m_pPlay;
	delete m_pDownload;
	//delete m_pRecommand;
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
	m_pDownload->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_FLASH_GAME_DOWNLOAD);
	m_pRecommand->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_FLASH_GAME_RECOMMAND);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();	
	pLayoutMgr->RegisterCtrl( this, "flashgame_play", m_pPlay );
	pLayoutMgr->RegisterCtrl( this, "flashgame_download", m_pDownload );
	pLayoutMgr->RegisterCtrl( this, "flashgamerecommand", m_pRecommand );
	pLayoutMgr->CreateControlPane( this, "flashgamestageleftpanel", "normal" );
	pLayoutMgr->CreateBmpPane( this,"flashgamestageleftpanel","normal" );

	m_pRecommand->Navigate("http://www.sogou.com");

	return 0;
}
