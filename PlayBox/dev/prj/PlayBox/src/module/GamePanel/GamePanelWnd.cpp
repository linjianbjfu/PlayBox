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
	m_bFullScreen         = false;
	m_bDown		          = false;
	m_isMainWindowTopMost = false;
	m_bFullScreen		  = false;
	m_iStep = 0;
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
	ON_BN_CLICKED(IDC_BTN_REPLAY,OnClickedReplay)
	ON_BN_CLICKED(IDC_BTN_FULL_SCREEN,OnClickedFullScreen)
	ON_BN_CLICKED(IDC_BTN_EXIT_FULL_SCREEN,OnClickedExitFullScreen)
	ON_BN_CLICKED(IDC_BTN_MUTE,OnClickedMute)
	ON_BN_CLICKED(IDC_BTN_UN_MUTE,OnClickedUnMute)
	ON_BN_CLICKED(IDC_BTN_PAUSE,OnClickedPause)
END_MESSAGE_MAP()

void GamePanelWnd::InitFlashParams(CShockwaveFlash*	pGameFlash)
{
	pGameFlash->SetBackgroundColor( -1 );//�ޱ���ɫ
	pGameFlash->SetAlignMode( 2 );       //
	pGameFlash->SetMenu( FALSE );        //����չ�ĵ����˵� 
	pGameFlash->SetScale( "ExactFit" );  //����ģʽΪ����ʾ������������
	pGameFlash->SetWMode( "Window" );    //���ô���ģʽ
	pGameFlash->SetQuality( 1 );         //���ò��Ŷ�������Ϊ�� 
	pGameFlash->SetEmbedMovie(TRUE);     //��������Ƕ�� 
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
	pLayoutMgr->RegisterCtrl( this, "DownPercent", m_pWndDownPercent );
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

void GamePanelWnd::SetTabItem( TAB_ITEM ti )
{
	m_tabItem = ti;
	std::string strID = CWebManager::GetInstance()->GetValue( ti.strParam, "id" );
	std::string strName = CWebManager::GetInstance()->GetValue( ti.strParam, "name" );
	std::string strPicSvrUrl = CWebManager::GetInstance()->GetValue( ti.strParam, "picurl" );
	std::string strSwfSvrUrl = CWebManager::GetInstance()->GetValue( ti.strParam, "swfurl" );
	
	std::string strFlashGameRightUrl;
	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_CONFIG_FLASH_GAME_RIGHT_URL,strFlashGameRightUrl);
	if (!strFlashGameRightUrl.empty())
		m_pWndRight->Navigate(strFlashGameRightUrl+strID);

	std::string strFlashGameBottomtUrl;
	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_CONFIG_FLASH_GAME_BOTTOM_URL,strFlashGameBottomtUrl);
	if (!strFlashGameBottomtUrl.empty())
		m_pWndBottom->Navigate(strFlashGameBottomtUrl+strID);

	m_olg.Clear();
	if(!GLOBAL_GAME->IGameData_GetGameByID(strID, OneGame::FLASH_GAME, m_olg))
	{
		m_olg.strID = strID;
		m_olg.strName = strName;
		m_olg.strGameSvrPath = strSwfSvrUrl;
		m_olg.strPicSvrPath = strPicSvrUrl;
		m_olg.nGameType = OneGame::FLASH_GAME | OneGame::RECENT_PLAY;
		GLOBAL_GAME->IGameData_AddGame(m_olg);
	}

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
		if (YL_FileInfo::IsValid(strLocalSwfPath))
		{
			m_bDown = false;
			PlayMovie(strLocalSwfPath);
		}else
		{
			m_bDown = true;
			HttpDownCtrl::GetInstance()->StartDown( m_olg.strID, m_olg.strGameSvrPath );
		}
		UpdateAllWnd();
	}
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
	//ɾ������swf��htm�ļ�
	if( !m_strCopyedHtmlPath.empty() )
		::DeleteFile( m_strCopyedHtmlPath.c_str() );

	__super::OnDestroy();
}

