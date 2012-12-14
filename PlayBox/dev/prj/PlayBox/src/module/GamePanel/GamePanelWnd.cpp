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
#include "../../DataInterface/ILocalGameData.h"
#include "../../core/CDataManager.h"
#include "../../Global/GlobalSwfPath.h"
#include "../../gui/CommonControl/xSkinButton.h"
#include "../../gui/CommonControl/xStaticText.h"
#include "../../module/httpDown/HttpDownCtrl.h"
#include "../../Gui/CommonControl/ShockwaveFlash.h"
#include "DownPercentWnd.h"
#include "GameCtrlWnd.h"


IMPLEMENT_DYNAMIC(GamePanelWnd, CBasicWnd)
GamePanelWnd::GamePanelWnd()
{
	m_isMainWindowTopMost = false;
	m_bFullScreen           = false;
	m_bDown		            = false;
	m_isMainWindowTopMost	= false;
	m_bFullScreen			= false;

	m_pGameFlash      = new CShockwaveFlash();
	m_pWndDownPercent = new DownPercentWnd();
	m_pBtnSwitch      = new CxSkinButton();
	m_pGameCtrl       = new CGameCtrlWnd();
	m_pGameInfo       = new CxStaticText();
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_LAYOUTMGR,(ILayoutChangeObserver*) this);
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*) this);
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_HTTP_DOWN,(IHttpDownObserver*) this);
}

GamePanelWnd::~GamePanelWnd()
{
	delete m_pGameFlash;
	delete m_pWndDownPercent;
	delete m_pBtnSwitch;
	delete m_pGameInfo;
	delete m_pGameCtrl;
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_LAYOUTMGR,(ILayoutChangeObserver*) this);
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*) this);
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_HTTP_DOWN,(IHttpDownObserver*) this);
}

BEGIN_MESSAGE_MAP(GamePanelWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_SWITCH,OnClickedSwitch)
	ON_MESSAGE(WM_REPLAY,OnClickedReplay)
	ON_MESSAGE(WM_CUT_SCREEN,OnClickedCut)
END_MESSAGE_MAP()


int GamePanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	CRect rectNull(0,0,0,0);

	m_pGameCtrl->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_WND_GAME_CTRL );
	m_pGameFlash->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN|WS_VISIBLE, rectNull, this, ID_WND_GAME_SWF);

	m_pGameFlash->SetBackgroundColor( -1 );//�ޱ���ɫ
	m_pGameFlash->SetAlignMode( 2 );       //
	m_pGameFlash->SetMenu( FALSE );        //����չ�ĵ����˵� 
	m_pGameFlash->SetScale( "ExactFit" );  //����ģʽΪ����ʾ������������
	m_pGameFlash->SetWMode( "Window" );    //���ô���ģʽ
	m_pGameFlash->SetQuality( 1 );         //���ò��Ŷ�������Ϊ�� 
	m_pGameFlash->SetEmbedMovie(TRUE);     //��������Ƕ�� 

	m_pWndDownPercent->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,rectNull,this,ID_WND_GAME_DOWN_PERCENT );
	m_pBtnSwitch->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_SWITCH);
	m_pGameInfo->Create(NULL,WS_VISIBLE, rectNull,this,ID_WND_GAME_INFO);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();	
	pLayoutMgr->RegisterCtrl( this, "GameCtrl", m_pGameCtrl );
	pLayoutMgr->RegisterCtrl( this, "GameFlash", m_pGameFlash );
	pLayoutMgr->RegisterCtrl( this, "GameCISwitch", m_pBtnSwitch );
	pLayoutMgr->RegisterCtrl( this, "GameInfo", m_pGameInfo );
		
	pLayoutMgr->CreateControlPane( this, "gamepanel", "normal" );
	pLayoutMgr->CreateBmpPane( this,"gamepanel","normal" );

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
	m_swfGame.strIntro  = CWebManager::GetInstance()->GetValue( ti.strParam, "intro" );;
	
	SetGameEntry( m_swfGame );
	
	string strGameInfo = "x ָ����СCRect������ұߵĵ�λ��";
	strGameInfo += "y ָ����СCRect���ϡ��±ߵĵ�λ��";
	strGameInfo += "size ָ����СCRect�ĵ�λ����SIZE��CSize��";
	strGameInfo += "cx ֵָ����С���ұߵĵ�λ����";
	strGameInfo += "cy ָ����С�ϡ��±ߵĵ�λ����";
	strGameInfo += "lpRect ָ��һ��RECT�ṹ��CRect��ָ����Сÿһ�ߵĵ�λ����";
	strGameInfo += "l ָ����СCRect��ߵĵ�λ����";
	strGameInfo += "t ָ����СCRect�ϱߵĵ�λ����";
	m_swfGame.strIntro = strGameInfo;
	m_pGameInfo->SetWindowText( m_swfGame.strIntro.c_str() );
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
	m_pGameCtrl->DestroyWindow();
	//ɾ������swf��htm�ļ�
	if( m_strCopyedHtmlPath.length() != 0 )
	{
		::DeleteFile( m_strCopyedHtmlPath.c_str() );
	}
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
	m_pGameFlash->SetFocus();

	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::IPanelChangeOb_ToFullScreen", "===OUT");
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
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::SetMainWindow", "===OUT");
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
	{
		AfxGetMainWindow()->SetWindowPos(&wndTopMost,-1,-1,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	}
	UpdateAllWnd();
	//��Ϸ�����ȡ����
	m_pGameFlash->SetFocus();
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::IPanelChangeOb_ExitFullScreen", "===OUT");
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
	vecText.push_back( "��ʼ����" );

	m_pWndDownPercent->SetText( vecText );
	m_pWndDownPercent->SetDownPercent( 0 );
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::HttpDownOb_DownStart", "===OUT");
}

