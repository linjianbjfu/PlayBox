#include "stdafx.h"
#include <atlimage.h>
#include <gdiplus.h>
#include "YL_StringUtil.h"
#include "YL_FileInfo.h"
#include "YL_RegInfo.h"
#include "YL_EncFileReg.h"
#include "GamePanelWnd.h"
#include "../WebInteract/MyWebBrowserWnd.h"
#include "../WebInteract/WebManager.h"
#include "../../AppConfig/config/ConfigAppDef.h"
#include "../../DataInterface/IGameData.h"
#include "../../core/CDataManager.h"
#include "../../Global/GlobalSwfPath.h"
#include "../../gui/CommonControl/xSkinButton.h"
#include "../../gui/CommonControl/xStaticText.h"
#include "../../module/httpDown/HttpDownCtrl.h"
#include "../../Gui/CommonControl/ShockwaveFlash.h"
#include "DownPercentWnd.h"
#include "util/Sound.h"
#include "src/module/Esc2ExitFullScrPanel/ESCFullDlg.h"
#include "src/AppConfig/config/ConfigLayoutDef.h"
#include "AppConfig/config/ConfigSettingDef.h"

IMPLEMENT_DYNAMIC(GamePanelWnd, CBasicWnd)
GamePanelWnd::GamePanelWnd()
{
	m_isMainWindowTopMost = false;
	m_bFullScreen           = false;
	m_bDown		            = false;
	m_isMainWindowTopMost	= false;
	m_bFullScreen			= false;

	this->m_iStep = 0;

	//2012-12-13
	m_pGameFlash      = new CShockwaveFlash();
	m_pWndDownPercent = new DownPercentWnd();
	m_pBtnRePlay      = new CxSkinButton();
	m_pBtnMute      = new CxSkinButton();
	m_pBtnUnMute      = new CxSkinButton();
	m_pBtnToFullScreen      = new CxSkinButton();
	m_pBtnExitFullScreen      = new CxSkinButton();
	m_pBtnPause      = new CxSkinButton();

	m_pWndRight = new MyWebBrowserWnd();
	m_pWndBottom = new MyWebBrowserWnd();
	m_pEscFullTipDlg = new ESCFullDlg;

	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_LAYOUTMGR,(ILayoutChangeObserver*) this);
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*) this);
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_HTTP_DOWN,(IHttpDownObserver*) this);
}

GamePanelWnd::~GamePanelWnd()
{
	delete m_pGameFlash;
	delete m_pWndDownPercent;
	delete m_pBtnRePlay;
	delete m_pBtnMute;
	delete m_pBtnUnMute;
	delete m_pBtnToFullScreen;
	delete m_pBtnExitFullScreen;
	delete m_pBtnPause;
	//do not delete MyWebBrowserWnd ptr, you can see now it's hwnd is feeefeee
	//delete m_pWndRight;
	//delete m_pWndBottom;

	if( IsWindow( m_pEscFullTipDlg->m_hWnd) )
		m_pEscFullTipDlg->DestroyWindow();

	delete m_pEscFullTipDlg;

	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_LAYOUTMGR,(ILayoutChangeObserver*) this);
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*) this);
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_HTTP_DOWN,(IHttpDownObserver*) this);
}

BEGIN_MESSAGE_MAP(GamePanelWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_REPLAY,OnClickedReplay)
	ON_BN_CLICKED(IDC_BTN_FULL_SCREEN,OnClickedFullScreen)
	ON_BN_CLICKED(IDC_BTN_EXIT_FULL_SCREEN,OnClickedExitFullScreen)
	ON_BN_CLICKED(IDC_BTN_MUTE,OnClickedMute)
	ON_BN_CLICKED(IDC_BTN_UN_MUTE,OnClickedUnMute)
	ON_BN_CLICKED(IDC_BTN_PAUSE,OnClickedPause)
END_MESSAGE_MAP()

