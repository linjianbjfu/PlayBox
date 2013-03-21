#include "stdafx.h"
#include "resource.h"
#include "FlashGameIntro.h"
#include "..\..\Gui\CommonControl\xSkinButton.h"
#include "OneFlashGameControl.h"
#include "..\WebInteract\MyWebBrowserWnd.h"

IMPLEMENT_DYNAMIC(CFlashGameIntro, CBasicWnd)

CFlashGameIntro::CFlashGameIntro(COneFlashGameControl* pCtrl)
{
	m_pCtrl = pCtrl;
	m_pBtnIntroCtrlHide = new CxSkinButton();
	m_pWebIntro = new MyWebBrowserWnd();
}

CFlashGameIntro::~CFlashGameIntro()
{
	delete m_pBtnIntroCtrlHide;
}

void CFlashGameIntro::Navigate(const std::string& strUrl)
{
	m_pWebIntro->Navigate(strUrl);
}

BEGIN_MESSAGE_MAP(CFlashGameIntro, CBasicWnd)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_FLASH_GAME_INTRO_CTRL_HIDE, OnIntroHideClicked)
END_MESSAGE_MAP()

int CFlashGameIntro::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectNull(0,0,0,0);
	m_pBtnIntroCtrlHide->Create(NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
		rectNull,this, IDC_BTN_FLASH_GAME_INTRO_CTRL_HIDE);
	m_pWebIntro->Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
		rectNull, this, ID_FLASH_GAME_INTRO_WEB);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();	
	pLayoutMgr->RegisterCtrl( this, "FlashGameBtnIntroHide", m_pBtnIntroCtrlHide );
	pLayoutMgr->RegisterCtrl( this, "FlashGameIntroWeb", m_pWebIntro );
	pLayoutMgr->CreateControlPane( this, "flashgameintropanel", "normal" );
	pLayoutMgr->CreateBmpPane( this,"flashgameintropanel","normal" );

	return 0;
}

void CFlashGameIntro::OnIntroHideClicked()
{
	m_pCtrl->ShowIntro(!m_pCtrl->IsIntroShow());
}