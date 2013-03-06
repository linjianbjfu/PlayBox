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
#include "GameCenterWelcomePanelWnd.h"

static const char* s_USER_MAN_LOGED_IN_OUT	= "UserMan_LogedIn_LogedOut";
static const char* s_USER_MAN_LOGED_IN	= "UserMan_LogedIn";
static const char* s_USER_MAN_LOGED_OUT	= "UserMan_LogedOut";

static const char* s_PLAYED_GAME_WELCOME_GAMELIST = "PlayedGame_Welcome_GameList";
static const char* s_PLAYED_GAME_WELCOME	= "PlayedGameWelcome";
static const char* s_PLAYED_GAME_GAMELIST	= "PlayedGameGameListPanel";

IMPLEMENT_DYNAMIC(PlayedGameWnd, CBasicWnd)

#define NEW_SKIN_BTN(pBtn) pBtn = new CxSkinButton();

PlayedGameWnd::PlayedGameWnd()
: m_bSortTimeByAsc(true), 
  m_iGameType(0),
  m_textType(RECENT_PLAY)
{
	NEW_SKIN_BTN(m_pBtnTimeOrder);
	NEW_SKIN_BTN(m_pBtnToWebGame);
	NEW_SKIN_BTN(m_pBtnToFlashGame);
	NEW_SKIN_BTN(m_pBtnToCollectedGame);
	m_pWndGameList = new PlayedGameListPanelWnd();
	m_pWndWelcome  = new CGameCenterWelcomeWnd();
	m_pWndRecommand = new MyWebBrowserWnd();
	m_pWndLogedIn	= new CUserLogedInWnd();
	m_pWndLogedOut	= new CUserLogedOutWnd();
	//init g_textBmp
	ISkinMgr* pMgr = AfxGetUIManager()->UIGetSkinMgr();
	m_pTextBmp[RECENT_PLAY] = pMgr->GetDibBmp("PlayedGameCtrlPanelTextRecentPlay");
	m_pTextBmp[FLASH_GAME]      = pMgr->GetDibBmp("PlayedGameCtrlPanelTextFlash");
	m_pTextBmp[WEB_GAME]        = pMgr->GetDibBmp("PlayedGameCtrlPanelTextWeb");
	m_pTextBmp[COLLECT_GAME]    = pMgr->GetDibBmp("PlayedGameCtrlPanelTextCollect");

	AfxGetMessageManager()->AttachMessage(ID_MESSAGE_USER, (IUserMsgObserver*)this);
	AfxGetMessageManager()->AttachMessage(ID_MESSAGE_GAMEDATA,(ITabBarObserver*) this);
}

PlayedGameWnd::~PlayedGameWnd()
{
	delete m_pBtnTimeOrder;
	delete m_pBtnToWebGame;
	delete m_pBtnToFlashGame;
	delete m_pBtnToCollectedGame;
	delete m_pWndGameList;
	delete m_pWndWelcome;
	delete m_pWndLogedIn;
	delete m_pWndLogedOut;
	//do not delete m_pWndRecommand
	for (int i=0; i<TT_END; i++)
	{
		delete m_pTextBmp[i];
		m_pTextBmp[i] = NULL;
	}
	
	AfxGetMessageManager()->DetachMessage(ID_MESSAGE_USER, (IUserMsgObserver*)this);
	AfxGetMessageManager()->DetachMessage(ID_MESSAGE_GAMEDATA, (IUserMsgObserver*)this);
}

BEGIN_MESSAGE_MAP(PlayedGameWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
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
	m_pWndGameList->Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,rectNULL,this,ID_WND_PLAYED_GAME_LIST);
	m_pWndWelcome->Create(NULL,NULL,WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,rectNULL,this,ID_WND_PLAYED_GAME_WELCOME);
	m_pWndRecommand->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNULL,this,ID_WND_PLAYED_GAME_RECOMMAND);
	m_pWndLogedIn->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNULL,this,ID_WND_USER_LOGED_IN);
	m_pWndLogedOut->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNULL,this,ID_WND_USER_LOGED_OUT);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->RegisterCtrl(this,"PlayedGameTimeOrder",m_pBtnTimeOrder);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameToWebGame",m_pBtnToWebGame);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameToFlashGame",m_pBtnToFlashGame);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameToCollectedGame",m_pBtnToCollectedGame);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameGameListPanel",m_pWndGameList);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameWelcome",m_pWndWelcome);
	pLayoutMgr->RegisterCtrl(this,"PlayedGameRecommandWebPanel",m_pWndRecommand);
	pLayoutMgr->RegisterCtrl(this,"UserMan_LogedIn",m_pWndLogedIn);
	pLayoutMgr->RegisterCtrl(this,"UserMan_LogedOut",m_pWndLogedOut);

	pLayoutMgr->CreateControlPane( this,"playedgamepanel","normal");
	pLayoutMgr->CreateBmpPane( this,"playedgamepanel","normal" );

	AfxGetUIManager()->UIGetLayoutMgr()->ShowLayer(s_USER_MAN_LOGED_IN_OUT, s_USER_MAN_LOGED_OUT);

	SetOnlyOneBtnCheckedAndValidate(m_pBtnTimeOrder);
	std::string strPlayedGameRightUrl;
	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_CONFIG_PLAYED_GAME_RIGHT_URL,strPlayedGameRightUrl);

	m_pWndRecommand->Navigate(strPlayedGameRightUrl.empty() ? 
		"about:blank" : strPlayedGameRightUrl);
	return 0;
}

