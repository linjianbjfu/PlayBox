#include "stdafx.h"
#include "resource.h"
#include "GameCenterPanelWnd.h"
#include "../WebInteract/MyWebBrowserWnd.h"
#include "../WebInteract/WebManager.h"
#include "../../core/CDataManager.h"
#include "../../gui/CommonControl/xSkinButton.h"
#include "../../gui/CommonControl/xStaticText.h"
#include "../../Gui/CommonControl/EditEx.h"
#include "../../AppConfig/config/ConfigSettingDef.h"

IMPLEMENT_DYNAMIC(GameCenterPanelWnd, CBasicWnd)
GameCenterPanelWnd::GameCenterPanelWnd() : m_curPageType(PT_MAIN_PAGE)
{
	m_pBtnMainPage = new CxSkinButton();
	m_pBtnFlashGame = new CxSkinButton();
	m_pBtnWebGame = new CxSkinButton();
	m_pBtnSearchBtn = new CxSkinButton();
	m_pEditSearch = new CEditEx();
	m_pWndBrowser = new MyWebBrowserWnd();
}

GameCenterPanelWnd::~GameCenterPanelWnd()
{
	delete m_pBtnMainPage;
	delete m_pBtnFlashGame;
	delete m_pBtnWebGame;
	delete m_pBtnSearchBtn;
	delete m_pEditSearch;
	//do not delete m_pWndBrowser
	//delete m_pWndBrowser;
}

BEGIN_MESSAGE_MAP(GameCenterPanelWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_GAMECENTER_MAIN_PAGE,OnClickedMainPage)
	ON_BN_CLICKED(IDC_BTN_GAMECENTER_FLASHGAME,OnClickedFlashGame)
	ON_BN_CLICKED(IDC_BTN_GAMECENTER_WEBGAME,OnClickedWebGame)
	ON_BN_CLICKED(IDC_BTN_GAMECENTER_SEARCH,OnClickedSearchBtn)
END_MESSAGE_MAP()

int GameCenterPanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

void GameCenterPanelWnd::OnDestroy()
{
	m_pWndBrowser->DestroyWindow();
	__super::OnDestroy();
}

void GameCenterPanelWnd::OnClickedMainPage()
{
	GoToURL(PT_MAIN_PAGE);
}

void GameCenterPanelWnd::OnClickedFlashGame()
{
	GoToURL(PT_FLASH_GAME);
}

void GameCenterPanelWnd::OnClickedWebGame()
{
	GoToURL(PT_WEB_GAME);
}

void GameCenterPanelWnd::OnClickedSearchBtn()
{
	GoToURL(PT_SEARCH);
}

void GameCenterPanelWnd::Navigate()
{
	AfxGetUserConfig()->GetConfigStringValue( CONF_SETTING_MODULE_NAME,CONF_SETTING_CONFIG_GAME_CENTER_MAIN_PAGE_URL,m_strUrlMainPage);
	AfxGetUserConfig()->GetConfigStringValue( CONF_SETTING_MODULE_NAME,CONF_SETTING_CONFIG_GAME_CENTER_FLASH_GAME_URL,m_strUrlFlashGame);
	AfxGetUserConfig()->GetConfigStringValue( CONF_SETTING_MODULE_NAME,CONF_SETTING_CONFIG_GAME_CENTER_WEB_GAME_URL,m_strUrlWebGame);
	AfxGetUserConfig()->GetConfigStringValue( CONF_SETTING_MODULE_NAME,CONF_SETTING_CONFIG_GAME_CENTER_SEARCH_URL,m_strUrlSearch);

	char szBuffer[MAX_PATH];
	CLhcImg::GetHomePath(szBuffer, MAX_PATH-1);
	m_strUrlMainPage = string(szBuffer)+"\\Resources\\StandardUI\\testGame.html";
	m_strUrlFlashGame = "http://www.baidu.com/s?wd=flashgame";
	m_strUrlWebGame = "http://www.baidu.com/s?wd=flashgame";
	m_strUrlSearch = "http://www.baidu.com/s?wd=search";

	GoToURL(PT_MAIN_PAGE);
}

void GameCenterPanelWnd::GoToURL(PageType pt)
{
	m_curPageType = pt;
	string strUrl;
	switch (m_curPageType)
	{
	case PT_MAIN_PAGE:
		strUrl = m_strUrlMainPage;
		break;
	case PT_FLASH_GAME:
		strUrl = m_strUrlFlashGame;
		break;
	case PT_WEB_GAME:
		strUrl = m_strUrlWebGame;
		break;
	case PT_SEARCH:
		strUrl = m_strUrlSearch+"?keyword=ËÑË÷¹Ø¼ü´Ê";
		break;
	}
	m_pBtnMainPage->SetCheck(m_curPageType == PT_MAIN_PAGE);
	m_pBtnFlashGame->SetCheck(m_curPageType == PT_FLASH_GAME);
	m_pBtnWebGame->SetCheck(m_curPageType == PT_WEB_GAME);
	m_pWndBrowser->Navigate(strUrl);
}