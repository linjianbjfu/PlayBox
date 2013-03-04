#include "stdafx.h"
#include "TopPanelWnd.h"
#include "TopPanelDef.h"
#include "YL_StringUtil.h"
#include "TopPanel_Control.h"
#include "../../Core/CDataManager.h"
#include "../../gui/util/ShowMenu.h"
#include "../../AppConfig/config/ConfigAppDef.h"
#include "../../AppConfig/config/ConfigLayoutDef.h"
#include "../../AppConfig/config/ConfigSettingDef.h"
#include "../../AppConfig/config/Dlg/DlgConfig.h"
#include "../../DataInterface/IPanelChangeObserver.h"
#include "../LayoutMgr/ChangeColor/IChangeColorMgr.h"
#include "../module/IrregularBorder/IrregularBorderMgr.h"
#include "YL_DirInfo.h"
#include "YL_FileInfo.h"
#include "tools.h"
#include "MinOrExit.h"
#include "../../PlayBoxDlg.h"
#include "../AboutPanel/AboutDlg.h"
#include "CheckNewVerDlg.h"
#include "src/module/CheckUpdate/CheckUpdate.h"
#include "../UserMan/UserManager.h"

static const char* s_STRBUTTONLAYER		= "Normal_Large";
static const char* s_STRBUTTON_NORMAL	= "Normal";
static const char* s_STRBUTTON_LARGE	= "Large";

IMPLEMENT_DYNAMIC(CTopPanelWnd, CBasicWnd)
CTopPanelWnd::CTopPanelWnd():m_bHideMainWindow(FALSE),
m_pDLGCheckUpdate(NULL)
{
	m_pShowMenu = NULL;
	AfxGetUserConfig()->GetConfigBoolValue( CONF_APP_MODULE_NAME,CONF_APP_MAINWND_HOLD,m_bMainWndHold);
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_LAYOUTMGR,(ILayoutChangeObserver*) this);
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*) this);
	m_bNormal = true;
}

CTopPanelWnd::~CTopPanelWnd()
{
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_LAYOUTMGR,(ILayoutChangeObserver*) this);
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_PANEL_CHANGE,(IPanelChangeObserver*) this);
}

BEGIN_MESSAGE_MAP(CTopPanelWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CHAR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(ID_BTN_CLOSE, OnBnClickedClose)
	ON_BN_CLICKED(ID_BTN_LARGE,OnBnClickedLarge)
	ON_BN_CLICKED(ID_BTN_NORMAL,OnBnClickedNormal)
	ON_BN_CLICKED(ID_BTN_MIN,OnBnClickedMin)
	ON_BN_CLICKED(ID_BTN_SETTING,OnBnClickedSetting)
	ON_BN_CLICKED(IDC_TOPPANEL_HOLD, OnBnClickedHold)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_MEMU_SETTING, OnSetting)
	ON_COMMAND(ID_MEMU_LOGON_OUT, OnLogonOut)
	ON_COMMAND(ID_MEMU_MAIN_PAGE, OnMenuMainPage)
	ON_COMMAND(ID_MEMU_PROBLEM_REPORT, OnMenuProblemReport)
	ON_COMMAND(ID_MENU_WEB_GAME_CUSTOM_SERVICE, OnMenuWebGameCustomService)
	ON_COMMAND(ID_MEMU_HELP_ABOUT, OnHelpAbout)
	ON_COMMAND(ID_CHECK_UPDATE, OnCheckNewVersion)

	ON_MESSAGE(MSG_GETLAYOUT,OnGetLayout)
	ON_MESSAGE(MSG_SAVELAYOUT,OnSaveLayout)
END_MESSAGE_MAP()