void GamePanelWnd::InitFlashParams(CShockwaveFlash*	pGameFlash)
{
	pGameFlash->SetBackgroundColor( -1 );//无背景色
	pGameFlash->SetAlignMode( 2 );       //
	pGameFlash->SetMenu( FALSE );        //无扩展的弹出菜单 
	pGameFlash->SetScale( "ExactFit" );  //缩放模式为能显示动画所有内容
	pGameFlash->SetWMode( "Window" );    //设置窗口模式
	pGameFlash->SetQuality( 1 );         //设置播放动画质量为高 
	pGameFlash->SetEmbedMovie(TRUE);     //动画是内嵌的 
}

int GamePanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectNull(0,0,0,0);
	m_pGameFlash->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN|WS_VISIBLE, rectNull, this, ID_WND_GAME_SWF);
	InitFlashParams(m_pGameFlash);
	m_pBtnRePlay->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_REPLAY);
	m_pBtnToFullScreen->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_FULL_SCREEN);
	m_pBtnExitFullScreen->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_EXIT_FULL_SCREEN);
	m_pBtnMute->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_MUTE);
	m_pBtnUnMute->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_UN_MUTE);
	m_pBtnPause->Create (NULL, WS_VISIBLE, rectNull, this, IDC_BTN_PAUSE);
	m_pWndRight->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_WND_FLASHGAME_RIGHT );
	m_pWndBottom->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_WND_FLASHGAME_BOTTOM );

	m_pWndDownPercent->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_WND_GAME_DOWN_PERCENT );

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();	
	pLayoutMgr->RegisterCtrl( this, "GameFlash", m_pGameFlash );
	pLayoutMgr->RegisterCtrl( this, "GameRePlay", m_pBtnRePlay );
	pLayoutMgr->RegisterCtrl( this, "ToFull", m_pBtnToFullScreen );
	pLayoutMgr->RegisterCtrl( this, "ExitFull", m_pBtnExitFullScreen );
	pLayoutMgr->RegisterCtrl( this, "Mute", m_pBtnMute );
	pLayoutMgr->RegisterCtrl( this, "UnMute", m_pBtnUnMute );
	pLayoutMgr->RegisterCtrl (this, "Pause", m_pBtnPause);
	pLayoutMgr->RegisterCtrl (this, "FlashGameBottom", m_pWndBottom);
	pLayoutMgr->RegisterCtrl (this, "FlashGameRight", m_pWndRight);
	pLayoutMgr->CreateControlPane( this, "gamepanel", "normal" );
	pLayoutMgr->CreateBmpPane( this,"gamepanel","normal" );

	m_pEscFullTipDlg->Create(IDD_DIALOG_ESCFULLSCR, this);

	UpdateAllWnd();
	return 0;
}

void GamePanelWnd::LoadSkin()
{
}

void GamePanelWnd::SetTabItem( TAB_ITEM ti )
{
	m_swfGame.strID = CWebManager::GetInstance()->GetValue( ti.strParam, "id" );
	m_swfGame.strName = CWebManager::GetInstance()->GetValue( ti.strParam, "name" );
	m_swfGame.strPicUrl = CWebManager::GetInstance()->GetValue( ti.strParam, "picurl" );
	m_swfGame.strSwfUrl = CWebManager::GetInstance()->GetValue( ti.strParam, "swfurl" );
	SetGameEntry( m_swfGame );
}

void GamePanelWnd::Init()
{
	if( m_strCopyedHtmlPath.length() != 0 )
	{
		::DeleteFile( m_strCopyedHtmlPath.c_str() );
		m_strCopyedHtmlPath.clear();
	}
}

void GamePanelWnd::Recycle()
{
	Init();
}

void GamePanelWnd::OnDestroy()
{
	m_pGameFlash->DestroyWindow();
	m_pWndDownPercent->DestroyWindow();
	m_pWndBottom->DestroyWindow();
	m_pWndRight->DestroyWindow();
	//删除播放swf的htm文件
	if( !m_strCopyedHtmlPath.empty() )
		::DeleteFile( m_strCopyedHtmlPath.c_str() );

	__super::OnDestroy();
}

