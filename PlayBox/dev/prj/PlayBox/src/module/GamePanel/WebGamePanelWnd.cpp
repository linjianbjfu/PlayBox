#include "stdafx.h"
#include "resource.h"
#include "YL_StringUtil.h"
#include "YL_FileInfo.h"
#include "YL_HTTPDownFile.h"
#include "WebGamePanelWnd.h"
#include "../WebInteract/MyWebBrowserWnd.h"
#include "../../AppConfig/config/ConfigAppDef.h"
#include "../WebInteract/WebManager.h"
#include "../../core/CDataManager.h"
#include "../../gui/CommonControl/xSkinButton.h"
#include "../../gui/CommonControl/xStaticText.h"
#include "../../Gui/CommonControl/EditEx.h"
#include "util/Sound.h"
#include "AppConfig/config/ConfigSettingDef.h"
#include "src/module/CleanCachePanel/CleanCacheDlg.h"
#include "src/module/Esc2ExitFullScrPanel/ESCFullDlg.h"
#include "src/AppConfig/config/ConfigLayoutDef.h"
#include ".\webgamepanelwnd.h"
#include "util\md5.h"
#include "..\UserMan\UserInfo.h"
#include "..\UserMan\UserManager.h"


IMPLEMENT_DYNAMIC(WebGamePanelWnd, CBasicWnd)
WebGamePanelWnd::WebGamePanelWnd()
{
	m_isMainWindowTopMost = false;
	m_bFullScreen = false;

	m_pWndWebGame	= new MyWebBrowserWnd();
	m_pBtnRefresh	= new CxSkinButton();
	m_pBtnToFull	= new CxSkinButton();
	m_pBtnExitFull	= new CxSkinButton();
	m_pBtnMute	= new CxSkinButton();
	m_pBtnUnMute	= new CxSkinButton();
	m_pBtnClearCache	= new CxSkinButton();
	m_pBtnSite	= new CxSkinButton();
	m_pBtnCustomService	= new CxSkinButton();
	m_pBtnPay	= new CxSkinButton();

	m_pEscFullTipDlg	= new ESCFullDlg;

	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*) this);
}

WebGamePanelWnd::~WebGamePanelWnd()
{
	delete m_pBtnRefresh;
	delete m_pBtnToFull;
	delete m_pBtnExitFull;
	delete m_pBtnMute;
	delete m_pBtnUnMute;
	delete m_pBtnClearCache;
	delete m_pBtnSite;
	delete m_pBtnCustomService;
	delete m_pBtnPay;

	if( IsWindow( m_pEscFullTipDlg->m_hWnd) )
	{
		m_pEscFullTipDlg->DestroyWindow();

	}
	delete m_pEscFullTipDlg;

	//do not delete m_pWndWebGame
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*) this);
}

BEGIN_MESSAGE_MAP(WebGamePanelWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_WEBGAME_REFRESH,OnClickedRefresh)
	ON_BN_CLICKED(IDC_BTN_WEBGAME_TO_FULL,OnClickedToFull)
	ON_BN_CLICKED(IDC_BTN_WEBGAME_EXIT_FULL,OnClickedExitFull)
	ON_BN_CLICKED(IDC_BTN_WEBGAME_MUTE,OnClickedMute)
	ON_BN_CLICKED(IDC_BTN_WEBGAME_UNMUTE,OnClickedUnMute)
	ON_BN_CLICKED(IDC_BTN_WEBGAME_CLEAR_CACHE,OnClickedClearCache)
	ON_BN_CLICKED(IDC_BTN_WEBGAME_SITE,OnClickedSite)
	ON_BN_CLICKED(IDC_BTN_WEBGAME_CUSTOM_SERVICE,OnClickedCustomService)
	ON_BN_CLICKED(IDC_BTN_WEBGAME_PAY,OnClickedPay)
END_MESSAGE_MAP()

int WebGamePanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	CRect rectNull(0,0,0,0);
	m_pWndWebGame->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_WND_WEBGAMEPLAYING );
	m_pBtnRefresh->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_REFRESH);
	m_pBtnToFull->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_TO_FULL);
	m_pBtnExitFull->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_EXIT_FULL);
	m_pBtnMute->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_MUTE);
	m_pBtnUnMute->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_UNMUTE);
	m_pBtnClearCache->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_CLEAR_CACHE);
	m_pBtnSite->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_SITE);
	m_pBtnCustomService->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_CUSTOM_SERVICE);
	m_pBtnPay->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_WEBGAME_PAY);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->RegisterCtrl( this, "WebGameRefresh", m_pBtnRefresh );
	pLayoutMgr->RegisterCtrl( this, "WebGameToFull", m_pBtnToFull );
	pLayoutMgr->RegisterCtrl( this, "WebGameToSmall", m_pBtnExitFull );
	pLayoutMgr->RegisterCtrl( this, "WebGameMute", m_pBtnMute );
	pLayoutMgr->RegisterCtrl( this, "WebGameUnMute", m_pBtnUnMute );
	pLayoutMgr->RegisterCtrl( this, "WebGameClearCache", m_pBtnClearCache );
	pLayoutMgr->RegisterCtrl( this, "WebGameSite", m_pBtnSite );
	pLayoutMgr->RegisterCtrl( this, "WebGameCustomService", m_pBtnCustomService );
	pLayoutMgr->RegisterCtrl( this, "WebGamePay", m_pBtnPay );
	pLayoutMgr->RegisterCtrl( this, "WebGameBrowser", m_pWndWebGame );

	pLayoutMgr->CreateControlPane( this, "webgamepanel", "normal" );
	pLayoutMgr->CreateBmpPane( this,"webgamepanel","normal" );

	if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
	{
		m_pBtnToFull->ShowWindow(SW_HIDE);
		m_pBtnExitFull->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pBtnToFull->ShowWindow(SW_SHOW);
		m_pBtnExitFull->ShowWindow(SW_HIDE);
	}

	bool bIsMute = CSound::GetInstance()->GetMute( 1 );
	if ( bIsMute )
	{
		m_pBtnMute->ShowWindow(SW_HIDE);
		m_pBtnUnMute->ShowWindow(SW_SHOW);
	}
	else
	{
		m_pBtnMute->ShowWindow(SW_SHOW);
		m_pBtnUnMute->ShowWindow(SW_HIDE);
	}

	m_pEscFullTipDlg->Create(IDD_DIALOG_ESCFULLSCR, this);

	return 0;
}

void WebGamePanelWnd::LoadSkin()
{
}

/*
$flag=MD5($id.$svrid.$username.$type.MD5($key));
$type="najiuwanbox";$key="69456fh45m9d1e77li
*/
bool WebGamePanelWnd::GenerateFlag(OUT std::string& strFlag,
								   IN const std::string& id,
								   IN const std::string& svrid,
								   IN const std::string& userName)
{
	strFlag.clear();
	static std::string strType = "najiuwanbox";
	static std::string strKey = "69456fh45m9d1e77li";
	static std::string strKeyMD5;
	if (strKeyMD5.empty())
	{
		char szKey[128] = {0};
		strcpy(szKey, strKey.c_str());
		char szKeyMD5[32] = {0};
		MD5String(szKey, szKeyMD5);
		strKeyMD5 = std::string(szKeyMD5);
	}
	strFlag = id + svrid + userName + strType + strKeyMD5;
	return true;
}

