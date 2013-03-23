#include "stdafx.h"

#include "../../../core/MessageID.h"
#include "DlgConfig.h"
#include "shlobj.h"
#include "../ConfigSettingDef.h"
#include "./src/Core/AfxGlobals.h"
#include "IPanelChange.h"
#include ".\dlgconfig.h"
#include "../LayoutMgr/SeperateMode/HostWndMgr.h"

HWND g_hLastConfWnd = NULL;

IMPLEMENT_DYNAMIC(CConfigDialog, CDialog)

CConfigDialog::CConfigDialog(int iShowIdx, CWnd *pParent) 
	: CDialog(CConfigDialog::IDD, pParent)
{
	m_iShowIdx	= iShowIdx;
	m_bFullScreen = FALSE;
	m_minOrExit = FALSE;
	m_bNoAsk     = FALSE;
	m_bkg= ::AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("ConfigDlg");
}

CConfigDialog::~CConfigDialog()
{
}

BEGIN_MESSAGE_MAP(CConfigDialog,CDialog)
	ON_BN_CLICKED(IDC_CONF_BTN_OK, OnClickedOK)
    ON_BN_CLICKED(IDCANCEL, OnClickedCancel)
	ON_BN_CLICKED(IDC_CONF_BTN_CLOSE, OnClickedCancel)
	ON_BN_CLICKED(IDC_CONF_BTN_RESET, OnConfigBtnDefaultBnClicked)
	ON_BN_CLICKED(IDC_CONF_CHK_AUTORUN,			OnBnClickedCheckAutoRun)
	ON_BN_CLICKED(IDC_CONF_CHK_ENABLE_BOSSKEY,	OnBnClickedChkBosskey)
	ON_BN_CLICKED(IDC_CONF_RAD_MIN,		OnBnClickedRadMin)
	ON_BN_CLICKED(IDC_CONF_RAD_EXIT,	OnBnClickedRadExit)
	ON_BN_CLICKED(IDC_CONF_CHK_NOASK,	OnBnClickedChkNoask)
	ON_BN_CLICKED(IDC_CONF_CHK_AUTOLOGIN,	OnBnClickedChkAutoLogin)
// 	ON_WM_CTLCOLOR()
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HOTKEY_BOSSKEY, m_BossKeyCtrl);
}