void GamePanelWnd::IPanelChangeOb_ToFullScreen( CWnd* pWnd )
{
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::IPanelChangeOb_ToFullScreen", "===IN");
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

	//去掉子窗口风格,这样GamePanelWnd才能全屏时获得焦点
	long style = ::GetWindowLong(m_hWnd,GWL_STYLE);
	style &= ~WS_CHILD;
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

	//游戏界面获取焦点
	// m_pGameFlash->SetFocus();

	ShowHideEseFull(true);
	SetTimer(ID_TIMER_ESCFULL_TIP, TIME_TIMER_ESCFULL_TIP, NULL);
}

void GamePanelWnd::SetMainWindow(bool isTopMost)
{
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::SetMainWindow", "===IN");
	if(isTopMost)
	{
		AfxGetMainWindow()->SetWindowPos(&wndTopMost,-1,-1,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
		AfxGetUserConfig()->SetConfigBoolValue( CONF_APP_MODULE_NAME,CONF_APP_MAINWND_HOLD,true);
	}else
	{
		AfxGetMainWindow()->SetWindowPos(&wndNoTopMost,-1,-1,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
		AfxGetUserConfig()->SetConfigBoolValue( CONF_APP_MODULE_NAME,CONF_APP_MAINWND_HOLD,false);
	}
}

void GamePanelWnd::IPanelChangeOb_ExitFullScreen( CWnd* pWnd )
{
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::IPanelChangeOb_ExitFullScreen", "===IN");
	if( pWnd != this )
		return;

	m_bFullScreen = false;

	//加上子窗口风格
	long style = ::GetWindowLong(m_hWnd,GWL_STYLE);
	style |= WS_CHILD;
	//style |= WS_CLIPCHILDREN; //去掉WS_CLIPCHILDREN后，GamePanelWnd刷新时就会有剧烈闪烁
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
	UpdateAllWnd();
	//游戏界面获取焦点
	// m_pGameFlash->SetFocus();

	KillTimer(ID_TIMER_ESCFULL_TIP);
	ShowHideEseFull(false);
}

void GamePanelWnd::HttpDownOb_DownStart( string& strID )
{
	if( strID != m_swfGame.strID )
		return;
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::HttpDownOb_DownStart", "===IN");
	m_bDown = true;
	UpdateAllWnd();

	m_pWndDownPercent->SetFailed( false );
	vector<string> vecText;
	vecText.push_back( "开始下载" );

	m_pWndDownPercent->SetText( vecText );
	m_pWndDownPercent->SetDownPercent( 0 );
}

void GamePanelWnd::HttpDownOb_DownFinish( string& strID, string& strSwfPath )
{
	if( strID != m_swfGame.strID )
		return;

	//进度窗口参数
	m_pWndDownPercent->SetFailed( false );
	vector<string> vecText;
	//Set the download percentage.
	vecText.push_back( "下载完成,启动中..." );
	m_pWndDownPercent->SetText( vecText );
	m_pWndDownPercent->SetDownPercent( 1.0 );

	//处理swfurl，去掉空格
	YL_StringUtil::ReplaceAll( m_swfGame.strPicUrl, " ", "%20" );
	//下载缩略图
	string strPicLocalDesPath;
	string strPicFormat;
	YL_FileInfo::GetFileNameSuffix( m_swfGame.strPicUrl, strPicFormat );

	string strPicDirPath;
	YL_FileInfo::GetFileDirPath( strSwfPath, strPicDirPath );
	
	char szHomePath[MAX_PATH];
	CLhcImg::GetHomePath( szHomePath, MAX_PATH );

	YL_StringUtil::Format( strPicLocalDesPath, "%s\\%s.%s", 
		strPicDirPath.c_str(), m_swfGame.strID.c_str(), strPicFormat.c_str() );

	if( !YL_FileInfo::IsValid( strPicLocalDesPath ) )
	{
		YL_CHTTPDownFile httpDownFile;
		httpDownFile.DownloadFile( m_swfGame.strPicUrl, strPicLocalDesPath );
	}

	OneGame og;
	og.strID = strID;
	og.strName = m_swfGame.strName;
	og.strPicPath = strPicLocalDesPath;
	og.strGamePath = strSwfPath;
	og.strIntro = m_swfGame.strIntro;
	og.nGameType = OneGame::FLASH_GAME | OneGame::RECENT_PLAY;
	GLOBAL_GAME->IGameData_AddGame( og );

	OneGame olg;
	GLOBAL_GAME->IGameData_GetGameByID( strID, og.nGameType, olg );

	//File has been downloaded ,Try to play the Movie now...
	PlayMovie( olg.strID, olg.strGamePath );
	m_bDown = false;
	UpdateAllWnd();
}

void GamePanelWnd::HttpDownOb_DownFailed( string& strID, HTTP_DOWN_FAILED_REASON r )
{
	if( strID != m_swfGame.strID )
		return;

	m_bDown = true;
	UpdateAllWnd();

	m_pWndDownPercent->SetFailed( true );
	vector<string> vecText;

	string strFailedReason = "不明原因";
	switch( r )
	{
	case HTTP_FAILED_CANT_CREATE_LOCAL_FILE:
		{
			string strValue;
			AfxGetUserConfig()->GetConfigStringValue( CONF_APP_MODULE_NAME, CONF_APP_SWF_PATH, strValue );
			YL_StringUtil::Format( strFailedReason, "无法创建文件，请查看下载目录%s是否只读", strValue.c_str() );
		}
		break;
	case HTTP_FAILED_CANT_WRITE_LOCAL_FILE:
		{
			string strValue;
			AfxGetUserConfig()->GetConfigStringValue( CONF_APP_MODULE_NAME, CONF_APP_SWF_PATH, strValue );
			YL_StringUtil::Format( strFailedReason, "无法写入文件，请查看下载目录%s是否只读", strValue.c_str() );
		}
		break;
	case HTTP_FAILED_SOCKET_ERROR:
		strFailedReason = "连接失败";
		break;
	case HTTP_FAILED_TIMEOUT:
		strFailedReason = "连接超时";
		break;
	case HTTP_FAILED_CANCELED:
		strFailedReason = "下载取消";
		break;
	case HTTP_FAILED_NOPEER:
	case HTTP_FAILED_HTTP_NOTUSE:
	case HTTP_FAILED_NOTCONNECT:
		strFailedReason = "链接无效";
		break;
	default:
		strFailedReason = "不明原因";
		break;
	}
	string strFailedText = "下载失败原因：";
	strFailedText += strFailedReason;

	vecText.push_back( strFailedText );
	m_pWndDownPercent->SetText( vecText );
	m_pWndDownPercent->SetDownPercent( 0 );
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::HttpDownOb_DownFailed", "===OUT");
}

void GamePanelWnd::HttpDownOb_DownProgress( string& strID, double dPercent, 
					unsigned int unFileSize, unsigned int unSpeed)
{
	if( strID != m_swfGame.strID )
		return;
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::HttpDownOb_DownProgress", "===IN");
	m_bDown = true;
	UpdateAllWnd();

	vector<string> vecText;
	string strText;
	//文件大小\t下载百分比
	YL_StringUtil::Format( strText, "%s   ", UINT2CString( unFileSize ).GetString() );
	
	string strTmp;
	if( unFileSize == 0 )
		YL_StringUtil::Format( strTmp, "连接中..." );
	else
		YL_StringUtil::Format( strTmp, "%d%%", (int)(dPercent*100) );

	strText += strTmp;
	vecText.push_back( strText );
	//下载速度
	YL_StringUtil::Format( strText, "下载速度:%s/s   再等待:%s", 
		UINT2CString( unSpeed ).GetString(),
		GetLeftTime( unFileSize, unSpeed, (unsigned int)(dPercent*100) ).GetString() );
	vecText.push_back( strText );	
	
	//Show the percentage of download-process.
	m_pWndDownPercent->SetFailed( false );
	m_pWndDownPercent->SetText( vecText );
	m_pWndDownPercent->SetDownPercent( dPercent );
}

void GamePanelWnd::SetGameEntry( SWF_GAME sg )
{
	m_swfGame = sg;
	OneGame olg;
	if( GLOBAL_GAME->IGameData_GetGameByID( m_swfGame.strID, OneGame::FLASH_GAME, olg ) )
	{
		m_bDown = false;
		//If the Movie has been downloaded then play it.
		//m_bDown = true;
		//this->SetTimer(ADS_TIMER_ID,ADS_TIME,NULL);
		PlayMovie( olg.strID, olg.strGamePath );
		UpdateAllWnd();
		std::string strFlashGameRightUrl;
		AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
			CONF_SETTING_CONFIG_FLASH_GAME_RIGHT_URL,strFlashGameRightUrl);
		if (!strFlashGameRightUrl.empty())
			m_pWndRight->Navigate(strFlashGameRightUrl+m_swfGame.strID);
		else
			m_pWndRight->Navigate("about:blank");

		std::string strFlashGameBottomtUrl;
		AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
			CONF_SETTING_CONFIG_FLASH_GAME_BOTTOM_URL,strFlashGameBottomtUrl);
		if (!strFlashGameBottomtUrl.empty())
			m_pWndBottom->Navigate(strFlashGameBottomtUrl+m_swfGame.strID);
		else
			m_pWndBottom->Navigate("about:blank");
	}else
	{
		m_bDown = true;
		//处理swfurl，去掉空格
		YL_StringUtil::ReplaceAll( m_swfGame.strSwfUrl, " ", "%20" );
		//下载swf文件
		HttpDownCtrl::GetInstance()->StartDown( m_swfGame.strID, m_swfGame.strSwfUrl );
		UpdateAllWnd();
	}
}