void GamePanelWnd::HttpDownOb_DownFinish( string& strID, string& strSwfPath )
{
	if( strID != m_swfGame.strID )
		return;

	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::HttpDownOb_DownFinish", "===IN");
	//���ȴ��ڲ���
	m_pWndDownPercent->SetFailed( false );
	vector<string> vecText;
	vecText.push_back( "�������,������..." );
	m_pWndDownPercent->SetText( vecText );
	m_pWndDownPercent->SetDownPercent( 1.0 );

	//����swfurl��ȥ���ո�
	YL_StringUtil::ReplaceAll( m_swfGame.strPicUrl, " ", "%20" );
	//��������ͼ
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

	//��ȡ����
	GLOBAL_LOCALGAME->ILocalGameData_AddGame( strID, m_swfGame.strName, strPicLocalDesPath, strSwfPath, m_swfGame.strIntro, 0 );

	OneLocalGame olg;
	GLOBAL_LOCALGAME->ILocalGameData_GetGameByID( strID, olg );

	PlayMovie( olg.strID, olg.strGamePath );
	m_bDown = false;
	UpdateAllWnd();
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::HttpDownOb_DownFinish", "===OUT");
}

void GamePanelWnd::HttpDownOb_DownFailed( string& strID, HTTP_DOWN_FAILED_REASON r )
{
	if( strID != m_swfGame.strID )
		return;

	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::HttpDownOb_DownFailed", "===IN");
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
			AfxGetUserConfig()->GetConfigStringValue( CONF_APP_MODULE_NAME, CONF_APP_SWF_PATH, strValue );
			YL_StringUtil::Format( strFailedReason, "�޷������ļ�����鿴����Ŀ¼%s�Ƿ�ֻ��", strValue.c_str() );
		}
		break;
	case HTTP_FAILED_CANT_WRITE_LOCAL_FILE:
		{
			string strValue;
			AfxGetUserConfig()->GetConfigStringValue( CONF_APP_MODULE_NAME, CONF_APP_SWF_PATH, strValue );
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
	if( strID != m_swfGame.strID )
		return;
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::HttpDownOb_DownProgress", "===IN");
	m_bDown = true;
	UpdateAllWnd();

	vector<string> vecText;
	string strText;
	//�ļ���С\t���ذٷֱ�
	YL_StringUtil::Format( strText, "%s   ", UINT2CString( unFileSize ).GetString() );
	
	string strTmp;
	if( unFileSize == 0 )
	{
		YL_StringUtil::Format( strTmp, "������..." );
	}else
	{
		YL_StringUtil::Format( strTmp, "%d%%", (int)(dPercent*100) );
	}	
	strText += strTmp;
	vecText.push_back( strText );
	//�����ٶ�
	YL_StringUtil::Format( strText, "�����ٶ�:%s/s   �ٵȴ�:%s", 
		UINT2CString( unSpeed ).GetString(),
		GetLeftTime( unFileSize, unSpeed, (unsigned int)(dPercent*100) ).GetString() );
	vecText.push_back( strText );	

	m_pWndDownPercent->SetFailed( false );
	m_pWndDownPercent->SetText( vecText );
	m_pWndDownPercent->SetDownPercent( dPercent );
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::HttpDownOb_DownProgress", "===OUT");
}

