#include "stdafx.h"
#include "DlgLogin.h"
#include "./src/Core/AfxGlobals.h"
#include "../../AppConfig/config/ConfigSettingDef.h"
#include "UserManager.h"
#include "WebWnd.h"
#include "CDataManager.h"
#include "YL_Base64.h"
#include "util/md5.h"

static CRect rectNULL(0,0,0,0);

IMPLEMENT_DYNAMIC(CDlgLogin, CDialog)
CDlgLogin::CDlgLogin(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLogin::IDD, pParent)
	,m_bRememberPassChecked(false)
	,m_bAutoLoginChecked(false)
	,m_bShowErrStatic(false)
	,m_lpszPassPlaceHolder("passw0rd")
{
	m_bkg= ::AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("LoginBK");
	AfxGetMessageManager()->AttachMessage(ID_MESSAGE_USER, (IUserMsgObserver*)this);
}

CDlgLogin::~CDlgLogin()
{
	AfxGetMessageManager()->DetachMessage(ID_MESSAGE_USER, (IUserMsgObserver*)this);
}

BEGIN_MESSAGE_MAP(CDlgLogin, CDialog)
	ON_WM_PAINT()
	ON_WM_NCHITTEST()
	ON_EN_SETFOCUS(IDC_EDIT_LOGIN_USER_NAME, OnUserNameEditSetFocus)
	ON_EN_SETFOCUS(IDC_EDIT_LOGIN_PASS_WORD, OnPasswordEditSetFocus)
	ON_BN_CLICKED(ID_BTN_LOGIN_CLOSE,OnCloseClicked)
	ON_BN_CLICKED(ID_BTN_LOGIN_REG, OnRegClicked)
	ON_BN_CLICKED(ID_BTN_LOGIN_FORGET_PASS, OnForgetPassClicked)
	ON_BN_CLICKED(ID_BTN_REMEMBER_PASS_CHECK,OnRememberPassCheckClicked)
	ON_BN_CLICKED(ID_BTN_REMEMBER_PASS_UNCHECK,OnRememberPassUnCheckClicked)
	ON_BN_CLICKED(ID_BTN_AUTO_LOGIN_CHECK,OnAutoLoginCheckClicked)
	ON_BN_CLICKED(ID_BTN_AUTO_LOGIN_UNCHECK,OnAutoLoginUnCheckClicked)
	ON_BN_CLICKED(ID_BTN_LOGIN_LOGIN,OnLoginClicked)
	ON_BN_CLICKED(ID_BTN_LOGIN_CANCEL,OnCancelClicked)
END_MESSAGE_MAP()

void CDlgLogin::InitEditControl(CEditGlow* pEdit, 
								LPCSTR lpszPicPath,
								LPCSTR lpszGlowPicPath)
{
	ISkinMgr* pSkinMgr = AfxGetUIManager()->UIGetSkinMgr();
	pEdit->SetPngBorder(std::string(pSkinMgr->GetDibBmp(lpszPicPath)->GetPath()),
		std::string(pSkinMgr->GetDibBmp(lpszGlowPicPath)->GetPath()));

	pEdit->SetTextColor(pSkinMgr->GetColor("SearchInputEditText"));
	pEdit->SetBkColor(pSkinMgr->GetColor("SearchInputEditBK"));
	pEdit->LimitText(128);
	pEdit->m_bDrawBorderSelf = true;
}

#define CREATE_BTN(btn, nID) \
	btn.Create(NULL,WS_VISIBLE,rectNULL,this,nID);