void GamePanelWnd::PlayMovie( string strID, string strPath )
{
	//播放swf	
	if( m_pGameFlash->GetMovie().Compare( strPath.c_str() ) == 0  )
		m_pGameFlash->SetMovie( GlobalSwfPath::Nothing() );
	m_pGameFlash->SetMovie(strPath.c_str());
	m_pGameFlash->Play();
	m_pGameFlash->SetFocus();
}

CString GamePanelWnd::UINT2CString(UINT ui)
{
	CString str;
	if(0 != (ui/(1<<20)))
	{
		double result = (double)ui/(1<<20);
		str.Format("%.2f MB", result);
	}else
	if( 0 != (ui/(1<<10)) )
		str.Format("%d KB", ui/(1<<10));
	else
		str.Format("%d B", ui);

	return str;
}

CString GamePanelWnd::GetLeftTime( unsigned int uiSize, unsigned int uiSpeed, unsigned int uiFinished )
{
	CString strRet = "--:--:--";

	if( uiSize != 0 && uiSpeed > 0)
	{
		CString str;
		UINT ts = ((UINT)((double)uiSize*(100-uiFinished)/100))/uiSpeed;
		UINT h = ts/3600;
		UINT m = (ts-h*3600)/60;
		UINT s = (ts-h*3600-m*60);

		if(h < 10)
			str.Format("0%d", h);
		else
			str.Format("%d", h);

		strRet = str;

		if(m < 10)
			str.Format(":0%d", m);
		else
			str.Format(":%d", m);

		strRet += str;

		if(s < 10)
			str.Format(":0%d", s);
		else
			str.Format(":%d", s);
		
		strRet += str;
	}
	return strRet;
}