void GamePanelWnd::SetGameEntry( SWF_GAME sg )
{
	YL_Log("CGameCtrlWnd.txt", LOG_DEBUG, "CGameCtrlWnd::SetGameEntry", "===IN");
	m_swfGame = sg;

	OneLocalGame olg;
	if( GLOBAL_LOCALGAME->ILocalGameData_GetGameByID( m_swfGame.strID, olg ) )
	{
		m_bDown = false;
		YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::SetGameEntry", 
			"m_swfGame.strID:%s,olg.strGamePath:%s", 
			m_swfGame.strID.c_str(),
			olg.strGamePath.c_str());
		PlayMovie( olg.strID, olg.strGamePath );
		UpdateAllWnd();

		m_pGameCtrl->m_strGameName = sg.strName;
	}else
	{
		m_bDown = true;
		//����swfurl��ȥ���ո�
		YL_StringUtil::ReplaceAll( m_swfGame.strSwfUrl, " ", "%20" );
		//����swf�ļ�
		HttpDownCtrl::GetInstance()->StartDown( m_swfGame.strID, m_swfGame.strSwfUrl );
		UpdateAllWnd();
	}
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::SetGameEntry", "===OUT");
}


void GamePanelWnd::PlayMovie( string strID, string strPath )
{
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::PlayMovie", "===IN");
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd", "strPath:%s", strPath.c_str());
	
	//����swf	
	if( m_pGameFlash->GetMovie().Compare( strPath.c_str() ) == 0  )
	{
		m_pGameFlash->SetMovie( GlobalSwfPath::Nothing() );
	}
	m_pGameFlash->SetMovie(strPath.c_str());
	m_pGameFlash->Play();
	m_pGameFlash->SetFocus();
	//��¼FIRST_SWF_GAME
	LogPlaySwfGameAct();
	YL_Log("GamePanelWnd.txt", LOG_DEBUG, "GamePanelWnd::PlayMovie", "===OUT");
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
	{
		str.Format("%d KB", ui/(1<<10));
	}else
	{
		str.Format("%d B", ui);
	}

	return str;
}