// CDlgLogin 消息处理程序
BOOL CDlgLogin::OnInitDialog()
{
	CDialog::OnInitDialog();
	COLORREF color(RGB(255,0,255));
	SetWindowRgn(m_bkg->CreateRgnFromBitmap(color,this), TRUE);

	AfxGetUserConfig()->GetConfigBoolValue(CONF_SETTING_MODULE_NAME,CONF_SETTING_REMEMBER_PASSWORD,m_bRememberPassChecked);
	AfxGetUserConfig()->GetConfigBoolValue(CONF_SETTING_MODULE_NAME,CONF_SETTING_AUTO_LOGIN,m_bAutoLoginChecked);
	
	m_editUserName.Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP,rectNULL,this,IDC_EDIT_LOGIN_USER_NAME);
	m_editPassWord.Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_PASSWORD,rectNULL,this,IDC_EDIT_LOGIN_PASS_WORD);
	InitEditControl(&m_editUserName, "LoginEditBorderNormal", "LoginEditBorderGlow");
	InitEditControl(&m_editPassWord, "LoginEditBorderNormal", "LoginEditBorderGlow");

	CREATE_BTN(m_btnClose, ID_BTN_LOGIN_CLOSE);
	CREATE_BTN(m_btnLoginReg, ID_BTN_LOGIN_REG);
	CREATE_BTN(m_btnForgetPass, ID_BTN_LOGIN_FORGET_PASS);
	CREATE_BTN(m_btnRememberPassCheck, ID_BTN_REMEMBER_PASS_CHECK);
	CREATE_BTN(m_btnRememberPassUnCheck, ID_BTN_REMEMBER_PASS_UNCHECK);
	CREATE_BTN(m_btnAutoLoginCheck, ID_BTN_AUTO_LOGIN_CHECK);
	CREATE_BTN(m_btnAutoLoginUnCheck, ID_BTN_AUTO_LOGIN_UNCHECK);
	CREATE_BTN(m_btnLogin, ID_BTN_LOGIN_LOGIN);
	CREATE_BTN(m_btnCancel, ID_BTN_LOGIN_CANCEL);
	m_staticError.Create(NULL,WS_VISIBLE, CRect(0,0,0,0),this,ID_STATIC_LOGIN_ERR);
	
	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	
	pLayoutMgr->RegisterCtrl( this,"LoginBtnClose",&m_btnClose);
	pLayoutMgr->RegisterCtrl( this,"LoginEditUserName",&m_editUserName);
	pLayoutMgr->RegisterCtrl( this,"LoginEditPassWord",&m_editPassWord);
	pLayoutMgr->RegisterCtrl( this,"LoginReg",&m_btnLoginReg);
	pLayoutMgr->RegisterCtrl( this,"LoginForgetPass",&m_btnForgetPass);
	pLayoutMgr->RegisterCtrl( this,"LoginRememberPassCheck",&m_btnRememberPassCheck);
	pLayoutMgr->RegisterCtrl( this,"LoginRememberPassUnCheck",&m_btnRememberPassUnCheck);
	pLayoutMgr->RegisterCtrl( this,"LoginAutoLoginCheck",&m_btnAutoLoginCheck);
	pLayoutMgr->RegisterCtrl( this,"LoginAutoLoginUnCheck",&m_btnAutoLoginUnCheck);
	pLayoutMgr->RegisterCtrl( this,"LoginFailStatic", &m_staticError);
	pLayoutMgr->RegisterCtrl( this,"LoginLogin",&m_btnLogin);
	pLayoutMgr->RegisterCtrl( this,"LoginCancel",&m_btnCancel);

	pLayoutMgr->CreateControlPane( this,"LoginDlg","normal" );
	pLayoutMgr->CreateBmpPane( this,"LoginDlg","normal" );
	ValidateCheckBoxOrShowFailText();
	//用户名和密码填入CEdit
	std::string strUserName;
	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_LOGIN_USER_NAME, strUserName);

	std::string strBase64MD5Pass;
	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_LOGIN_PASSWORD, strBase64MD5Pass);
	if (!strUserName.empty() && 
		m_bRememberPassChecked && 
		!strBase64MD5Pass.empty())
		m_editPassWord.SetWindowText(m_lpszPassPlaceHolder);

	m_editUserName.SetWindowText(strUserName.c_str());
	m_editUserName.SetFocus();

	int l = (GetSystemMetrics(SM_CXSCREEN) - m_bkg->GetWidth())/2;
	int t = (GetSystemMetrics(SM_CYSCREEN) - m_bkg->GetHeight())/2;
	CRect wndRect(l, t, l+m_bkg->GetWidth(), t+m_bkg->GetHeight());
	MoveWindow(wndRect);
	return FALSE; //must return FALSE, so cedit can setfocus
}