void GamePanelWnd::UpdateAllWnd()
{
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::UpdateAllWnd", "===IN");
	if( m_bDown )//需要显示下载界面
	{
		//显示下载wnd
		m_pWndDownPercent->ShowWindow( SW_SHOW );
		m_pGameFlash->ShowWindow( SW_HIDE );
		m_pBtnRePlay->ShowWindow( SW_HIDE );
		m_pBtnMute->ShowWindow( SW_HIDE );
		m_pBtnUnMute->ShowWindow( SW_HIDE );
		m_pBtnToFullScreen->ShowWindow( SW_HIDE );
		m_pBtnExitFullScreen->ShowWindow( SW_HIDE );
		m_pBtnPause->ShowWindow(SW_HIDE);
		m_pWndRight->ShowWindow(SW_HIDE);
		m_pWndBottom->ShowWindow(SW_HIDE);
	}else
	{
		m_pWndDownPercent->ShowWindow( SW_HIDE );
		//显示重玩
		m_pBtnRePlay->ShowWindow( SW_SHOW );
		m_pBtnRePlay->Invalidate();
		//声音
		if( CSound::GetInstance()->GetMute( 1 ) )
		{
			m_pBtnMute->ShowWindow( SW_HIDE );
			m_pBtnUnMute->ShowWindow( SW_SHOW );
		}else
		{
			m_pBtnMute->ShowWindow( SW_SHOW );
			m_pBtnUnMute->ShowWindow( SW_HIDE );
		}
		//全屏
		if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
		{
			m_pBtnToFullScreen->ShowWindow( SW_HIDE );
			m_pBtnExitFullScreen->ShowWindow( SW_SHOW );
		}else
		{
			m_pBtnToFullScreen->ShowWindow( SW_SHOW );
			m_pBtnExitFullScreen->ShowWindow( SW_HIDE );
		}
		m_pBtnPause->ShowWindow(SW_SHOW);
		m_pWndRight->ShowWindow(SW_SHOW);
		m_pWndBottom->ShowWindow(SW_SHOW);
		m_pGameFlash->ShowWindow( SW_SHOW );
	}
}
void GamePanelWnd::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);	
	m_pWndDownPercent->MoveWindow( 0, 0, cx, cy );
	UpdateAllWnd();	
}