void GamePanelWnd::LogPlaySwfGameAct()
{
	char FIRST_SWF_GAME[2];
	memset(FIRST_SWF_GAME, 0, 2);
	YL_EncFileReg::ReadString( HKEY_PLAYBOX_ROOT,STR_REG_SOFT,"FIRST_SWF_GAME",FIRST_SWF_GAME,2 );

	if( strlen( FIRST_SWF_GAME ) == 0 )
	{
		char szID[128];
		if( CLhcImg::GetUserID( szID,128) && strcmp(szID,"0") != 0  )
		{
			char temp[2];
			strcpy(temp, "1");
			YL_EncFileReg::WriteString( HKEY_PLAYBOX_ROOT,STR_REG_SOFT,"FIRST_SWF_GAME",temp );
			LogRealMsg( "FIRST_SWF_GAME","");

		}
	}
	//ÿ��һ����Ϸ����¼һ��
	string strPlayLog;
	YL_StringUtil::Format( strPlayLog, "gameid:%s|name:%s", m_swfGame.strID.c_str(), m_swfGame.strName.c_str() );
	LogRealMsg( "PLAY_SWF_GAME", strPlayLog.c_str() );
	//�������Ϸ������1
	int nListen = 0;
	AfxGetUserConfig()->GetConfigIntValue( CONF_APP_MODULE_NAME, CONF_APP_PLAYED_GAME, nListen );
	nListen++;
	AfxGetUserConfig()->SetConfigIntValue( CONF_APP_MODULE_NAME, CONF_APP_PLAYED_GAME, nListen );
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
		m_pBtnSwitch->ShowWindow( SW_HIDE );
		m_pGameFlash->ShowWindow( SW_HIDE );
		m_pGameCtrl->ShowWindow( SW_HIDE );
		m_pGameInfo->ShowWindow( SW_HIDE );
	}else
	{
		m_pWndDownPercent->ShowWindow( SW_HIDE );
		m_pBtnSwitch->ShowWindow( SW_SHOW );
		m_pBtnSwitch->Invalidate();
		m_pGameFlash->ShowWindow( SW_SHOW );
		if( m_pBtnSwitch->GetCheck() == 1 )
		{
			m_pGameInfo->ShowWindow( SW_HIDE );
			m_pGameCtrl->ShowWindow( SW_SHOW );
			m_pGameCtrl->UpdateAllWnd();
		}else
		{
			m_pGameCtrl->ShowWindow( SW_HIDE );
			m_pGameInfo->ShowWindow( SW_SHOW );
		}
	}
	YL_Log("CGameCtrlWnd.txt", LOG_DEBUG, "CGameCtrlWnd::UpdateAllWnd", "===OUT");
}
void GamePanelWnd::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);	
	m_pWndDownPercent->MoveWindow( 0, 0, cx, cy );
	UpdateAllWnd();	
}

void GamePanelWnd::OnClickedSwitch()
{
	if( m_pBtnSwitch->GetCheck() == 1 )
	{
		m_pBtnSwitch->SetCheck( 0 );
	}else
	{
		m_pBtnSwitch->SetCheck( 1 );
	}
	UpdateAllWnd();
}

LRESULT GamePanelWnd::OnClickedReplay(WPARAM wParam,LPARAM lParama)
{
	SetGameEntry( m_swfGame );
	return 0L;
}

LRESULT GamePanelWnd::OnClickedCut(WPARAM wParam,LPARAM lParama)
{
	string strDesktopPath;
	char szTmpPath[MAX_PATH] = {0};
	SHGetSpecialFolderPath (AfxGetMainWindow()->m_hWnd, szTmpPath, CSIDL_DESKTOP, FALSE);
	strDesktopPath = szTmpPath;

	if (!strDesktopPath.length ())
		return 1L;

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
	
	return 0L;
}

void GamePanelWnd::ILayoutChangeOb_InitFinished()
{
	YL_Log("GamePlayingWnd.txt", LOG_DEBUG, "GamePlayingWnd::ILayoutChangeOb_InitFinished", "===IN");
	UpdateAllWnd();
	YL_Log("GamePlayingWnd.txt", LOG_DEBUG, "GamePlayingWnd::ILayoutChangeOb_InitFinished", "===OUT");
}
void GamePanelWnd::ILayoutChangeOb_SkinChanged(string oldSkinPath, string newSkinPath)
{
	YL_Log("GamePlayingWnd.txt", LOG_DEBUG, "GamePlayingWnd::ILayoutChangeOb_SkinChanged", "===IN");
	UpdateAllWnd();
	YL_Log("GamePlayingWnd.txt", LOG_DEBUG, "GamePlayingWnd::ILayoutChangeOb_SkinChanged", "===OUT");
}

void GamePanelWnd::ILayoutChangeOb_UpdateLayout(HWND hWnd)
{
	YL_Log("GamePlayingWnd.txt", LOG_DEBUG, "GamePlayingWnd::ILayoutChangeOb_UpdateLayout", "===IN");
	if( hWnd == GetSafeHwnd() )
	{
		UpdateAllWnd();
	}
	YL_Log("GamePlayingWnd.txt", LOG_DEBUG, "GamePlayingWnd::ILayoutChangeOb_UpdateLayout", "===OUT");
}