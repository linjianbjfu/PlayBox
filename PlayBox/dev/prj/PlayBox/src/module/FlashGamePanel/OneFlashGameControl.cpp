#include "stdafx.h"
#include "OneFlashGameControl.h"
#include "..\WebInteract\WebManager.h"
#include "CDataManager.h"
#include "YL_HTTPDownFile.h"
#include "..\httpDown\HttpDownCtrl.h"
#include "AppConfig\config\ConfigSettingDef.h"
#include "..\WebInteract\MyWebBrowserWnd.h"
#include "FlashGamePlay.h"
#include "FlashDownloadWnd.h"
#include "..\Esc2ExitFullScrPanel\ESCFullDlg.h"
#include "shockwaveflash.h"
#include "FlashGamePanelWnd.h"
#include "AppConfig\config\ConfigLayoutDef.h"
#include "util\Sound.h"
#include "IMuteMsgObserver.h"
#include "FlashGameStagePanelWnd.h"

COneFlashGameControl::COneFlashGameControl()
: m_pBrowserIntro(NULL), m_pBrowserRecommand(NULL),
m_pPlay(NULL), m_pFlashCore(NULL),m_pDownload(NULL),
m_pFlashGamePanel(NULL), m_bDown(false), m_isMainWindowTopMost(false),
m_bFullScreen(false), m_pEscFullTipDlg(NULL), m_pWndParent(NULL)
{
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*) this);
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_HTTP_DOWN,(IHttpDownObserver*) this);
}

COneFlashGameControl::~COneFlashGameControl()
{
	delete m_pEscFullTipDlg;
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*) this);
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_HTTP_DOWN,(IHttpDownObserver*) this);
}

void COneFlashGameControl::SetBrowserIntro(MyWebBrowserWnd* pWnd)
{
	m_pBrowserIntro = pWnd;
}

void COneFlashGameControl::SetBrowserRecommand(MyWebBrowserWnd* pWnd)
{
	m_pBrowserRecommand = pWnd;
}

void COneFlashGameControl::SetFlashPlay(CFlashGamePlay* pWnd)
{
	m_pPlay = pWnd;
}

void COneFlashGameControl::SetFlashDownload(CFlashGameDownloadWnd* pWnd)
{
	m_pDownload = pWnd;
}

void COneFlashGameControl::SetFlashCore(CShockwaveFlash* pWnd)
{
	m_pFlashCore = pWnd;
}

void COneFlashGameControl::SetFlashGamePanel(CFlashGamePanelWnd* pWnd)
{
	m_pFlashGamePanel = pWnd;
}

void COneFlashGameControl::SetStage(CFlashGameStagePanelWnd* pWnd)
{
	m_pStage = pWnd;
}

void COneFlashGameControl::SetStageLeft(CFlashGameStageLeft* pWnd)
{
	m_pStageLeft = pWnd;
}

void COneFlashGameControl::SetTabItem(TAB_ITEM ti)
{
	m_tabItem = ti;
	std::string strID = CWebManager::GetInstance()->GetValue( ti.strParam, "id" );
	std::string strName = CWebManager::GetInstance()->GetValue( ti.strParam, "name" );
	std::string strPicSvrUrl = CWebManager::GetInstance()->GetValue( ti.strParam, "picurl" );
	std::string strSwfSvrUrl = CWebManager::GetInstance()->GetValue( ti.strParam, "swfurl" );

	NavigateIntro(strID);
	NavigateRecommand(strID);

	m_olg.Clear();
	m_olg.strID = strID;
	m_olg.strName = strName;
	m_olg.strGameSvrPath = strSwfSvrUrl;
	m_olg.strPicSvrPath = strPicSvrUrl;
	m_olg.nGameType = OneGame::FLASH_GAME | OneGame::RECENT_PLAY;
	GLOBAL_GAME->IGameData_AddGame(m_olg);

	std::string strLocalPicPath;
	if (m_olg.GetLocalPicPath(strLocalPicPath) && 
		!YL_FileInfo::IsValid(strLocalPicPath) )
	{
		YL_CHTTPDownFile httpDownFile;
		httpDownFile.DownloadFile( m_olg.strPicSvrPath, strLocalPicPath );
	}

	std::string strLocalSwfPath;
	if (m_olg.GetLocalSwfPath(strLocalSwfPath))
	{
		m_bDown = !YL_FileInfo::IsValid(strLocalSwfPath);
		if (m_bDown)
			HttpDownCtrl::GetInstance()->StartDown( m_olg.strID, m_olg.strGameSvrPath );
		else
			PlayMovie(strLocalSwfPath);
		
		UpdateAllWnd();
	}
}