void CDlgLogin::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	AfxGetUIManager()->UIGetLayoutMgr()->PaintBkGround( m_hWnd ,&dc,false );
	m_editUserName.ShowSearchEditFrame(&dc,true);
	m_editPassWord.ShowSearchEditFrame(&dc,true);
	if (m_bShowErrStatic)
		dc.FillSolidRect(1, 167, 318, 22, RGB(245, 203, 204));
}

void CDlgLogin::OnCloseClicked()
{
	CDialog::OnCancel();
}

void CDlgLogin::OnRegClicked()
{
	EndDialog(0);
	AfxGetMainWindow()->PostMessage(MSG_OPEN_REG_DIALOG, 0, 0);
}

void CDlgLogin::OnForgetPassClicked()
{
	EndDialog(0);
	AfxGetMainWindow()->PostMessage(MSG_OPEN_FORGET_PASS_DIALOG, 0, 0);
}

void CDlgLogin::OnRememberPassCheckClicked()
{
	m_bRememberPassChecked = false;
	m_bAutoLoginChecked = false;
	ValidateCheckBoxOrShowFailText();
}

void CDlgLogin::OnRememberPassUnCheckClicked()
{
	m_bRememberPassChecked = true;
	ValidateCheckBoxOrShowFailText();
}

void CDlgLogin::ValidateCheckBoxOrShowFailText()
{
	if (m_bShowErrStatic)
	{
		m_staticError.ShowWindow(SW_SHOW);
		m_btnRememberPassCheck.ShowWindow(SW_HIDE);
		m_btnRememberPassUnCheck.ShowWindow(SW_HIDE);
		m_btnAutoLoginCheck.ShowWindow(SW_HIDE);
		m_btnAutoLoginUnCheck.ShowWindow(SW_HIDE);
	}else
	{
		m_staticError.ShowWindow(SW_HIDE);
		m_btnRememberPassCheck.ShowWindow(m_bRememberPassChecked ? SW_SHOW : SW_HIDE);
		m_btnRememberPassUnCheck.ShowWindow(!m_bRememberPassChecked ? SW_SHOW : SW_HIDE);
		m_btnAutoLoginCheck.ShowWindow(m_bAutoLoginChecked ? SW_SHOW : SW_HIDE);
		m_btnAutoLoginUnCheck.ShowWindow(!m_bAutoLoginChecked ? SW_SHOW : SW_HIDE);
	}
	Invalidate();
}

void CDlgLogin::OnAutoLoginCheckClicked()
{
	m_bAutoLoginChecked = false;
	ValidateCheckBoxOrShowFailText();
}

void CDlgLogin::OnAutoLoginUnCheckClicked()
{
	m_bAutoLoginChecked = true;
	m_bRememberPassChecked = true;
	ValidateCheckBoxOrShowFailText();
}

void CDlgLogin::OnLoginClicked()
{
	CString cstrUserName;
	CString cstrPassWord;
	m_editUserName.GetWindowText(cstrUserName);
	m_editPassWord.GetWindowText(cstrPassWord);
	if (cstrUserName.GetLength() == 0)
	{
		ShowErrStaticAndLaterDisappear("请输入用户名");
		return;
	}
	if (cstrPassWord.GetLength() == 0) 
	{
		ShowErrStaticAndLaterDisappear("请输入密码");
		return;
	}
	WriteConf();
	CUserManager::GetInstance()->AddTask(m_task);
	CUserManager::GetInstance()->AddTask(m_collectedWebGame);

	if (cstrPassWord.Compare(m_lpszPassPlaceHolder) == 0)
	{
		std::string strBase64MD5Pass;
		AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
			CONF_SETTING_LOGIN_PASSWORD, strBase64MD5Pass);
		char szMD5[256];
		ZeroMemory(szMD5, 256);
		YL_Base64Decode(szMD5, strBase64MD5Pass.c_str());
		CUserManager::GetInstance()->User_Login(cstrUserName, szMD5, true);
	}else
		CUserManager::GetInstance()->User_Login(cstrUserName, cstrPassWord);
}