void CTopPanelWnd::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if(lpMeasureItemStruct->CtlType==ODT_MENU && IsMenu((HMENU)lpMeasureItemStruct->itemID))
	{
		if( m_pShowMenu != NULL )
		{
			m_pShowMenu->MeasureItem( lpMeasureItemStruct);		
		}
	}
	__super::OnMeasureItem( nIDCtl,lpMeasureItemStruct);
}
int CTopPanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CRect rectNULL(0,0,0,0);

	m_btnClose.Create(NULL,NULL,rectNULL,this,ID_BTN_CLOSE);
	m_btnMin.Create( NULL,NULL,rectNULL,this,ID_BTN_MIN );
	m_btnNormal.Create( NULL,NULL,rectNULL,this,ID_BTN_NORMAL);
	m_btnLarge.Create(NULL,NULL,rectNULL,this,ID_BTN_LARGE);
	m_btnSetting.Create(NULL, NULL, rectNULL, this, ID_BTN_SETTING);

	CTopPanelControl::GetInstance()->SetTopPanel(this);

	RECT   r;   
	SystemParametersInfo( SPI_GETWORKAREA, sizeof(RECT), &r, 0 );   

	m_iDesktopWidth		= r.right - r.left;
	m_iDesktopHeight	= r.bottom - r.top;

	AfxGetUserConfig()->GetConfigBoolValue( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_NORMAL,m_bNormal);
	AfxGetUserConfig()->GetConfigIntValue( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_HEIGHT,m_iWndNormalHeigth);
	AfxGetUserConfig()->GetConfigIntValue( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_WIDTH,m_iWndNormalWidth);
	AfxGetUserConfig()->GetConfigIntValue( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_X_POS,m_iXPos);
	AfxGetUserConfig()->GetConfigIntValue( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_Y_POS,m_iYPos);

	//对话框最小大小控制
	int mincx ,mincy;
	AfxGetUserConfig()->GetConfigIntValue( CONF_LAYOUT_MODULE_NAME, CONF_LAYOUT_MIN_WIDTH, mincx );
	AfxGetUserConfig()->GetConfigIntValue( CONF_LAYOUT_MODULE_NAME, CONF_LAYOUT_MIN_HEIGHT, mincy );

	//解决config.ini中大小参数全部变成0的问题。
	if(mincx < 640 || mincy < 400 )
	{
		mincx = 640;
		mincy = 400;
	}
	
	m_mincx = mincx;
	m_mincy = mincy;	
	
	if(m_iWndNormalHeigth < mincy)
		m_iWndNormalHeigth = 580;
	if(m_iWndNormalWidth < mincx)
		m_iWndNormalWidth = 830;


	if( m_iXPos <= 0 || m_iXPos >= m_iDesktopWidth )
	{
		m_iXPos = 100;
	}
	if( m_iYPos <= 0 || m_iYPos >= m_iDesktopHeight )
	{
		m_iYPos = 100;
	}

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();

	pLayoutMgr->RegisterCtrl( this,"Close",&m_btnClose);
	pLayoutMgr->RegisterCtrl( this,"Normal",&m_btnNormal);
	pLayoutMgr->RegisterCtrl( this,"Large",&m_btnLarge);
	pLayoutMgr->RegisterCtrl( this,"Min",&m_btnMin);
	pLayoutMgr->RegisterCtrl( this,"Setting",&m_btnSetting);
	pLayoutMgr->CreateControlPane( this,"toppanel","normal");	
	pLayoutMgr->CreateBmpPane( this,"toppanel","normal" );
	bool bEnableBossKey;
	AfxGetUserConfig()->GetConfigBoolValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_ENABLE_BOSS_KEY, bEnableBossKey);
	if (bEnableBossKey)
	{
		int iKeyValue;
		AfxGetUserConfig()->GetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_BOSS_KEY_VALUE, iKeyValue);
		RegisterBossKey(iKeyValue);
	}
	
	PostMessage(MSG_GETLAYOUT);
	return 0;
}

LRESULT CTopPanelWnd::OnGetLayout(WPARAM wParam,LPARAM lParam)
{
	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	if( m_bNormal)
	{
		if (!m_bHideMainWindow)
		{
			AfxGetMainWindow()->ShowWindow(SW_SHOW);
		}else{
			m_bHideMainWindow=FALSE;
		}
		pLayoutMgr->ShowLayer( s_STRBUTTONLAYER, s_STRBUTTON_LARGE );
		::MoveWindow( AfxGetMainWindow()->m_hWnd,m_iXPos,m_iYPos,m_iWndNormalWidth,m_iWndNormalHeigth,TRUE);
	}
	else
	{
		if (!m_bHideMainWindow)
		{
			AfxGetMainWindow()->ShowWindow(SW_SHOW);
		}else
		{
			m_bHideMainWindow=FALSE;
		}
		pLayoutMgr->ShowLayer( s_STRBUTTONLAYER, s_STRBUTTON_NORMAL );
		::MoveWindow( AfxGetMainWindow()->m_hWnd,0,0,m_iDesktopWidth,m_iDesktopHeight,TRUE);
	}
	return 0;
}