void COneFlashGameControl::RePlay()
{
	std::string strLocalSwfPath;
	if (m_olg.GetLocalSwfPath(strLocalSwfPath) &&
		YL_FileInfo::IsValid(strLocalSwfPath))
		PlayMovie(strLocalSwfPath);
}

void COneFlashGameControl::ExitFullScreen()
{
	if( !GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
		return;

	GLOBAL_PANELCHANGEDATA->IPanelChange_ExitFullScreen();
}

void COneFlashGameControl::ToFullScreen()
{
	if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
		return;

	GLOBAL_PANELCHANGEDATA->IPanelChange_ToFullScreen(m_pPlay);
}

void COneFlashGameControl::NavigateIntro(std::string strID)
{
	std::string strFlashGameRightUrl;
	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_CONFIG_FLASH_GAME_RIGHT_URL,strFlashGameRightUrl);
	if (!strFlashGameRightUrl.empty())
		m_pBrowserIntro->Navigate(strFlashGameRightUrl+strID);
}

void COneFlashGameControl::NavigateRecommand(std::string strID)
{
	std::string strFlashGameBottomtUrl;
	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_CONFIG_FLASH_GAME_BOTTOM_URL,strFlashGameBottomtUrl);
	if (!strFlashGameBottomtUrl.empty())
		m_pBrowserRecommand->Navigate(strFlashGameBottomtUrl+strID);
}

void COneFlashGameControl::UpdateAllWnd()
{
	m_pPlay->ShowWindow(m_bDown ? FALSE : TRUE);
	m_pDownload->ShowWindow(m_bDown ? TRUE : FALSE);
}

void COneFlashGameControl::PlayMovie(string strPath)
{
	//m_pFlashCore->SetMovie(GlobalSwfPath::BlankSwfPath().c_str());
	//m_pFlashCore->Play();
	//m_pFlashCore->StopPlay();
	m_pFlashCore->SetMovie(strPath.c_str());
	m_pFlashCore->Play();
	m_pFlashCore->SetFocus();
}

void COneFlashGameControl::IPanelChangeOb_ToFullScreen( CWnd* pWnd )
{
	if( pWnd != m_pPlay )
		return;

	m_isMainWindowTopMost = false;
	bool curState;
	AfxGetUserConfig()->GetConfigBoolValue(CONF_APP_MODULE_NAME,
		CONF_APP_MAINWND_HOLD,curState);
	if(curState)
	{
		SetMainWindow(false);
		m_isMainWindowTopMost=true;
	}
	m_pPlay->GetClientRect(&m_rectBeforeFull);
	m_pWndParent = m_pPlay->GetParent();
	//去掉WS_CHILD,这样CWnd才能全屏时获得焦点
	long style = ::GetWindowLong(m_pPlay->GetSafeHwnd(),GWL_STYLE);
	style &= ~WS_CHILD;
	::SetWindowLong(m_pPlay->GetSafeHwnd(), GWL_STYLE, style);

	long lExStyle = ::GetWindowLong(m_pPlay->GetSafeHwnd(), GWL_EXSTYLE);
	lExStyle &= ~WS_EX_APPWINDOW; 
	lExStyle |= WS_EX_TOOLWINDOW; 
	::SetWindowLong(m_pPlay->GetSafeHwnd(), GWL_EXSTYLE, lExStyle);

	m_pPlay->SetParent(m_pPlay->GetDesktopWindow());
	m_pPlay->GetDesktopWindow()->GetWindowRect(&m_rectFullScreen);

	m_pPlay->SetWindowPos(&CWnd::wndTopMost,m_rectFullScreen.left,
		m_rectFullScreen.top, m_rectFullScreen.Width(),
		m_rectFullScreen.Height(),SWP_SHOWWINDOW);
	m_bFullScreen = true;
	m_pPlay->ShowButtons(false);

	m_pPlay->SetFocus();
	ShowHideEseFull(true);
}

