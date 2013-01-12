#include "stdafx.h"
#include "resource.h"
#include "PlayedGameWnd.h"
#include "..\UserMan\UserLogedInPanelWnd.h"
#include "..\UserMan\UserLogedOutPanelWnd.h"
#include "xSkinButton.h"
#include "PlayedGameListPanelWnd.h"
#include "..\WebInteract\MyWebBrowserWnd.h"
#include ".\playedgamewnd.h"
#include "/PlayBox_/EncConf/EncConf/common/YL_StringUtil.h"
#include "/PlayBox_/EncConf/EncConf/common/tools.h"
#include "AfxGlobals.h"
#include "AppConfig\config\ConfigSettingDef.h"

IMPLEMENT_DYNAMIC(PlayedGameWnd, CBasicWnd)
PlayedGameWnd::PlayedGameWnd()
: m_bSortTimeByAsc(true)
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
	ON_WM_SHOWWINDOW()
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

	GLOBAL_LOCALGAME->ILocalGameData_GetAllGame(m_gameList);
	m_pWndGameListWnd->ReSetGameList(m_gameList);

	std::string strPlayedGameRightUrl;
	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_CONFIG_PLAYED_GAME_RIGHT_URL,strPlayedGameRightUrl);
	if (!strPlayedGameRightUrl.empty())
	{
		m_pWndRecommand->Navigate(strPlayedGameRightUrl);
	} else {
		m_pWndRecommand->Navigate("about:blank");
	}

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
	LocalGameList lgl;
	copy(m_gameList.begin(), m_gameList.end(), back_inserter(lgl));
	m_gameList.clear();
	LocalGameList::iterator it = lgl.begin();
	for (; it!=lgl.end(); it++)
	{
		time_t timeAdd = str2time(it->strAddTime);

		LocalGameList::iterator beginPos = m_gameList.begin();
		for (int i=0; i<m_gameList.size(); i++)
		{
			time_t t = str2time(m_gameList[i].strAddTime);
			if ( (timeAdd<t) == m_bSortTimeByAsc ) {
				break;
			}
			
		}
		m_gameList.insert(beginPos+i, *it);
	}
	lgl.clear();
	m_bSortTimeByAsc = !m_bSortTimeByAsc;  // 默认是升序排序 假如多次点击排序的时候需要反序 则取消注释
	m_pWndGameListWnd->ReSetGameList(m_gameList);
}

void PlayedGameWnd::OnClickedToWebGame()
{
	GLOBAL_LOCALGAME->ILocalGameData_GetWebGame(m_gameList);
	// 是否登录
	// ...
	m_pWndGameListWnd->ReSetGameList(m_gameList);
}

void PlayedGameWnd::OnClickedToFlashGame()
{
	GLOBAL_LOCALGAME->ILocalGameData_GetFlashGame(m_gameList);
	// 是否登录
	// ...
	m_pWndGameListWnd->ReSetGameList(m_gameList);
}

void PlayedGameWnd::OnClickedToDownloadManager()
{

}

void PlayedGameWnd::OnClickedToCollectedGame()
{
	// 是否登录
	// ...
}

void PlayedGameWnd::OnClickedToDelete()
{	// 删除按钮~ 现在是显示全部游戏
	GLOBAL_LOCALGAME->ILocalGameData_GetAllGame(m_gameList);
	m_pWndGameListWnd->ReSetGameList(m_gameList);
}

time_t PlayedGameWnd::str2time(const string & strTime)
{
	struct tm tmLocal;

	vector<string> date;
	vector<string> time;
	YL_StringUtil::Tokenize(strTime, date, " ");
	if (date.size() != 2) { // get 2 strings like "2013-1-13" & "12:50"
		return 0;
	}
	YL_StringUtil::Tokenize(date[1], time, ":");
	if (time.size() != 2) {
		return 0;
	}
	// set time "12:50"
	tmLocal.tm_hour = str2int(time[0]);
	tmLocal.tm_min = str2int(time[1]);

	time[0] = date[0];
	date.clear();
	YL_StringUtil::Tokenize(time[0], date, "-");
	if (date.size() != 3) {
		return 0;
	}
	// set date "2013-1-13"
	tmLocal.tm_year = str2int(date[0])-1900;
	tmLocal.tm_mon = str2int(date[1]) - 1;
	tmLocal.tm_mday = str2int(date[2]);

	return mktime(&tmLocal);
}