LRESULT CTopPanelWnd::OnSaveLayout(WPARAM wParam,LPARAM lParam)
{
	if( m_bNormal )
	{
		CRect rectWnd;
		AfxGetMainWindow()->GetWindowRect(rectWnd);

		m_iXPos	= rectWnd.left;
		m_iYPos	= rectWnd.top;
		m_iWndNormalWidth	= rectWnd.right - rectWnd.left;
		m_iWndNormalHeigth	= rectWnd.bottom - rectWnd.top;
	}

	if(m_iXPos<0 || m_iYPos<0 || m_iWndNormalWidth < 0 || m_iWndNormalHeigth<0 
		|| m_iWndNormalWidth < m_mincx || m_iWndNormalHeigth < m_mincy )
		return 0;

	AfxGetUserConfig()->SetConfigIntValue( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_HEIGHT,m_iWndNormalHeigth);
	AfxGetUserConfig()->SetConfigIntValue( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_WIDTH,m_iWndNormalWidth);
	AfxGetUserConfig()->SetConfigIntValue( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_X_POS,m_iXPos);
	AfxGetUserConfig()->SetConfigIntValue( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_Y_POS,m_iYPos);
	return 0;
}

void CTopPanelWnd::OnDestroy()
{
	SendMessage(MSG_SAVELAYOUT);
}

void CTopPanelWnd::OnBnClickedClose()
{
	bool askexit,exitchoice;
	AfxGetUserConfig()->GetConfigBoolValue( CONF_SETTING_MODULE_NAME,CONF_SETTING_CONFIG_IFASKEXIT,askexit);
	if (!askexit)
	{
		CMinOrExit dlg( GetParent() );
		AfxGetUIManager()->UIAddDialog(&dlg);
		int ifexit=dlg.DoModal();
		AfxGetUIManager()->UIRemoveDialog(&dlg);
		if (ifexit==IDCANCEL)
		{
			return;
		}
	}

	AfxGetUserConfig()->GetConfigBoolValue( CONF_SETTING_MODULE_NAME,CONF_SETTING_CONFIG_EXITCHOICE,exitchoice);
	if (exitchoice)
	{		
		//点击关闭按钮，界面退出非常慢，所以先hide主窗口
		AfxGetMainWindow()->ShowWindow( SW_HIDE );
		LogRealMsg( "AppExit", "");
		::SendMessage(AfxGetMainWindow()->m_hWnd, MSG_DELETE_TRAY,0,0);
		::PostMessage( AfxGetMainWindow()->m_hWnd,WM_CLOSE,0,0 );
		LogUserActMsg(STR_USER_CLICK, "BTN:CLOSE");
	}
	else
	{
		AfxGetMainWindow()->ShowWindow( SW_HIDE );	//任务栏没图标
		//AfxGetMainWindow()->ShowWindow( SW_MINIMIZE );	//任务栏有图标
	}
}
void CTopPanelWnd::OnBnClickedLarge()
{
	GLOBAL_PANELCHANGEDATA->IPanelChange_Max();
}
void CTopPanelWnd::OnBnClickedNormal()
{
	GLOBAL_PANELCHANGEDATA->IPanelChange_Restore();
}
void CTopPanelWnd::OnBnClickedMin()
{
	CTopPanelControl::GetInstance()->DoMin();
}

void CTopPanelWnd::OnBnClickedSetting()
{
	//PostMessage( WM_COMMAND,ID_MEMU_SETTING,0);
	RECT rect;
	POINT point;
	m_btnSetting.GetWindowRect( &rect );
	point.x = rect.left;
	point.y = rect.bottom;
	CTopPanelControl::GetInstance()->ShowMenu(&m_pShowMenu, point);
}

void CTopPanelWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if( m_bNormal)
	{
		PostMessage( WM_COMMAND,ID_BTN_LARGE,0);
	}
	else
	{
		PostMessage( WM_COMMAND,ID_BTN_NORMAL,0);
	}
}

void CTopPanelWnd::LoadSkin()
{
}

void CTopPanelWnd::AddLayers()
{
	LoadSkin();
}

void CTopPanelWnd::ILayoutChangeOb_InitFinished()
{
}
void CTopPanelWnd::ILayoutChangeOb_SkinChanged(string oldSkinPath, string newSkinPath)
{
	if(oldSkinPath == newSkinPath)
		return;

	if(!m_bNormal)
		AfxGetUIManager()->UIGetLayoutMgr()->ShowLayer( s_STRBUTTONLAYER,s_STRBUTTON_NORMAL );
	else
		AfxGetUIManager()->UIGetLayoutMgr()->ShowLayer( s_STRBUTTONLAYER,s_STRBUTTON_LARGE );

	CRect rc;
	GetWindowRect(&rc);
	OnSize(0, rc.Width(), rc.Height());	
}

void CTopPanelWnd::OnAppExit()
{
	::PostMessage( AfxGetMainWindow()->m_hWnd,WM_CLOSE,0,0 );
	LogUserActMsg(STR_USER_CLICK, "BTN:CLOSE");
}