void COneFlashGameControl::IPanelChangeOb_ExitFullScreen( CWnd* pWnd )
{
	if( pWnd != m_pPlay )
		return;

	m_bFullScreen = false;
	//加上子窗口风格
	long style = ::GetWindowLong(m_pPlay->GetSafeHwnd(),GWL_STYLE);
	style |= WS_CHILD;
	//style |= WS_CLIPCHILDREN; //去掉WS_CLIPCHILDREN后，GamePanelWnd刷新时就会有剧烈闪烁
	::SetWindowLong(m_pPlay->GetSafeHwnd(), GWL_STYLE, style);

	long lExStyle = ::GetWindowLong(m_pPlay->GetSafeHwnd(), GWL_EXSTYLE);
	lExStyle |= WS_EX_APPWINDOW; 
	lExStyle &= ~WS_EX_TOOLWINDOW; 
	::SetWindowLong(m_pPlay->GetSafeHwnd(), GWL_EXSTYLE, lExStyle);

	m_pPlay->SetParent(m_pWndParent);
	m_pPlay->MoveWindow(&m_rectBeforeFull);

	if(m_isMainWindowTopMost)
	{
		SetMainWindow(true);
		m_isMainWindowTopMost = false;
	}

	bool bHold = false;
	AfxGetUserConfig()->GetConfigBoolValue( CONF_APP_MODULE_NAME, 
		CONF_APP_MAINWND_HOLD,bHold);
	if(bHold)
		AfxGetMainWindow()->SetWindowPos(&CWnd::wndTopMost,
		-1,-1,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	UpdateAllWnd();
	m_pPlay->ShowButtons(true);
	//游戏界面获取焦点
	m_pPlay->SetFocus();
	ShowHideEseFull(false);
}

void COneFlashGameControl::SetMainWindow(bool isTopMost)
{
	AfxGetMainWindow()->SetWindowPos(isTopMost ? &CWnd::wndTopMost : &CWnd::wndNoTopMost,
		-1,-1,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	AfxGetUserConfig()->SetConfigBoolValue( CONF_APP_MODULE_NAME,CONF_APP_MAINWND_HOLD,isTopMost);
}

void COneFlashGameControl::HttpDownOb_DownStart( string& strID )
{
	if( strID != m_olg.strID )
		return;

	m_bDown = true;
	UpdateAllWnd();
	m_pDownload->SetFailed( false );
	vector<string> vecText;
	vecText.push_back( "开始下载" );
	//m_pDownload->SetText( vecText );
	m_pDownload->SetDownPercent( 0 );
}

void COneFlashGameControl::HttpDownOb_DownFinish( string& strID, string& strSwfPath )
{
	if(strID != m_olg.strID)
		return;

	//进度窗口参数
	m_pDownload->SetFailed( false );
	vector<string> vecText;
	//Set the download percentage.
	vecText.push_back( "下载完成,启动中..." );
	m_pDownload->SetText( vecText );
	m_pDownload->SetDownPercent( 1.0 );

	std::string strLocalPicPath;
	if (m_olg.GetLocalPicPath(strLocalPicPath) && 
		!YL_FileInfo::IsValid(strLocalPicPath) )
	{
		YL_CHTTPDownFile httpDownFile;
		httpDownFile.DownloadFile( m_olg.strPicSvrPath, strLocalPicPath );
	}
	PlayMovie(strSwfPath);
	m_bDown = false;
	UpdateAllWnd();
}

void COneFlashGameControl::HttpDownOb_DownFailed( string& strID, HTTP_DOWN_FAILED_REASON r )
{
	if( strID != m_olg.strID )
		return;

	m_bDown = true;
	UpdateAllWnd();
	m_pDownload->SetFailed( true );
	vector<string> vecText;
	string strFailedReason = "未知原因";
	switch( r )
	{
	case HTTP_FAILED_CANT_CREATE_LOCAL_FILE:
		{
			string strValue;
			GlobalSwfPath::GetConfigSwfPath(strValue);
			YL_StringUtil::Format( strFailedReason, "无法创建文件，请查看下载目录%s是否只读", strValue.c_str() );
		}
		break;
	case HTTP_FAILED_CANT_WRITE_LOCAL_FILE:
		{
			string strValue;
			GlobalSwfPath::GetConfigSwfPath(strValue);
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
	m_pDownload->SetText( vecText );
	m_pDownload->SetDownPercent( 0 );
}

void COneFlashGameControl::HttpDownOb_DownProgress( string& strID, double dPercent, 
										   unsigned int unFileSize, unsigned int unSpeed)
{
	if(strID != m_olg.strID)
		return;

	m_bDown = true;
	UpdateAllWnd();
	vector<string> vecText;
	string strText;
	//文件大小\t下载百分比
	//YL_StringUtil::Format( strText, "%s   ", UINT2CString( unFileSize ).GetString() );
	string strTmp;
	if( unFileSize == 0 )
		YL_StringUtil::Format( strTmp, "连接中..." );
	else
	{
		//YL_StringUtil::Format( strTmp, "%d%%", (int)(dPercent*100) );
		YL_StringUtil::Format(strTmp,"游戏缓冲中... ");
	}

	strText = strTmp;
	//vecText.push_back( strText );
	YL_StringUtil::Format( strTmp, "下载速度:%s/s ", 
		UINT2CString( unSpeed ).GetString() );
	strText +=strTmp;
	vecText.push_back( strText );	
	//Show the percentage of download-process.
	m_pDownload->SetFailed( false );
	m_pDownload->SetText( vecText );
	m_pDownload->SetDownPercent( dPercent );
}

CString COneFlashGameControl::UINT2CString(UINT ui)
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

CString COneFlashGameControl::GetLeftTime( unsigned int uiSize, unsigned int uiSpeed, unsigned int uiFinished )
{
	CString strRet = "--:--:--";
	if( uiSize != 0 && uiSpeed > 0)
	{
		CString str;
		UINT ts = ((UINT)((double)uiSize*(100-uiFinished)/100))/uiSpeed;
		UINT h = ts/3600;
		UINT m = (ts-h*3600)/60;
		UINT s = (ts-h*3600-m*60);
		str.Format(h < 10 ? "0%d" : "%d", h);
		strRet = str;
		str.Format(m < 10 ? ":0%d" : ":%d", m);		
		strRet += str;
		str.Format(s < 10 ? ":0%d" : ":%d", s);
		strRet += str;
	}
	return strRet;
}

void COneFlashGameControl::ShowHideEseFull(bool isShow)
{
	if (!m_pEscFullTipDlg)
	{
		m_pEscFullTipDlg = new ESCFullDlg();
		m_pEscFullTipDlg->Create(IDD_DIALOG_ESCFULLSCR, m_pPlay);
	}
	int allValue=0;
	static int curValue=0;
	AfxGetUserConfig()->GetConfigIntValue(CONF_LAYOUT_MODULE_NAME, 
		CONF_LAYOUT_ESC_ALL, allValue);
	if( isShow && curValue < 1 && allValue < 5)
	{
		curValue++;
		m_pEscFullTipDlg->ShowDlg( TRUE );
		allValue=allValue+1;
		allValue = 0;
		AfxGetUserConfig()->SetConfigIntValue(CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_ESC_ALL,allValue,true);

	}else if(!isShow && curValue <= 1)
		m_pEscFullTipDlg->ShowDlg( FALSE );
}

void COneFlashGameControl::SetMute(bool mute)
{
	bool bIsMute = CSound::GetInstance()->GetMute( 1 );
	if (mute != bIsMute)
	{
		CSound::GetInstance()->SetMute( 1, mute );
	}

	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers(ID_MESSAGE_MUTE,listOb);
	for( list<IMessageObserver*>::iterator itOb = listOb.begin();
		itOb != listOb.end();itOb++ )
	{
		IMuteMsgObserver* pOb = dynamic_cast<IMuteMsgObserver*>(*itOb);
		pOb->MuteMsg_Change(mute);
	} 
}

static char* introName = "flashgameintro";
bool COneFlashGameControl::IsIntroShow()
{
	CRect rc;
	AfxGetUIManager()->UIGetLayoutMgr()->GetPaneItemRect(m_pStage, introName, rc);
	return (rc.Width() != 0);
}

void COneFlashGameControl::ShowIntro(bool bShow)
{
	CRect rc;
	ILayoutMgr* pLayoutMgr = AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->GetPaneItemRect(m_pStage, introName, rc);

	if (bShow)
		rc.left = rc.right - 260;	
	else
		rc.left = rc.right;
	pLayoutMgr->SetPaneSize((CWnd*)m_pStage, introName, rc);

	AfxGetUIManager()->UIGetLayoutMgr()->UpdateLayout(m_pStage->GetSafeHwnd());
}

static char* recommandName = "flashgamerecommand";
bool COneFlashGameControl::IsRecommandShow()
{
	CRect rc;
	AfxGetUIManager()->UIGetLayoutMgr()->GetPaneItemRect((CWnd*)m_pStageLeft, recommandName, rc);
	return (rc.Height() != 0);
}

void COneFlashGameControl::ShowRecommand(bool bShow)
{
	CRect rc;
	ILayoutMgr* pLayoutMgr = AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->GetPaneItemRect((CWnd*)m_pStageLeft, recommandName, rc);

	if (bShow)
		rc.top = rc.bottom - 104;
	else
		rc.top = rc.bottom;
	CWnd* pWnd = (CWnd*)m_pStageLeft;
	pLayoutMgr->SetPaneSize(pWnd, recommandName, rc);
	AfxGetUIManager()->UIGetLayoutMgr()->UpdateLayout(pWnd->GetSafeHwnd());
}

bool COneFlashGameControl::IsDownloading()
{
	return m_bDown;
}