BOOL CConfigDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	COLORREF m_color(RGB(255,0,255));
	SetWindowRgn(m_bkg->CreateRgnFromBitmap(m_color,this), TRUE);

	//保证同时只出现一个配置对话框
	if(g_hLastConfWnd != NULL && g_hLastConfWnd != m_hWnd)
	{
		::SendMessage(g_hLastConfWnd, WM_COMMAND, IDCANCEL, 0);
	}
	g_hLastConfWnd = m_hWnd;

	CenterWindow();
	CRect rctNULL(0,0,0,0);

	m_btnOK.Create(NULL,WS_VISIBLE,rctNULL,this,IDC_CONF_BTN_OK);
	m_btnCancle.Create(NULL, WS_VISIBLE, rctNULL, this, IDCANCEL);
	m_btnReset.Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CONF_BTN_RESET);
	m_btnClose.Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CONF_BTN_CLOSE);
	m_checkedAutoLogin.Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CONF_CHK_AUTOLOGIN);
	m_unChkedAutoLogin.Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CONF_CHK_AUTOLOGIN);

	m_checkedAutoRun.Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CONF_CHK_AUTORUN);
	m_unChkedAutoRun.Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CONF_CHK_AUTORUN);
	m_checkUseBossKey.Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CONF_CHK_ENABLE_BOSSKEY);
	m_unChkUseBossKey.Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CONF_CHK_ENABLE_BOSSKEY);
	m_radMin_Checked.Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CONF_RAD_MIN);
	m_radMin_unChked.Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CONF_RAD_MIN);
	m_radExit_Checked.Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CONF_RAD_EXIT);
	m_radExit_unChked.Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CONF_RAD_EXIT);
	m_checkedNoAsk.Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CONF_CHK_NOASK);
	m_unChkedNoAsk.Create(NULL, WS_VISIBLE, rctNULL, this, IDC_CONF_CHK_NOASK);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();

	pLayoutMgr->RegisterCtrl( this,"ConfigDlg_btnOK",&m_btnOK);
	pLayoutMgr->RegisterCtrl( this,"ConfigDlg_btnCancel",&m_btnCancle);
	pLayoutMgr->RegisterCtrl( this,"ConfigDlg_btnReset",&m_btnReset);
	pLayoutMgr->RegisterCtrl( this,"ConfigDlg_btnClose",&m_btnClose);
	pLayoutMgr->RegisterCtrl(this, "ConfigDlg_SYS_chkAutoLogin_checked",&m_checkedAutoLogin);
	pLayoutMgr->RegisterCtrl(this, "ConfigDlg_SYS_chkAutoLogin_unchecked",&m_unChkedAutoLogin);


	pLayoutMgr->RegisterCtrl(this, "ConfigDlg_SYS_chkAutorun_checked",	&m_checkedAutoRun);
	pLayoutMgr->RegisterCtrl(this, "ConfigDlg_SYS_chkAutorun_unchecked",&m_unChkedAutoRun);
	pLayoutMgr->RegisterCtrl(this, "ConfigDlg_SYS_chkBosskey_checked",	&m_checkUseBossKey);
	pLayoutMgr->RegisterCtrl(this, "ConfigDlg_SYS_chkBosskey_unchecked",&m_unChkUseBossKey);
	pLayoutMgr->RegisterCtrl(this, "ConfigDlg_SYS_radMin_Checked",		&m_radMin_Checked);
	pLayoutMgr->RegisterCtrl(this, "ConfigDlg_SYS_radMin_unChecked",	&m_radMin_unChked);
	pLayoutMgr->RegisterCtrl(this, "ConfigDlg_SYS_radExit_Checked",		&m_radExit_Checked);
	pLayoutMgr->RegisterCtrl(this, "ConfigDlg_SYS_radExit_unChecked",	&m_radExit_unChked);
	pLayoutMgr->RegisterCtrl(this, "ConfigDlg_SYS_chkNoAsk_checked",	&m_checkedNoAsk);
	pLayoutMgr->RegisterCtrl(this, "ConfigDlg_SYS_chkNoAsk_unchecked",	&m_unChkedNoAsk);
	

	m_checkedNoAsk.ShowWindow(SW_HIDE);
	m_unChkedNoAsk.ShowWindow(SW_HIDE);

	pLayoutMgr->CreateControlPane( this,"ConfigDlg","normal");	
	pLayoutMgr->CreateBmpPane( this,"ConfigDlg","normal" );


	// Load Config
	//开机自启动	
	m_bAutoRun = GlobalFunc::IsAutoRun();
	//最小化或关闭
	AfxUserConfig*	pUserConfig = AfxGetUserConfig();
	bool bValue;
	pUserConfig->GetConfigBoolValue( CONF_SETTING_MODULE_NAME,CONF_SETTING_CONFIG_EXITCHOICE,bValue);
	m_minOrExit = bValue;

	pUserConfig->GetConfigBoolValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_AUTO_LOGIN, bValue);
	m_bAutoLogin = bValue;

	pUserConfig->GetConfigBoolValue( CONF_SETTING_MODULE_NAME,CONF_SETTING_CONFIG_IFASKEXIT,bValue);
	m_bNoAsk = bValue;
	pUserConfig->GetConfigBoolValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_ENABLE_BOSS_KEY, bValue);
	m_bUseBossKey = bValue;

	int iKeyValue = 0;
	pUserConfig->GetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_BOSS_KEY_VALUE, iKeyValue);
	m_BossKeyCtrl.SetHotKey(HIWORD( *((DWORD*)&iKeyValue) ), LOWORD( *((DWORD*)&iKeyValue) ));

	UpdateData(FALSE);

	m_btnReset.ShowWindow(SW_HIDE);