void CTopPanelWnd::OnSetting()
{
	CConfigDialog configDlg;
	AfxGetUIManager()->UIAddDialog(&configDlg);
	int ret = configDlg.DoModal();
	AfxGetUIManager()->UIRemoveDialog(&configDlg);
	if (ret == IDOK)
	{
		bool bEnableBossKey;
		AfxGetUserConfig()->GetConfigBoolValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_ENABLE_BOSS_KEY, bEnableBossKey);
		UnRegisterBossKey();
		if (bEnableBossKey)
		{
			int iKeyValue;
			AfxGetUserConfig()->GetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_BOSS_KEY_VALUE, iKeyValue);
			RegisterBossKey(iKeyValue);
		}
	}
}

void CTopPanelWnd::OnMenuMainPage()
{
	string strValue;
	AfxGetUserConfig()->GetConfigStringValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_CONFIG_MAIN_PAGE, strValue );
	if (!strValue.empty())
	{
		TAB_ITEM tabItem;
		tabItem.strTitle = TAB_OFFICE_SITE;
		tabItem.enumType = TAB_BROWSER;
		tabItem.strParam = "url=" + strValue;
		GLOBAL_TABBARDATA->ITabBar_ChangeTab( tabItem );
	}
}

void CTopPanelWnd::OnMenuProblemReport()
{
	string strValue;
	AfxGetUserConfig()->GetConfigStringValue( CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_CONFIG_WEB_GAME_CUSTOM_SERVICE, strValue );
	if (!strValue.empty())
	{
		TAB_ITEM tabItem;
		tabItem.strTitle = TAB_REPORT_PROBLEM;
		tabItem.enumType = TAB_BROWSER;
		tabItem.strParam = "url=" + strValue;
		GLOBAL_TABBARDATA->ITabBar_ChangeTab( tabItem );
	}
}

void CTopPanelWnd::OnMenuWebGameCustomService()
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

void CTopPanelWnd::OnHelpAbout()
{
	//open about dialog
	CAboutDlg dlg( GetParent() );
	AfxGetUIManager()->UIAddDialog(&dlg);
	int ifexit = dlg.DoModal();
	AfxGetUIManager()->UIRemoveDialog(&dlg);
}

void CTopPanelWnd::OnLogonOut()
{
	CUserManager::GetInstance()->User_Logout();
}

void CTopPanelWnd::OnCheckNewVersion()
{
	//CCheckNewVerDlg dlg;
	/*CCheckUpdate dlg;
	AfxGetUIManager()->UIAddDialog( (DWORD)&dlg);
	dlg.DoModal();
	AfxGetUIManager()->UIRemoveDialog( (DWORD)&dlg);*/
	if( m_pDLGCheckUpdate != NULL)
		m_pDLGCheckUpdate->ShowWindow(SW_SHOW);
	else
	{
		m_pDLGCheckUpdate = new CCheckUpdate;
		m_pDLGCheckUpdate->Create(IDD_DIALOG_CHKUPDATE,this);
		SetChildCHKUpdate(m_pDLGCheckUpdate);
		m_pDLGCheckUpdate->ShowWindow(SW_SHOW);
	}
}

void CTopPanelWnd::OnBnClickedHold()
{
	CTopPanelControl::GetInstance()->HoldWindow();
}

void CTopPanelWnd::IPanelChangeOb_WindowMax()
{
	if( ::AfxGetMainWindow()->IsWindowVisible() == FALSE
		|| ::AfxGetMainWindow()->IsIconic() == TRUE )
	{
		::AfxGetMainWnd()->ShowWindow( SW_RESTORE );
	}
	if( m_bNormal )
	{
		AfxGetUIManager()->UIGetLayoutMgr()->ShowLayer( s_STRBUTTONLAYER,s_STRBUTTON_NORMAL );
		m_bNormal =  false;

		CRect rectWnd;
		AfxGetMainWindow()->GetWindowRect(rectWnd);

		m_iXPos	= rectWnd.left;
		m_iYPos	= rectWnd.top;
		m_iWndNormalWidth	= rectWnd.right - rectWnd.left;
		m_iWndNormalHeigth	= rectWnd.bottom - rectWnd.top;

		RECT   r;   
		SystemParametersInfo(   SPI_GETWORKAREA,   sizeof(RECT),   &r,   0   );   

		AfxGetUserConfig()->SetConfigBoolValue( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_NORMAL,m_bNormal);
		::MoveWindow( AfxGetMainWindow()->m_hWnd,r.left,r.top, r.right-r.left, r.bottom-r.top,TRUE);
		AfxGetMainWindow()->ModifyStyle(0, WS_MAXIMIZE, 0);
		LogUserActMsg(STR_USER_CLICK, "BTN:TO_MAX"); 
	}	
}

