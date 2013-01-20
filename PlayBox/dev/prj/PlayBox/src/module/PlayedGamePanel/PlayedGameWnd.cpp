#include "stdafx.h"
#include "resource.h"
#include "PlayedGameWnd.h"
#include "..\UserMan\UserLogedInPanelWnd.h"
#include "..\UserMan\UserLogedOutPanelWnd.h"
#include "xSkinButton.h"
#include "PlayedGameListPanelWnd.h"
#include "..\WebInteract\MyWebBrowserWnd.h"
#include "YL_StringUtil.h"
#include "tools.h"
#include "AfxGlobals.h"
#include "AppConfig\config\ConfigSettingDef.h"
#include "..\UserMan\UserManager.h"

static const char* s_USER_MAN_LOGED_IN_OUT	= "UserMan_LogedIn_LogedOut";
static const char* s_USER_MAN_LOGED_IN	= "UserMan_LogedIn";
static const char* s_USER_MAN_LOGED_OUT	= "UserMan_LogedOut";

IMPLEMENT_DYNAMIC(PlayedGameWnd, CBasicWnd)

#define NEW_SKIN_BTN(pBtn) pBtn = new CxSkinButton();
PlayedGameWnd::PlayedGameWnd()
: m_bSortTimeByAsc(true), m_iGameType(0)
{
	NEW_SKIN_BTN(m_pBtnTimeOrder);
	NEW_SKIN_BTN(m_pBtnToWebGame);
	NEW_SKIN_BTN(m_pBtnToFlashGame);
	NEW_SKIN_BTN(m_pBtnToCollectedGame);
	m_pWndGameListWnd = new PlayedGameListPanelWnd();
	m_pWndRecommand = new MyWebBrowserWnd();
	m_pWndLogedIn	= new CUserLogedInWnd();
	m_pWndLogedOut	= new CUserLogedOutWnd();
	AfxGetMessageManager()->AttachMessage(ID_MESSAGE_USER, this);
}

PlayedGameWnd::~PlayedGameWnd()
{
	delete m_pBtnTimeOrder;
	delete m_pBtnToWebGame;
	delete m_pBtnToFlashGame;
	delete m_pBtnToCollectedGame;
	delete m_pWndGameListWnd;
	delete m_pWndLogedIn;
	delete m_pWndLogedOut;
	//do not delete m_pWndRecommand
	AfxGetMessageManager()->DetachMessage(ID_MESSAGE_USER, this);
}

BEGIN_MESSAGE_MAP(PlayedGameWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_BN_CLICKED(ID_BTN_PLAYED_GAME_TIME_ORDER,OnClickedTimerOrder)
	ON_BN_CLICKED(ID_BTN_PLAYED_GAME_TO_WEB_GAME,OnClickedToWebGame)
	ON_BN_CLICKED(ID_BTN_PLAYED_GAME_TO_FLASH_GAME,OnClickedToFlashGame)
	ON_BN_CLICKED(ID_BTN_PLAYED_GAME_TO_COLLECTED_GAME,OnClickedToCollectedGame)
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
	m_pBtnToCollectedGame->Create(NULL,NULL,rectNULL,this,ID_BTN_PLAYED_GAME_TO_COLLECTED_GAME);
	m_pWndGameListWnd->Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,rectNULL,this,ID_WND_PLAYED_GAME_LIST);
	m_pWndRecommand->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNULL,this,ID_WND_PLAYED_GAME_RECOMMAND);
	m_pWndLogedIn->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNULL,this,ID_WND_USER_LOGED_IN);
	m_pWndLogedOut->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNULL,this,ID_WND_USER_LOGED_OUT);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->RegisterCtrl(this,"PlayedGameTimeOrder",m_pBtnTimeOrder);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameToWebGame",m_pBtnToWebGame);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameToFlashGame",m_pBtnToFlashGame);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameToCollectedGame",m_pBtnToCollectedGame);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameGameListPanel",m_pWndGameListWnd);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameRecommandWebPanel",m_pWndRecommand);
	pLayoutMgr->RegisterCtrl(this,"UserMan_LogedIn",m_pWndLogedIn);
	pLayoutMgr->RegisterCtrl(this,"UserMan_LogedOut",m_pWndLogedOut);

	pLayoutMgr->CreateControlPane( this,"playedgamepanel","normal");
	pLayoutMgr->CreateBmpPane( this,"playedgamepanel","normal" );

	AfxGetUIManager()->UIGetLayoutMgr()->ShowLayer(s_USER_MAN_LOGED_IN_OUT, s_USER_MAN_LOGED_OUT);

	RefreshData();
	std::string strPlayedGameRightUrl;
	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_CONFIG_PLAYED_GAME_RIGHT_URL,strPlayedGameRightUrl);

	m_pWndRecommand->Navigate(strPlayedGameRightUrl.empty() ? 
		"about:blank" : strPlayedGameRightUrl);
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
	GameList lgl;
	copy(m_gameList.begin(), m_gameList.end(), back_inserter(lgl));
	m_gameList.clear();
	GameList::iterator it = lgl.begin();
	for (; it!=lgl.end(); it++)
	{
		time_t timeAdd = str2time(it->strAddTime);
		GameList::iterator beginPos = m_gameList.begin();
		for (int i=0; i<m_gameList.size(); i++)
		{
			time_t t = str2time(m_gameList[i].strAddTime);
			if ((timeAdd<t) == m_bSortTimeByAsc)
				break;
		}
		m_gameList.insert(beginPos+i, *it);
	}
	lgl.clear();
	m_bSortTimeByAsc = !m_bSortTimeByAsc;  // 默认是升序排序 假如多次点击排序的时候需要反序 则取消注释
	m_pWndGameListWnd->ReSetGameList(m_gameList);
}