void GamePanelWnd::IPanelChangeOb_ToFullScreen( CWnd* pWnd )
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

	//ȥ���Ӵ��ڷ��,����GamePanelWnd����ȫ��ʱ��ý���
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

	//��Ϸ�����ȡ����
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
	//�����Ӵ��ڷ��
	long style = ::GetWindowLong(m_hWnd,GWL_STYLE);
	style |= WS_CHILD;
	//style |= WS_CLIPCHILDREN; //ȥ��WS_CLIPCHILDREN��GamePanelWndˢ��ʱ�ͻ��о�����˸
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
		AfxGetMainWindow()->SetWindowPos(&wndTopMost,-1,-1,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	UpdateAllWnd();
	//��Ϸ�����ȡ����
	// m_pGameFlash->SetFocus();
	KillTimer(ID_TIMER_ESCFULL_TIP);
	ShowHideEseFull(false);
}

void GamePanelWnd::HttpDownOb_DownStart( string& strID )
{
	if( strID != m_olg.strID )
		return;
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::HttpDownOb_DownStart", "===IN");
	m_bDown = true;
	UpdateAllWnd();

	m_pWndDownPercent->SetFailed( false );
	vector<string> vecText;
	vecText.push_back( "��ʼ����" );
	//m_pWndDownPercent->SetText( vecText );
	m_pWndDownPercent->SetDownPercent( 0 );
}

void GamePanelWnd::HttpDownOb_DownFinish( string& strID, string& strSwfPath )
{
	if(strID != m_olg.strID)
		return;

	//���ȴ��ڲ���
	m_pWndDownPercent->SetFailed( false );
	vector<string> vecText;
	//Set the download percentage.
	vecText.push_back( "�������,������..." );
	m_pWndDownPercent->SetText( vecText );
	m_pWndDownPercent->SetDownPercent( 1.0 );

	OneGame olg;
	GLOBAL_GAME->IGameData_GetGameByID( strID, OneGame::FLASH_GAME, olg );
	std::string strLocalPicPath;
	if (olg.GetLocalPicPath(strLocalPicPath) && 
		!YL_FileInfo::IsValid(strLocalPicPath) )
	{
		YL_CHTTPDownFile httpDownFile;
		httpDownFile.DownloadFile( olg.strPicSvrPath, strLocalPicPath );
	}
	PlayMovie(strSwfPath);
	m_bDown = false;
	UpdateAllWnd();
}

void GamePanelWnd::HttpDownOb_DownFailed( string& strID, HTTP_DOWN_FAILED_REASON r )
{
	if( strID != m_olg.strID )
		return;

	m_bDown = true;
	UpdateAllWnd();
	m_pWndDownPercent->SetFailed( true );
	vector<string> vecText;
	string strFailedReason = "����ԭ��";
	switch( r )
	{
	case HTTP_FAILED_CANT_CREATE_LOCAL_FILE:
		{
			string strValue;
			GlobalSwfPath::GetConfigSwfPath(strValue);
			YL_StringUtil::Format( strFailedReason, "�޷������ļ�����鿴����Ŀ¼%s�Ƿ�ֻ��", strValue.c_str() );
		}
		break;
	case HTTP_FAILED_CANT_WRITE_LOCAL_FILE:
		{
			string strValue;
			GlobalSwfPath::GetConfigSwfPath(strValue);
			YL_StringUtil::Format( strFailedReason, "�޷�д���ļ�����鿴����Ŀ¼%s�Ƿ�ֻ��", strValue.c_str() );
		}
		break;
	case HTTP_FAILED_SOCKET_ERROR:
		strFailedReason = "����ʧ��";
		break;
	case HTTP_FAILED_TIMEOUT:
		strFailedReason = "���ӳ�ʱ";
		break;
	case HTTP_FAILED_CANCELED:
		strFailedReason = "����ȡ��";
		break;
	case HTTP_FAILED_NOPEER:
	case HTTP_FAILED_HTTP_NOTUSE:
	case HTTP_FAILED_NOTCONNECT:
		strFailedReason = "������Ч";
		break;
	default:
		strFailedReason = "����ԭ��";
		break;
	}
	string strFailedText = "����ʧ��ԭ��";
	strFailedText += strFailedReason;

	vecText.push_back( strFailedText );
	m_pWndDownPercent->SetText( vecText );
	m_pWndDownPercent->SetDownPercent( 0 );
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::HttpDownOb_DownFailed", "===OUT");
}