void CDlgLogin::OnCancelClicked()
{
	CUserManager::GetInstance()->User_CancelLog();
	CDialog::OnCancel();
}

void CDlgLogin::UserMsg_Login()
{
	PostMessage(WM_CLOSE, 0, 0);
}

void CDlgLogin::UserMsg_LogOut()
{
	PostMessage(WM_CLOSE, 0, 0);
}

void CDlgLogin::UserMsg_LogFaild()
{
	ShowErrStaticAndLaterDisappear(_T("登陆失败"));
}

void CDlgLogin::UserMsg_BeginLogin()
{
	ShowErrStaticAndLaterDisappear(_T("登陆中..."));
}

void CDlgLogin::AddTask(const TAB_ITEM& ti)
{
	if (ti.enumType != TAB_UNKNOWN)
		m_task = ti;
}

void CDlgLogin::AddTask(const OneGame& g)
{
	if (g.nGameType != 0)
		m_collectedWebGame = g;
}

void CDlgLogin::ShowErrStaticAndLaterDisappear(LPCTSTR lpszText)
{
	m_bShowErrStatic = true;
	m_staticError.SetWindowText(lpszText);
	ValidateCheckBoxOrShowFailText();
}

void CDlgLogin::HideErrStatic()
{
	if (!m_bShowErrStatic)
		return;

	m_bShowErrStatic = false;
	m_staticError.SetWindowText(_T(""));
	ValidateCheckBoxOrShowFailText();	
}

void CDlgLogin::OnUserNameEditSetFocus()
{
	HideErrStatic();
}

void CDlgLogin::OnPasswordEditSetFocus()
{
	HideErrStatic();
}

void CDlgLogin::WriteConf()
{
	AfxGetUserConfig()->SetConfigBoolValue(CONF_SETTING_MODULE_NAME,CONF_SETTING_REMEMBER_PASSWORD,m_bRememberPassChecked);
	AfxGetUserConfig()->SetConfigBoolValue(CONF_SETTING_MODULE_NAME,CONF_SETTING_AUTO_LOGIN,m_bAutoLoginChecked);
	CString cstrUserName;
	m_editUserName.GetWindowText(cstrUserName);
	AfxGetUserConfig()->SetConfigStringValue(CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_LOGIN_USER_NAME, std::string(cstrUserName), true);

	char szBase64MD5[256];
	ZeroMemory(szBase64MD5, 256);
	if (m_bRememberPassChecked)
	{
		CString cstrPassWord;
		m_editPassWord.GetWindowText(cstrPassWord);
		if (cstrPassWord.Compare(m_lpszPassPlaceHolder) == 0)//用户没有修改密码
		{
			std::string strConf;
			AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
				CONF_SETTING_LOGIN_PASSWORD, strConf);
			strcpy(szBase64MD5, strConf.c_str());
		}else//用户重新输入了密码
		{
			char lpszPass[256];
			ZeroMemory(lpszPass, 256);
			strcpy(lpszPass, cstrPassWord.GetBuffer());

			char szPwdMD5[33] = {0};
			MD5String(lpszPass, szPwdMD5);
			YL_Base64Encode(szBase64MD5, szPwdMD5, strlen(szPwdMD5));
		}
	}
	AfxGetUserConfig()->SetConfigStringValue(CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_LOGIN_PASSWORD, std::string(szBase64MD5), true);
}

void CDlgLogin::OnOK()
{
	OnLoginClicked();
}

UINT CDlgLogin::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	rc.bottom = rc.top + 80;
	return rc.PtInRect(point) ? HTCAPTION : CDialog::OnNcHitTest(point);
}