void PlayedGameWnd::OnDestroy()
{
	m_pWndGameList->DestroyWindow();
	m_pWndWelcome->DestroyWindow();
	m_pWndLogedIn->DestroyWindow();
	m_pWndLogedOut->DestroyWindow();
	m_pWndRecommand->DestroyWindow();
	__super::OnDestroy();
}

//这个函数的意义发生变化，不是按时间排序，而是现实所有的最近玩过的游戏
void PlayedGameWnd::OnClickedTimerOrder()
{
	SetOnlyOneBtnCheckedAndValidate(m_pBtnTimeOrder);
	//下面是按时间排序的实现方法
	/*
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
	m_pWndGameList->ReSetGameList(m_gameList);
	*/
}

void PlayedGameWnd::SetOnlyOneBtnCheckedAndValidate(CxSkinButton* pBtn)
{
	if (pBtn->GetCheck() == BST_CHECKED)
		return;

	m_pBtnTimeOrder->SetCheck(m_pBtnTimeOrder == pBtn ? BST_CHECKED : BST_UNCHECKED);
	m_pBtnToWebGame->SetCheck(m_pBtnToWebGame == pBtn ? BST_CHECKED : BST_UNCHECKED);
	m_pBtnToFlashGame->SetCheck(m_pBtnToFlashGame == pBtn ? BST_CHECKED : BST_UNCHECKED);
	m_pBtnToCollectedGame->SetCheck(m_pBtnToCollectedGame == pBtn ? BST_CHECKED : BST_UNCHECKED);

	if(m_pBtnTimeOrder == pBtn)
		m_textType = RECENT_PLAY;
	else if(m_pBtnToWebGame == pBtn)
		m_textType = WEB_GAME;
	else if(m_pBtnToFlashGame == pBtn)
		m_textType = FLASH_GAME;
	else if(m_pBtnToCollectedGame == pBtn)
		m_textType = COLLECT_GAME;

	ValidateInterface();
}

void PlayedGameWnd::OnClickedToWebGame()
{
	SetOnlyOneBtnCheckedAndValidate(m_pBtnToWebGame);
}

void PlayedGameWnd::OnClickedToFlashGame()
{
	SetOnlyOneBtnCheckedAndValidate(m_pBtnToFlashGame);
}

void PlayedGameWnd::OnClickedToCollectedGame()
{
	SetOnlyOneBtnCheckedAndValidate(m_pBtnToCollectedGame);
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

void PlayedGameWnd::ValidateInterface()
{
	m_iGameType = 0;
	if (m_pBtnTimeOrder->GetCheck())
		m_iGameType |= OneGame::FLASH_GAME | OneGame::WEB_GAME;
	if (m_pBtnToWebGame->GetCheck())
		m_iGameType |= OneGame::WEB_GAME;
	if (m_pBtnToFlashGame->GetCheck())
		m_iGameType |= OneGame::FLASH_GAME;
	if (m_pBtnToCollectedGame->GetCheck())
		m_iGameType |= OneGame::COLLECTED;
	GLOBAL_GAME->IGameData_GetGame(m_gameList, m_iGameType);
	if(m_gameList.empty())
		AfxGetUIManager()->UIGetLayoutMgr()->ShowLayer(s_PLAYED_GAME_WELCOME_GAMELIST, s_PLAYED_GAME_WELCOME);
	else
	{
		AfxGetUIManager()->UIGetLayoutMgr()->ShowLayer(s_PLAYED_GAME_WELCOME_GAMELIST, s_PLAYED_GAME_GAMELIST);
		m_pWndGameList->ReSetGameList(m_gameList);
	}	
	Invalidate(FALSE);
}

void PlayedGameWnd::UserMsg_Login()
{
	m_pWndLogedIn->SetUserInfo(CUserManager::GetInstance()->User_GetUserInfo());
	AfxGetUIManager()->UIGetLayoutMgr()->ShowLayer(s_USER_MAN_LOGED_IN_OUT, s_USER_MAN_LOGED_IN);
	ValidateInterface();
}

void PlayedGameWnd::UserMsg_LogOut()
{
	AfxGetUIManager()->UIGetLayoutMgr()->ShowLayer(s_USER_MAN_LOGED_IN_OUT, s_USER_MAN_LOGED_OUT);
	ValidateInterface();
}

void PlayedGameWnd::OnPaint()
{
	CPaintDC dc(this);
	AfxGetUIManager()->UIGetLayoutMgr()->PaintBkGround( m_hWnd ,&dc,false );
	
	if (m_pTextBmp[m_textType] != NULL)
	{
		CRect rc(34, 43, 108, 61);
		m_pTextBmp[m_textType]->SetCDibRect(rc);
		m_pTextBmp[m_textType]->Draw(&dc, FALSE);
	}
}

void PlayedGameWnd::IGameData_Changed()
{
	ValidateInterface();
}