void GamePanelWnd::OnClickedReplay()
{
	SetGameEntry( m_swfGame );
	if(GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen())
	{
		this->m_pWndRight->ShowWindow(SW_HIDE);
		this->m_pWndBottom->ShowWindow(SW_HIDE);
		CRect rect;
		this->GetClientRect(&rect);
		rect.top +=40;
		this->m_pGameFlash->MoveWindow(rect,false);
	}
}

void GamePanelWnd::OnClickedPause()
{
	if(GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen())
	{
		this->m_pWndRight->ShowWindow(SW_HIDE);
		this->m_pWndBottom->ShowWindow(SW_HIDE);
		CRect rect;
		this->GetClientRect(&rect);
		rect.top +=40;
		this->m_pGameFlash->MoveWindow(rect,false);
	}
}

void GamePanelWnd::OnClickedCut()
{
	string strDesktopPath;
	char szTmpPath[MAX_PATH] = {0};
	SHGetSpecialFolderPath (AfxGetMainWindow()->m_hWnd, szTmpPath, CSIDL_DESKTOP, FALSE);
	strDesktopPath = szTmpPath;

	if (!strDesktopPath.length ())
		return;

	time_t now;
	time (&now);

	tm * tm_now = localtime (&now);

	string strNow;
	YL_StringUtil::Format (strNow, "%02d%02d%02d%02d%02d",
		tm_now->tm_mon + 1, tm_now->tm_mday,
		tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);

	string strFileName;
	YL_StringUtil::Format(strFileName, "%s\\%s.jpg",
		strDesktopPath.c_str(), strNow.c_str());

	CRect rcWindow;
	m_pGameFlash->GetClientRect(&rcWindow);

	HDC hdcSrc = m_pGameFlash->GetDC()->GetSafeHdc();
	int nBitPerPixel = GetDeviceCaps(hdcSrc, BITSPIXEL);
	CImage image;
	image.Create (rcWindow.Width (), rcWindow.Height (), nBitPerPixel);
	BitBlt (image.GetDC (), 0, 0, rcWindow.Width (), rcWindow.Height (), 
		hdcSrc, rcWindow.left, rcWindow.top, SRCCOPY);

	::ReleaseDC(NULL, hdcSrc);
	image.ReleaseDC();
	image.Save(strFileName.c_str(), Gdiplus::ImageFormatJPEG);

	CString strTitle = "提示";
	CString strContent = CString(strFileName.c_str());
	
	::SendMessage( AfxGetMainWindow()->m_hWnd, MSG_SHOW_TRAYICONBALLOON, WPARAM(&strTitle), LPARAM(&strContent) );
}