/* * * * * * * * * * * * * * 原版 * * * *
*	RECT rect;
*	GetDlgItem(IDC_STATIC_RECT)->GetWindowRect(&rect);
*	POINT point;
*	point.x = rect.left;
*	point.y = rect.top;
*	ScreenToClient(&point);
*    //创建右边的所有对话框
*	CRect dlgRc = CRect( point.x,point.y,(rect.right - rect.left)+point.x,(rect.bottom-rect.top)+point.y );
* * * * * * * * * * * * * * * * * * * * */
/*
	CRect dlgRc;
	GetDlgItem(IDC_STATIC_RECT)->GetClientRect(&dlgRc);

	m_downDlg.Create( CDownDialog::IDD, this );
	m_downDlg.m_strAlert = "下载";
	
	m_sysDlg.Create( CSysDialog::IDD, this );
	m_sysDlg.m_strAlert = "系统";

	m_mapBase[CONF_IDX_DOWN]  = &m_downDlg;
	m_mapBase[CONF_IDX_SYS]   = &m_sysDlg;

	//摆放各wnd位置
	CRect rc(dlgRc);
	for( int i=0; i<m_mapBase.size(); i++ )
	{
		AfxGetUIManager()->UIAddDialog(m_mapBase[i]);
		CRect rcOne;
		m_mapBase[i]->GetWindowRect(&rcOne);

		rc.bottom = rc.top + rcOne.Height();
		m_mapBase[i]->MoveWindow( rc );
		m_mapBase[i]->ShowWindow(SW_SHOW);


		rc.top = rc.bottom;
	}
*/
	if( m_bFullScreen )
	{
		::SetWindowPos(m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
	}


	CRect winPos(0,0,520,370);
	SetWindowPos(NULL, 0, 0, 520, 370, SWP_NOMOVE);
	m_BossKeyCtrl.MoveWindow(130, 162, 205, 22);
	CenterWindow();

	YL_Log("AddDlg.txt",LOG_DEBUG,"add","ConfigDlg");
	AfxGetUIManager()->UIAddDialog(this);
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_APPEXIT,this );
	return TRUE; 
}

void CConfigDialog::OnClickedOK()
{
	//检测是否所有对话框上的值都是在指定范围内的
	if( !IsAllInputOK() )
		return;

	//保存所有配置
	/* * * * * * * * * * * * * * *
	for( int i=0; i<m_mapBase.size(); i++ )
	{
		m_mapBase[i]->SaveConf();
	}
	//摧毁所有窗口
	for( int i=0; i<m_mapBase.size(); i++ )
	{
		//m_mapBase[i]->DestroyWindow();
		m_mapBase[i]->EndDialog(0);
		AfxGetUIManager()->UIRemoveDialog(m_mapBase[i]);
	}
	* * * * * * * * * * * * * * */
	UpdateData();
	GlobalFunc::CreateGBoxRunInStartUp( m_bAutoRun );

	bool bValue = m_minOrExit;
	AfxGetUserConfig()->SetConfigBoolValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_CONFIG_EXITCHOICE,bValue);

	bValue = m_bAutoLogin;
	AfxGetUserConfig()->SetConfigBoolValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_AUTO_LOGIN, bValue);

	bValue = m_bNoAsk;
	AfxGetUserConfig()->SetConfigBoolValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_CONFIG_IFASKEXIT,bValue);
	bValue = m_bUseBossKey;
	AfxGetUserConfig()->SetConfigBoolValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_ENABLE_BOSS_KEY, bValue );

	WORD wMod, wVK;
	m_BossKeyCtrl.GetHotKey(wVK, wMod);
	int iKeyValue = wVK;
	iKeyValue = (iKeyValue<<16) | wMod;
	AfxGetUserConfig()->SetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_BOSS_KEY_VALUE, iKeyValue);


	g_hLastConfWnd = NULL;

	YL_Log("AddDlg.txt",LOG_DEBUG,"remove","ConfigDlg");
	AfxGetUIManager()->UIRemoveDialog(this);
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_APPEXIT,this );
	OnOK();
}