void GamePanelWnd::HttpDownOb_DownProgress( string& strID, double dPercent, 
					unsigned int unFileSize, unsigned int unSpeed)
{
	if(strID != m_olg.strID)
		return;

	m_bDown = true;
	UpdateAllWnd();
	vector<string> vecText;
	string strText;
	//�ļ���С\t���ذٷֱ�
	//YL_StringUtil::Format( strText, "%s   ", UINT2CString( unFileSize ).GetString() );
	string strTmp;
	if( unFileSize == 0 )
		YL_StringUtil::Format( strTmp, "������..." );
	else
	{
		//YL_StringUtil::Format( strTmp, "%d%%", (int)(dPercent*100) );
		YL_StringUtil::Format(strTmp,"��Ϸ������... ");
	}

	strText = strTmp;
	//vecText.push_back( strText );
	//�����ٶ�
	YL_StringUtil::Format( strTmp, "�����ٶ�:%s/s ", 
		UINT2CString( unSpeed ).GetString() );
	strText +=strTmp;
	vecText.push_back( strText );	
	//Show the percentage of download-process.
	m_pWndDownPercent->SetFailed( false );
	m_pWndDownPercent->SetText( vecText );
	m_pWndDownPercent->SetDownPercent( dPercent );
}

void GamePanelWnd::PlayMovie(string strPath )
{
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
	if( m_bDown )//��Ҫ��ʾ���ؽ���
	{
		//��ʾ����wnd
		m_pWndDownPercent->ShowWindow( SW_SHOW );
		m_pGameFlash->ShowWindow( SW_HIDE );
		m_pBtnRePlay->ShowWindow( SW_SHOW );
		//����
		if( CSound::GetInstance()->GetMute( 1 ) )
		{
			m_pBtnMute->ShowWindow( SW_HIDE );
			m_pBtnUnMute->ShowWindow( SW_SHOW );
		}else
		{
			m_pBtnMute->ShowWindow( SW_SHOW );
			m_pBtnUnMute->ShowWindow( SW_HIDE );
		}
		//ȫ��
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
	}else
	{
		m_pWndDownPercent->ShowWindow( SW_HIDE );
		//��ʾ����
		m_pBtnRePlay->ShowWindow( SW_SHOW );
		m_pBtnRePlay->Invalidate();
		//����
		if( CSound::GetInstance()->GetMute( 1 ) )
		{
			m_pBtnMute->ShowWindow( SW_HIDE );
			m_pBtnUnMute->ShowWindow( SW_SHOW );
		}else
		{
			m_pBtnMute->ShowWindow( SW_SHOW );
			m_pBtnUnMute->ShowWindow( SW_HIDE );
		}
		//ȫ��
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

void GamePanelWnd::OnClickedReplay()
{
	std::string strLocalSwfPath;
	m_olg.GetLocalSwfPath(strLocalSwfPath);
	PlayMovie(strLocalSwfPath);
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

	CString strTitle = "��ʾ";
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
	//ȫ��ʱ��disable������ťEQ��Desklyric
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
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		OnClickedExitFullScreen();
		return TRUE;
	}
	return __super::PreTranslateMessage(pMsg);
}