void GamePanelWnd::ILayoutChangeOb_InitFinished()
{
	UpdateAllWnd();
}
void GamePanelWnd::ILayoutChangeOb_SkinChanged(string oldSkinPath, string newSkinPath)
{
	UpdateAllWnd();
}

void GamePanelWnd::ILayoutChangeOb_UpdateLayout(HWND hWnd)
{
	if( hWnd == GetSafeHwnd() )
	{
		UpdateAllWnd();
	}
}

void GamePanelWnd::OnClickedMute()
{
	bool bIsMute = CSound::GetInstance()->GetMute( 1 );
	CSound::GetInstance()->SetMute( 1, !bIsMute );
	UpdateAllWnd();
	if(GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen())
	{
		this->m_pWndRight->ShowWindow(SW_HIDE);
		this->m_pWndBottom->ShowWindow(SW_HIDE);
		CRect rect;
		this->GetClientRect(&rect);
		rect.top +=40;
		this->m_pGameFlash->MoveWindow(rect,false);
	}
}

void GamePanelWnd::OnClickedUnMute()
{
	bool bIsMute = CSound::GetInstance()->GetMute( 1 );
	CSound::GetInstance()->SetMute( 1, !bIsMute );
	UpdateAllWnd();
	if(GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen())
	{
		this->m_pWndRight->ShowWindow(SW_HIDE);
		this->m_pWndBottom->ShowWindow(SW_HIDE);
		CRect rect;
		this->GetClientRect(&rect);
		rect.top +=40;
		this->m_pGameFlash->MoveWindow(rect,false);
	}
}

void GamePanelWnd::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent == ADS_TIMER_ID )
	{
		++this->m_iStep ;
		this->m_pWndDownPercent ->SetDownPercent  (m_iStep/(double)60);
		vector<string> vecText;
		switch(this->m_iStep %3)
		{
		case 0:
			vecText.push_back ("正在缓冲...");
			break;
		case 1:
			vecText.push_back ("正在缓冲.  ");
			break;
		case 2:
			vecText.push_back ("正在缓冲.. ");
			break;
		default:
			vecText.push_back ("正在缓冲...");
			break;
		}
		this->m_pWndDownPercent ->SetText (vecText);
		if(this->m_iStep == 60)
		{
			KillTimer (ADS_TIMER_ID );
			m_bDown = false;
			UpdateAllWnd();
		}
	}

	__super::OnTimer(nIDEvent);
}

void GamePanelWnd::OnClickedFullScreen()
{
	if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
		GLOBAL_PANELCHANGEDATA->IPanelChange_ExitFullScreen();
	else
	{
		GLOBAL_PANELCHANGEDATA->IPanelChange_ToFullScreen( this );
		this->m_pWndRight->ShowWindow(SW_HIDE);
		this->m_pWndBottom->ShowWindow(SW_HIDE);
		CRect rect;
		this->GetClientRect(&rect);
		rect.top +=40;
		this->m_pGameFlash->MoveWindow(rect,false);
	}
}

void GamePanelWnd::OnClickedExitFullScreen()
{
	if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
		GLOBAL_PANELCHANGEDATA->IPanelChange_ExitFullScreen();
	else
	{
		GLOBAL_PANELCHANGEDATA->IPanelChange_ToFullScreen( this );
		this->m_pWndRight->ShowWindow(SW_SHOW);
		this->m_pWndBottom->ShowWindow(SW_SHOW);
	}
}

void GamePanelWnd::ShowHideEseFull(bool isShow)
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

	}else if(!isShow && curValue <= 1)
		m_pEscFullTipDlg->ShowDlg( FALSE );
}

BOOL GamePanelWnd::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			OnClickedExitFullScreen();
			return TRUE;
		}
	}
	return __super::PreTranslateMessage(pMsg);
}