void WebGamePanelWnd::SetTabItem( TAB_ITEM ti )
{
	m_webGame.strID = CWebManager::GetInstance()->GetValue( ti.strParam, "id" );
	m_webGame.strSvrID = CWebManager::GetInstance()->GetValue( ti.strParam, "svrid" );
	m_webGame.strName = CWebManager::GetInstance()->GetValue( ti.strParam, "name" );
	m_webGame.strPicUrl = CWebManager::GetInstance()->GetValue( ti.strParam, "picurl" );
	string strValue;
	AfxGetUserConfig()->GetConfigStringValue( CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_CONFIG_WEB_GAME_URL, strValue );
	if (!strValue.empty())
	{
		/*
		http://box.najiuwan.com/index.php?s=/game/login_game_box/
		username/yinshaohua/pass/1111/flag/asdasd23432fdsfdsf/id/107/svrid/2
		*/
		UserInfo* pUserInfo = CUserManager::GetInstance()->User_GetUserInfo();
		if (pUserInfo == NULL)
			return;
		
		std::string strFlag;
		GenerateFlag(strFlag, m_webGame.strID, m_webGame.strSvrID, pUserInfo->strName);
		string strUrl = strValue + "username/" + pUserInfo->strName +
			"/pass/" + pUserInfo->strPassMD5 +
			"/flag/" + strFlag +
			"/id/" + m_webGame.strID + 
			"/svrid/" + m_webGame.strSvrID;
		m_pWndWebGame->Navigate(strUrl);

		if ( YL_FileInfo::IsValid(m_webGame.strPicUrl) )
		{ // picurl假如指向了本地文件,则是从本地游戏列表进入的游戏
			return;
		}

		OneGame og;
		og.strName	= m_webGame.strName;
		og.strID	= m_webGame.strID;
		og.strSrvID	= m_webGame.strSvrID;
		og.strGamePath=strUrl;

		string strSavePath;
		AfxGetUserConfig()->GetConfigStringValue(CONF_APP_MODULE_NAME, CONF_APP_SWF_PATH, strSavePath);
		if (strSavePath[strSavePath.length()-1] != '\\')
		strSavePath += '\\';

		YL_StringUtil::ReplaceAll( m_webGame.strPicUrl, " ", "%20" );
		string strPicFormat;
		YL_FileInfo::GetFileNameSuffix( m_webGame.strPicUrl, strPicFormat );


		string strPicLocalDesPath;
		YL_StringUtil::Format( strPicLocalDesPath, "%s%s_2.%s", 
		strSavePath.c_str(), og.strID.c_str(), strPicFormat.c_str() );

		og.strPicPath = strPicLocalDesPath;
		YL_CHTTPDownFile httpDownFile;
		httpDownFile.DownloadFile( m_webGame.strPicUrl,  strPicLocalDesPath);

		og.nGameType= OneGame::WEB_GAME | OneGame::RECENT_PLAY;
		GLOBAL_GAME->IGameData_AddGame(og);
	}
}

void WebGamePanelWnd::Init()
{
	m_pWndWebGame->Init();
}

void WebGamePanelWnd::OnDestroy()
{
	m_pWndWebGame->DestroyWindow();
	__super::OnDestroy();
}

void WebGamePanelWnd::IPanelChangeOb_ToFullScreen( CWnd* pWnd )
{
	if( pWnd != this )
		return;

	m_isMainWindowTopMost = false;
	bool curState;
	AfxGetUserConfig()->GetConfigBoolValue( CONF_APP_MODULE_NAME,CONF_APP_MAINWND_HOLD,curState);
	if(curState)
	{
		SetMainWindow(false);
		m_isMainWindowTopMost=true;
	}

	m_pWndParent = GetParent();
	GetWindowRect(&m_rectBeforeFull);
	m_pWndParent->ScreenToClient(&m_rectBeforeFull);

	//去掉子窗口风格,这样WebGamePanelWnd才能全屏时获得焦点
	long style = ::GetWindowLong(m_hWnd,GWL_STYLE);
	style &= ~WS_CHILD;
	//style &= ~WS_CLIPCHILDREN;
	::SetWindowLong(m_hWnd, GWL_STYLE, style);

	long lExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	lExStyle &= ~WS_EX_APPWINDOW; 
	lExStyle |= WS_EX_TOOLWINDOW; 
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, lExStyle);

	SetParent(GetDesktopWindow());
	GetDesktopWindow()->GetWindowRect(&m_rectFullScreen);

	SetWindowPos(&wndTopMost,m_rectFullScreen.left,m_rectFullScreen.top,
		m_rectFullScreen.Width(),m_rectFullScreen.Height(),SWP_SHOWWINDOW);
	m_bFullScreen = true;

	m_pBtnToFull->ShowWindow(SW_HIDE);
	m_pBtnExitFull->ShowWindow(SW_SHOW);
	ShowHideEseFull(true);
	//m_pEscFullTipDlg->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
	SetTimer(ID_TIMER_ESCFULL_TIP, TIME_TIMER_ESCFULL_TIP, NULL);
}