void PlayedGameWnd::RefreshData()
{
	//根据按钮状态，确定用户想看的game的类型
	int iGameType = 0;
	if (m_pBtnToFlashGame->GetCheck())
		iGameType |= OneGame::FLASH_GAME;
	else
		iGameType &= ~OneGame::FLASH_GAME;

	if (m_pBtnToWebGame->GetCheck())
		iGameType |= OneGame::WEB_GAME;
	else
		iGameType &= ~OneGame::WEB_GAME;

	if (m_pBtnToFlashGame->GetCheck() && m_pBtnToWebGame->GetCheck())
	{
		iGameType &= ~OneGame::FLASH_GAME;
		iGameType &= ~OneGame::WEB_GAME;
	}

	if (m_pBtnToCollectedGame->GetCheck())
		iGameType |= OneGame::COLLECTED;
	else
		iGameType &= ~OneGame::COLLECTED;
	
	GLOBAL_GAME->IGameData_GetGame(m_gameList, iGameType);
	m_pWndGameListWnd->ReSetGameList(m_gameList);
}

void PlayedGameWnd::OnClickedToWebGame()
{
	m_pBtnToWebGame->SetCheck(!m_pBtnToWebGame->GetCheck());
	RefreshData();
}

void PlayedGameWnd::OnClickedToFlashGame()
{
	m_pBtnToFlashGame->SetCheck(!m_pBtnToFlashGame->GetCheck());
	RefreshData();
}

void PlayedGameWnd::OnClickedToCollectedGame()
{
	m_pBtnToCollectedGame->SetCheck(!m_pBtnToCollectedGame->GetCheck());
	RefreshData();
}

time_t PlayedGameWnd::str2time(const string & strTime)
{
	struct tm tmLocal;
	vector<string> date;
	vector<string> time;
	YL_StringUtil::Tokenize(strTime, date, " ");
	if (date.size() != 2) // get 2 strings like "2013-1-13" & "12:50"
		return 0;

	YL_StringUtil::Tokenize(date[1], time, ":");
	if (time.size() != 2)
		return 0;

	// set time "12:50"
	tmLocal.tm_hour = str2int(time[0]);
	tmLocal.tm_min = str2int(time[1]);

	time[0] = date[0];
	date.clear();
	YL_StringUtil::Tokenize(time[0], date, "-");
	if (date.size() != 3)
		return 0;

	// set date "2013-1-13"
	tmLocal.tm_year = str2int(date[0])-1900;
	tmLocal.tm_mon = str2int(date[1]) - 1;
	tmLocal.tm_mday = str2int(date[2]);
	return mktime(&tmLocal);
}

void PlayedGameWnd::UserMsg_Login()
{
	m_pWndLogedIn->SetUserInfo(CUserManager::GetInstance()->User_GetUserInfo());
	AfxGetUIManager()->UIGetLayoutMgr()->ShowLayer(s_USER_MAN_LOGED_IN_OUT, s_USER_MAN_LOGED_IN);
	RefreshData();
}

void PlayedGameWnd::UserMsg_LogOut()
{
	AfxGetUIManager()->UIGetLayoutMgr()->ShowLayer(s_USER_MAN_LOGED_IN_OUT, s_USER_MAN_LOGED_OUT);
	RefreshData();
}