bool CConfigDialog::IsAllInputOK()
{
	string strMsg;
	for( int i=0; i<m_mapBase.size(); i++ )
	{
		if( !m_mapBase[i]->IsAllInputOK( strMsg ) )
		{
			if( strMsg.length() != 0 )
			{
				MessageBox( strMsg.c_str(), m_mapBase[i]->m_strAlert.c_str() );
			}		
			return false;
		}
	}
	return true;
}

void CConfigDialog::OnClickedCancel()
{
	g_hLastConfWnd = NULL;

	for( int i=0; i<m_mapBase.size(); i++ )
	{
		//m_mapBase[i]->DestroyWindow();
		m_mapBase[i]->EndDialog(0);
		AfxGetUIManager()->UIRemoveDialog(m_mapBase[i]);
	}
	AfxGetUIManager()->UIRemoveDialog(this);
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_APPEXIT,this );

	OnCancel();
}

void CConfigDialog::OnConfigBtnDefaultBnClicked()
{
	for( int i=0; i<m_mapBase.size(); i++ )
	{
		m_mapBase[i]->SetDefault();
	}
	GetDlgItem(IDC_CONF_BTN_OK)->EnableWindow();
	LogUserActMsg( "SETTING", "SET_CHANGE:DEFAULT" );
}

void CConfigDialog::IAppExit()
{
	SendMessage(WM_COMMAND,IDCANCEL,0);
}

HBRUSH CConfigDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	hbr = ::CreateSolidBrush( RGB(245,245,245) );
	return hbr;
}
void CConfigDialog::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	AfxGetUIManager()->UIGetLayoutMgr()->PaintBkGround( m_hWnd ,&dc,false );
}

void CConfigDialog::SetDefault()
{
	m_bAutoRun	= TRUE;
	m_minOrExit	= FALSE;
	m_bNoAsk	= FALSE;
	m_bUseBossKey=FALSE;
	UpdateData( FALSE );
}

void CConfigDialog::OnBnClickedRadMin()
{
	m_minOrExit = FALSE;
	UpdateData(FALSE);
}

void CConfigDialog::OnBnClickedRadExit()
{
	m_minOrExit = TRUE;
	UpdateData(FALSE);
}

void CConfigDialog::OnBnClickedChkNoask()
{
	m_bNoAsk = !m_bNoAsk;
	UpdateData(FALSE);
}

void CConfigDialog::OnBnClickedCheckAutoRun()
{
	m_bAutoRun = !m_bAutoRun;
	UpdateData(FALSE);
}

void CConfigDialog::OnBnClickedChkBosskey()
{
	m_bUseBossKey = !m_bUseBossKey;
	UpdateData(FALSE);
}

void CConfigDialog::OnBnClickedChkAutoLogin()
{
	m_bAutoLogin = !m_bAutoLogin;
	UpdateData(FALSE);
}