void WebGamePanelWnd::SetMainWindow(bool isTopMost)
{
	YL_Log("WebGamePanelWnd.txt", LOG_DEBUG, "WebGamePanelWnd::SetMainWindow", "===IN");
	if(isTopMost)
	{
		AfxGetMainWindow()->SetWindowPos(&wndTopMost,-1,-1,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
		AfxGetUserConfig()->SetConfigBoolValue( CONF_APP_MODULE_NAME,CONF_APP_MAINWND_HOLD,true);
	}else
	{
		AfxGetMainWindow()->SetWindowPos(&wndNoTopMost,-1,-1,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
		AfxGetUserConfig()->SetConfigBoolValue( CONF_APP_MODULE_NAME,CONF_APP_MAINWND_HOLD,false);
	}
	YL_Log("WebGamePanelWnd.txt", LOG_DEBUG, "WebGamePanelWnd::SetMainWindow", "===OUT");
}

void WebGamePanelWnd::IPanelChangeOb_ExitFullScreen( CWnd* pWnd )
{
	YL_Log("WebGamePanelWnd.txt", LOG_DEBUG, "WebGamePanelWnd::IPanelChangeOb_ExitFullScreen", "===IN");
	if( pWnd != this )
		return;

	m_bFullScreen = false;

	//加上子窗口风格
	long style = ::GetWindowLong(m_hWnd,GWL_STYLE);
	style |= WS_CHILD;
	//style |= WS_CLIPCHILDREN; //去掉WS_CLIPCHILDREN后，WebGamePanelWnd刷新时就会有剧烈闪烁
	::SetWindowLong(m_hWnd, GWL_STYLE, style);
	
	long lExStyle = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
	lExStyle |= WS_EX_APPWINDOW; 
	lExStyle &= ~WS_EX_TOOLWINDOW; 
	::SetWindowLong(m_hWnd, GWL_EXSTYLE, lExStyle);

	SetParent(m_pWndParent);
	MoveWindow(&m_rectBeforeFull);

	if(m_isMainWindowTopMost)
	{
		SetMainWindow(true);
		m_isMainWindowTopMost = false;
	}

	bool bHold = false;
	AfxGetUserConfig()->GetConfigBoolValue( CONF_APP_MODULE_NAME,CONF_APP_MAINWND_HOLD,bHold);
	if( bHold )
	{
		AfxGetMainWindow()->SetWindowPos(&wndTopMost,-1,-1,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	}

	m_pBtnToFull->ShowWindow(SW_SHOW);
	m_pBtnExitFull->ShowWindow(SW_HIDE);
	KillTimer(ID_TIMER_ESCFULL_TIP);
	ShowHideEseFull(false);
	YL_Log("WebGamePanelWnd.txt", LOG_DEBUG, "WebGamePanelWnd::IPanelChangeOb_ExitFullScreen", "===OUT");
}

BOOL WebGamePanelWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void WebGamePanelWnd::Recycle()
{
	Init();
}

void WebGamePanelWnd::OnClickedRefresh()
{
	m_pWndWebGame->Refresh();
}

void WebGamePanelWnd::OnClickedToFull()
{
	if( !GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
	{
		GLOBAL_PANELCHANGEDATA->IPanelChange_ToFullScreen( this );
	}
}

void WebGamePanelWnd::OnClickedExitFull()
{
	if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
	{
		GLOBAL_PANELCHANGEDATA->IPanelChange_ExitFullScreen();
	}
}

void WebGamePanelWnd::OnClickedMute()
{
	bool bIsMute = CSound::GetInstance()->GetMute( 1 );
	CSound::GetInstance()->SetMute( 1, !bIsMute );

	if (!bIsMute)
	{
		CSound::GetInstance()->SetMute( 1, TRUE );
		m_pBtnMute->ShowWindow(SW_HIDE);
		m_pBtnUnMute->ShowWindow(SW_SHOW);
	}
}

void WebGamePanelWnd::OnClickedUnMute()
{
	bool bIsMute = CSound::GetInstance()->GetMute( 1 );

	if (bIsMute)
	{
		CSound::GetInstance()->SetMute( 1, FALSE );
		m_pBtnMute->ShowWindow(SW_SHOW);
		m_pBtnUnMute->ShowWindow(SW_HIDE);
	}
}

void WebGamePanelWnd::OnClickedClearCache()
{
	CCleanCacheDlg dlg(this);
	AfxGetUIManager()->UIAddDialog(&dlg);
	int ret = dlg.DoModal();
	AfxGetUIManager()->UIRemoveDialog(&dlg);

	if (ret == IDOK)
	{
		// 重启游戏
		OnClickedRefresh();
	}
}

void WebGamePanelWnd::OnClickedSite()
{
	string strValue;
	AfxGetUserConfig()->GetConfigStringValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_CONFIG_WEB_GAME_OFFICE_SITE, strValue );
	if (!strValue.empty())
	{
		TAB_ITEM tabItem;
		tabItem.strTitle = TAB_BROWSER_DEFAULT_TITLE;
		tabItem.enumType = TAB_BROWSER;
		tabItem.strParam = "url=" + strValue + m_webGame.strID;
		GLOBAL_TABBARDATA->ITabBar_ChangeTab( tabItem );
	}
}

void WebGamePanelWnd::OnClickedCustomService()
{
	string strValue;
	AfxGetUserConfig()->GetConfigStringValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_CONFIG_WEB_GAME_CUSTOM_SERVICE, strValue );
	if (!strValue.empty())
	{
		TAB_ITEM tabItem;
		tabItem.strTitle = TAB_WEB_GAME_CUSTOM_SERVICE_TITLE;
		tabItem.enumType = TAB_BROWSER;
		tabItem.strParam = "url=" + strValue;
		GLOBAL_TABBARDATA->ITabBar_ChangeTab( tabItem );
	}
}

void WebGamePanelWnd::OnClickedPay()
{
	string strValue;
	AfxGetUserConfig()->GetConfigStringValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_CONFIG_PAY, strValue );
	if (!strValue.empty())
	{
		TAB_ITEM tabItem;
		tabItem.strTitle = TAB_PAY_TITLE;
		tabItem.enumType = TAB_BROWSER;
		tabItem.strParam = "url=" + strValue + m_webGame.strID;
		GLOBAL_TABBARDATA->ITabBar_ChangeTab( tabItem );
	}
}

void WebGamePanelWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ID_TIMER_ESCFULL_TIP)
	{
		KillTimer(nIDEvent);
		ShowHideEseFull(false);
	}
}

void WebGamePanelWnd::ShowHideEseFull(bool isShow)
{
	//全屏时，disable两个按钮EQ和Desklyric
	/////////////////////////
	int allValue=0;
	static int curValue=0;
	AfxGetUserConfig()->GetConfigIntValue(CONF_LAYOUT_MODULE_NAME, CONF_LAYOUT_ESC_ALL, allValue);
	if( isShow
		&& (curValue<1)
		&& (allValue<5))
	{
		curValue++;
		m_pEscFullTipDlg->ShowDlg( TRUE );

		if(allValue==0)
		{
			LogRealMsg( "ENTER_FULLSCREEN","Yes" );
		}
		allValue=allValue+1;
		allValue = 0;
		AfxGetUserConfig()->SetConfigIntValue(CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_ESC_ALL,allValue,true);

	}else if( (isShow==false)
		&& (curValue<=1) )
	{
		m_pEscFullTipDlg->ShowDlg( FALSE );
	}
}

BOOL WebGamePanelWnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			OnClickedExitFull();
			return TRUE;
		}
	}
	return __super::PreTranslateMessage(pMsg);
}
