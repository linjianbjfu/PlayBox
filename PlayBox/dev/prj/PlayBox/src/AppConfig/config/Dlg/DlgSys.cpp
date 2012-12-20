#include "stdafx.h"
#include "DlgSys.h"
#include <io.h>
#include "./src/Core/AfxGlobals.h"
#include "../ConfigSettingDef.h"
#include "YL_FileInfo.h"
#include "YL_StringUtil.h"
#include ".\dlgsys.h"

IMPLEMENT_DYNAMIC(CSysDialog, BaseDialog)

CSysDialog::CSysDialog(CWnd* pParent /*=NULL*/)
{
	CDialog(CSysDialog::IDD, pParent);
	m_minOrExit = FALSE;
	m_bNoAsk     = FALSE;
}

CSysDialog::~CSysDialog()
{
}

BEGIN_MESSAGE_MAP(CSysDialog, BaseDialog)
	ON_BN_CLICKED(IDC_CONF_CHK_AUTORUN,			OnBnClickedCheckAutoRun)
	ON_BN_CLICKED(IDC_CONF_CHK_ENABLE_BOSSKEY,	OnBnClickedChkBosskey)
	ON_BN_CLICKED(IDC_CONF_RAD_MIN,		OnBnClickedRadMin)
	ON_BN_CLICKED(IDC_CONF_RAD_EXIT,	OnBnClickedRadExit)
	ON_BN_CLICKED(IDC_CONF_CHK_NOASK,	OnBnClickedChkNoask)
END_MESSAGE_MAP()

void CSysDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_HOTKEY_BOSSKEY, m_BossKeyCtrl);
}

BOOL CSysDialog::OnInitDialog()
{
	BaseDialog::OnInitDialog();

	CRect rctNULL(0, 0, 0, 0);
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

	ILayoutMgr* pLayoutMgr = AfxGetUIManager()->UIGetLayoutMgr();
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

	pLayoutMgr->CreateControlPane( this,"ConfigDlg_SYS","normal");	
	pLayoutMgr->CreateBmpPane( this,"ConfigDlg_SYS","normal" );


	UpdateData( FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CSysDialog::SaveConf( bool bSave2File )
{
	UpdateData();
	GlobalFunc::CreateGBoxRunInStartUp( m_bAutoRun );

	bool bValue = m_minOrExit;
	AfxGetUserConfig()->SetConfigBoolValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_CONFIG_EXITCHOICE,bValue);

	bValue = m_bNoAsk;
	AfxGetUserConfig()->SetConfigBoolValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_CONFIG_IFASKEXIT,bValue);
	bValue = m_bUseBossKey;
	AfxGetUserConfig()->SetConfigBoolValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_ENABLE_BOSS_KEY, bValue );

	WORD wMod, wVK;
	m_BossKeyCtrl.GetHotKey(wVK, wMod);
	int iKeyValue = wVK;
	iKeyValue = (iKeyValue<<16) | wMod;
	AfxGetUserConfig()->SetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_BOSS_KEY_VALUE, iKeyValue);
}

void CSysDialog::LoadConf()
{
	//开机自启动	
	m_bAutoRun = GlobalFunc::IsAutoRun();
	//最小化或关闭
	AfxUserConfig*	pUserConfig = AfxGetUserConfig();
	bool bValue;
	pUserConfig->GetConfigBoolValue( CONF_SETTING_MODULE_NAME,CONF_SETTING_CONFIG_EXITCHOICE,bValue);
	m_minOrExit = bValue;

	pUserConfig->GetConfigBoolValue( CONF_SETTING_MODULE_NAME,CONF_SETTING_CONFIG_IFASKEXIT,bValue);
	m_bNoAsk = bValue;
	pUserConfig->GetConfigBoolValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_ENABLE_BOSS_KEY, bValue);
	m_bUseBossKey = bValue;

	int iKeyValue = 0;
	AfxGetUserConfig()->GetConfigIntValue(CONF_SETTING_MODULE_NAME, CONF_SETTING_BOSS_KEY_VALUE, iKeyValue);
	m_BossKeyCtrl.SetHotKey(HIWORD( *((DWORD*)&iKeyValue) ), LOWORD( *((DWORD*)&iKeyValue) ));
}

void CSysDialog::SetDefault()
{
	m_bAutoRun	= TRUE;
	m_minOrExit	= FALSE;
	m_bNoAsk	= FALSE;
	m_bUseBossKey=FALSE;
	UpdateData( FALSE );
}

bool CSysDialog::IsAllInputOK(string& strMsg)
{
	if( UpdateData() == FALSE )
		return false;
	return true;
}

HRESULT CSysDialog::CreateLink(LPCSTR lpszPathObj, LPCSTR lpszPathLink, LPCSTR lpszDesc)
{
    HRESULT hres;
    IShellLink* psl;

    hres = CoInitialize(NULL);
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID *) &psl);
    
    if (SUCCEEDED(hres)) {
        IPersistFile *ppf;

        psl->SetPath(lpszPathObj);
        psl->SetDescription(lpszDesc);
        hres = psl->QueryInterface(IID_IPersistFile, (void **)&ppf);
        if (SUCCEEDED(hres)) {
            WCHAR wsz[MAX_PATH];
            
            MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH);
            hres = ppf->Save(wsz, TRUE);
            ppf->Release();
        }
        psl->Release();
    }
    return hres;
}

void CSysDialog::OnBnClickedRadMin()
{
	m_minOrExit = FALSE;
	UpdateData(FALSE);
	SthChanged();
}

void CSysDialog::OnBnClickedRadExit()
{
	m_minOrExit = TRUE;
	UpdateData(FALSE);
	SthChanged();
}

void CSysDialog::OnBnClickedChkNoask()
{
	m_bNoAsk = !m_bNoAsk;
	UpdateData(FALSE);
	SthChanged();
}

void CSysDialog::OnBnClickedCheckAutoRun()
{
	m_bAutoRun = !m_bAutoRun;
	UpdateData(FALSE);
	SthChanged();
}

void CSysDialog::OnBnClickedChkBosskey()
{
	m_bUseBossKey = !m_bUseBossKey;
	UpdateData(FALSE);
	SthChanged();
}

BOOL CSysDialog::UpdateData(BOOL bSaveAndValidate /* = TRUE */ )
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
			m_checkedNoAsk.ShowWindow(SW_SHOW);
			m_unChkedNoAsk.ShowWindow(SW_HIDE);
		}
		else
		{
			m_checkedNoAsk.ShowWindow(SW_HIDE);
			m_unChkedNoAsk.ShowWindow(SW_SHOW);
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
	}
	else
	{	// bSaveAndValidate == TRUE
		m_bAutoRun		= m_checkedAutoRun.IsWindowVisible();
		m_bUseBossKey	= m_checkUseBossKey.IsWindowVisible();
		m_bNoAsk		= m_checkedNoAsk.IsWindowVisible();
		m_minOrExit		= m_radExit_Checked.IsWindowVisible();
	}

	return __super::UpdateData(bSaveAndValidate);
}