BOOL CConfigDialog::UpdateData( BOOL bSaveAndValidate /* = TRUE */ )
{
	if (!bSaveAndValidate)
	{
		m_BossKeyCtrl.EnableWindow(m_bUseBossKey);
		if (m_bAutoRun)		// 开机启动
		{
			m_checkedAutoRun.ShowWindow(SW_SHOW);
			m_unChkedAutoRun.ShowWindow(SW_HIDE);
		}
		else
		{
			m_checkedAutoRun.ShowWindow(SW_HIDE);
			m_unChkedAutoRun.ShowWindow(SW_SHOW);
		}

		if (m_bUseBossKey)	//启用老板键
		{
			m_checkUseBossKey.ShowWindow(SW_SHOW);
			m_unChkUseBossKey.ShowWindow(SW_HIDE);
		}
		else
		{
			m_checkUseBossKey.ShowWindow(SW_HIDE);
			m_unChkUseBossKey.ShowWindow(SW_SHOW);
		}

		if (m_bNoAsk)		// 不再询问
		{
			/* * *
			m_checkedNoAsk.ShowWindow(SW_SHOW);
			m_unChkedNoAsk.ShowWindow(SW_HIDE);
			* * * */
			m_checkedNoAsk.ShowWindow(SW_HIDE);
			m_unChkedNoAsk.ShowWindow(SW_HIDE);
		}
		else
		{
			/* * * * 
			m_checkedNoAsk.ShowWindow(SW_HIDE);
			m_unChkedNoAsk.ShowWindow(SW_SHOW);
			* * * * */
			m_checkedNoAsk.ShowWindow(SW_HIDE);
			m_unChkedNoAsk.ShowWindow(SW_HIDE);
		}

		if (m_minOrExit)	// 默认选择 退出/最小化
		{
			m_minOrExit = TRUE;
			m_radMin_Checked.ShowWindow(SW_HIDE);
			m_radMin_unChked.ShowWindow(SW_SHOW);
			m_radExit_Checked.ShowWindow(SW_SHOW);
			m_radExit_unChked.ShowWindow(SW_HIDE);
		}
		else
		{			
			m_radMin_Checked.ShowWindow(SW_SHOW);
			m_radMin_unChked.ShowWindow(SW_HIDE);
			m_radExit_Checked.ShowWindow(SW_HIDE);
			m_radExit_unChked.ShowWindow(SW_SHOW);
		}

		if (m_bAutoLogin)
		{
			m_checkedAutoLogin.ShowWindow(SW_SHOW);
			m_unChkedAutoLogin.ShowWindow(SW_HIDE);
		}
		else
		{
			m_checkedAutoLogin.ShowWindow(SW_HIDE);
			m_unChkedAutoLogin.ShowWindow(SW_SHOW);
		}
		
	}
	else
	{	// bSaveAndValidate == TRUE
		m_bAutoRun		= m_checkedAutoRun.IsWindowVisible();
		m_bUseBossKey	= m_checkUseBossKey.IsWindowVisible();
		m_bNoAsk		= m_checkedNoAsk.IsWindowVisible();
		m_minOrExit		= m_radExit_Checked.IsWindowVisible();
		m_bAutoLogin	= m_checkedAutoLogin.IsWindowVisible();
	}

	return __super::UpdateData(bSaveAndValidate);
}

LRESULT CConfigDialog::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	rc.bottom=rc.top+29;
	return rc.PtInRect(point)?HTCAPTION:CDialog::OnNcHitTest(point);
}

void CConfigDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect rctAutoRun( CPoint(65, 58), CSize(70, 16));
	CRect rctAutoLogin( CPoint(65, 83), CSize(130, 16) );
	CRect rctBossKey( CPoint(65, 165), CSize(58, 16) );
	CRect rctMin( CPoint(65, 245), CSize(70, 16) );
	CRect rctExit( CPoint(65, 270), CSize(50, 16) );
	if (rctAutoLogin.PtInRect(point))
	{
		OnBnClickedChkAutoLogin();
	}
	else if (rctAutoRun.PtInRect(point))
	{
		OnBnClickedCheckAutoRun();
	}
	else if (rctBossKey.PtInRect(point))
	{
		OnBnClickedChkBosskey();
	}
	else if (rctMin.PtInRect(point))
	{
		OnBnClickedRadMin();
	}
	else if (rctExit.PtInRect(point))
	{
		OnBnClickedRadExit();
	}
	
}