void CTopPanelWnd::IPanelChangeOb_WindowRestore()
{
	if( ::AfxGetMainWindow()->IsWindowVisible() == FALSE
		|| ::AfxGetMainWindow()->IsIconic() == TRUE )
	{
		::AfxGetMainWnd()->ShowWindow( SW_RESTORE );
	}
	else if( !m_bNormal )
	{
		AfxGetUIManager()->UIGetLayoutMgr()->ShowLayer( s_STRBUTTONLAYER,s_STRBUTTON_LARGE );
 
		m_bNormal = true;
		AfxGetUserConfig()->SetConfigBoolValue( CONF_LAYOUT_MODULE_NAME,CONF_LAYOUT_NORMAL,m_bNormal);

		::MoveWindow( AfxGetMainWindow()->m_hWnd,m_iXPos,m_iYPos,m_iWndNormalWidth,m_iWndNormalHeigth,TRUE);
		AfxGetMainWindow()->ModifyStyle(WS_MAXIMIZE, 0,0);
		LogUserActMsg(STR_USER_CLICK, "BTN:TO_NOMARL");
	}
}

void CTopPanelWnd::IPanelChangeOb_WindowMin()
{
	CPlayBoxDlg * pDlg = (CPlayBoxDlg *)AfxGetMainWindow ();
	if (pDlg->m_bStartupRun)
		AfxGetMainWindow()->ShowWindow( SW_HIDE ); //任务栏没有图标
	else
		AfxGetMainWindow()->ShowWindow( SW_MINIMIZE );	//任务栏有图标

	AfxGetUIManager()->UIGetChangeColorMgr()->OnClose();
	LogUserActMsg(STR_USER_CLICK, "BTN:TO_MIN");
}

void CTopPanelWnd::IPanelChangeOb_WindowClose()
{
	OnBnClickedClose();
}

BOOL CTopPanelWnd::RegisterHotKey( UINT uHotKeyID, UINT uVirtualKey/*=0*/, UINT uModifiers/*=0*/,
								  BOOL bRegister /*= FALSE*/ )
{
	if (bRegister == FALSE)
	{
		return ::UnregisterHotKey(AfxGetMainWnd()->GetSafeHwnd(), uHotKeyID);
	}

	return ::RegisterHotKey(AfxGetMainWindow()->GetSafeHwnd(), uHotKeyID, uModifiers, uVirtualKey);
}

void CTopPanelWnd::RegisterBossKey(int iKeyValue)
{
	UINT uVK	= HIWORD( *((DWORD*)&iKeyValue) );
	UINT uMod	= LOWORD( *((DWORD*)&iKeyValue) );
	UINT uShift = (uMod&0x1) << 2;
	UINT uAlt	= uMod >> 2;
	uMod &= 0x02;
	uMod |= uShift |= uAlt;
	if (!RegisterHotKey(ID_HOTKEY_BOSSKEY, uVK, uMod, TRUE))
	{
		MessageBox(TEXT("老板键注册失败"), TEXT("警告"), MB_OK|MB_ICONWARNING);
	}
}

void CTopPanelWnd::UnRegisterBossKey()
{
	RegisterHotKey(ID_HOTKEY_BOSSKEY);
}

void CTopPanelWnd::SetChildCHKUpdate(CCheckUpdate *pCHKUpdate)
{
	if ((pCHKUpdate != NULL) &&(m_pDLGCheckUpdate != NULL))
	{
		if(pCHKUpdate == m_pDLGCheckUpdate)
		{
			AfxGetUIManager()->UIAddDialog(m_pDLGCheckUpdate);
			return ;
		}
		else
		{
			AfxGetUIManager()->UIRemoveDialog(m_pDLGCheckUpdate);
			m_pDLGCheckUpdate = pCHKUpdate;
			AfxGetUIManager()->UIAddDialog(m_pDLGCheckUpdate);
			return ;
		}
	}
	else if((pCHKUpdate == NULL )&&(m_pDLGCheckUpdate != NULL))
	{
		AfxGetUIManager()->UIRemoveDialog( (DWORD)m_pDLGCheckUpdate);
		m_pDLGCheckUpdate = pCHKUpdate;
		return ;
	}
	else if((pCHKUpdate != NULL)&& (m_pDLGCheckUpdate == NULL))
	{
		m_pDLGCheckUpdate = pCHKUpdate;
		AfxGetUIManager()->UIAddDialog(m_pDLGCheckUpdate);
		return ;
	}
	
}