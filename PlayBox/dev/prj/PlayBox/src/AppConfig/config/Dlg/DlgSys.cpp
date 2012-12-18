#include "stdafx.h"
#include "DlgSys.h"
#include <io.h>
#include "./src/Core/AfxGlobals.h"
#include "../ConfigSettingDef.h"
#include "YL_FileInfo.h"
#include "YL_StringUtil.h"

IMPLEMENT_DYNAMIC(CSysDialog, BaseDialog)

CSysDialog::CSysDialog(CWnd* pParent /*=NULL*/)
: m_bUseBossKey(FALSE)
{
	CDialog(CSysDialog::IDD, pParent);
	m_minOrExit = FALSE;
	m_noAsk     = FALSE;
}

CSysDialog::~CSysDialog()
{
}

BEGIN_MESSAGE_MAP(CSysDialog, BaseDialog)
	ON_BN_CLICKED(IDC_RAD_MIN, OnBnClickedRadMin)
	ON_BN_CLICKED(IDC_RAD_EXIT, OnBnClickedRadExit)
	ON_BN_CLICKED(IDC_CHK_NOASK, OnBnClickedChkNoask)
	ON_BN_CLICKED(IDC_CHECK_AUTORUN, OnBnClickedCheckAutoRun)
	ON_BN_CLICKED(IDC_CHK_BOSSKEY, OnBnClickedChkBosskey)
END_MESSAGE_MAP()

void CSysDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Check(pDX, IDC_CHECK_AUTORUN, m_bAutoRun);
	DDX_Check(pDX, IDC_CHK_NOASK, m_noAsk);
	DDX_Control(pDX, IDC_CHECK_AUTORUN, m_checkAutoRun);
	DDX_Radio(pDX, IDC_RAD_MIN, m_minOrExit);
	DDX_Control(pDX, IDC_RAD_MIN, m_minCtrl);
	DDX_Control(pDX, IDC_RAD_EXIT, m_exitCtrl);
	DDX_Control(pDX, IDC_CHK_NOASK, m_noAskCtrl);
	DDX_Control(pDX, IDC_STATIC_MINOREXIT, m_staticMinOrExit);
	DDX_Check(pDX, IDC_CHK_BOSSKEY, m_bUseBossKey);
	DDX_Control(pDX, IDC_HOTKEY_BOSSKEY, m_BossKeyCtrl);
}

void CSysDialog::SaveConf( bool bSave2File )
{
	UpdateData();
	GlobalFunc::CreateGBoxRunInStartUp( m_bAutoRun );

	bool bValue = m_minOrExit;
	AfxGetUserConfig()->SetConfigBoolValue( CONF_SETTING_MODULE_NAME,CONF_SETTING_CONFIG_EXITCHOICE,bValue);

	bValue = m_noAsk;
	AfxGetUserConfig()->SetConfigBoolValue( CONF_SETTING_MODULE_NAME,CONF_SETTING_CONFIG_IFASKEXIT,bValue);
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
	m_noAsk = bValue;
	pUserConfig->GetConfigBoolValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_ENABLE_BOSS_KEY, bValue);
	m_bUseBossKey = bValue;
	m_BossKeyCtrl.EnableWindow(m_bUseBossKey);

	int iKeyValue;
	pUserConfig->GetConfigIntValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_BOSS_KEY_VALUE, iKeyValue);
	m_BossKeyCtrl.SetHotKey(HIWORD(*((DWORD*)&iKeyValue)), LOWORD(*((DWORD*)&iKeyValue)) );
}

void CSysDialog::SetDefault()
{
	m_bAutoRun	= TRUE;
	m_minOrExit	= FALSE;
	m_noAsk		= FALSE;
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
	SthChanged();
}

void CSysDialog::OnBnClickedRadExit()
{
	SthChanged();
}

void CSysDialog::OnBnClickedChkNoask()
{
	SthChanged();
}

void CSysDialog::OnBnClickedCheckAutoRun()
{
	SthChanged();
}

void CSysDialog::OnBnClickedChkBosskey()
{
	SthChanged();
	UpdateData();
	m_BossKeyCtrl.EnableWindow(m_bUseBossKey);
}