#include "stdafx.h"
#include "DlgLogin.h"
#include "./src/Core/AfxGlobals.h"
#include "../../AppConfig/config/ConfigSettingDef.h"
#include "UserManager.h"

static CRect rectNULL(0,0,0,0);

IMPLEMENT_DYNAMIC(CDlgLogin, CDialog)
CDlgLogin::CDlgLogin(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLogin::IDD, pParent)
	,m_bRememberPassChecked(false)
	,m_bAutoLoginChecked(false)
{
	m_editFont.CreateFont(14,      // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"宋体");
	m_bkg= ::AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("LoginBK");
}

CDlgLogin::~CDlgLogin()
{
}

BEGIN_MESSAGE_MAP(CDlgLogin, CDialog)
	ON_WM_PAINT()
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

void CDlgLogin::InitEditControl(CEdit* pEdit, UINT nID)
{
	pEdit->Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP,rectNULL,this,nID);
	pEdit->SetFont(&m_editFont);
	pEdit->SetMargins(3, 0);
	pEdit->ModifyStyle(ES_MULTILINE, 0);
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
	
	InitEditControl(&m_editUserName, IDC_EDIT_LOGIN_USER_NAME);
	InitEditControl(&m_editPassWord, IDC_EDIT_LOGIN_PASS_WORD);

	CREATE_BTN(m_btnClose, ID_BTN_LOGIN_CLOSE);
	CREATE_BTN(m_btnLoginReg, ID_BTN_LOGIN_REG);
	CREATE_BTN(m_btnForgetPass, ID_BTN_LOGIN_FORGET_PASS);
	CREATE_BTN(m_btnRememberPassCheck, ID_BTN_REMEMBER_PASS_CHECK);
	CREATE_BTN(m_btnRememberPassUnCheck, ID_BTN_REMEMBER_PASS_UNCHECK);
	CREATE_BTN(m_btnAutoLoginCheck, ID_BTN_AUTO_LOGIN_CHECK);
	CREATE_BTN(m_btnAutoLoginUnCheck, ID_BTN_AUTO_LOGIN_UNCHECK);
	CREATE_BTN(m_btnLogin, ID_BTN_LOGIN_LOGIN);
	CREATE_BTN(m_btnCancel, ID_BTN_LOGIN_CANCEL);
	
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
	pLayoutMgr->RegisterCtrl( this,"LoginLogin",&m_btnLogin);
	pLayoutMgr->RegisterCtrl( this,"LoginCancel",&m_btnCancel);

	pLayoutMgr->CreateControlPane( this,"LoginDlg","normal" );
	pLayoutMgr->CreateBmpPane( this,"LoginDlg","normal" );

	ValidateCheckBox();

	int l = (GetSystemMetrics(SM_CXSCREEN) - m_bkg->GetWidth())/2;
	int t = (GetSystemMetrics(SM_CYSCREEN) - m_bkg->GetHeight())/2;
	CRect wndRect(l, t, l+m_bkg->GetWidth(), t+m_bkg->GetHeight());
	MoveWindow(wndRect);
	return TRUE;
}

void CDlgLogin::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	AfxGetUIManager()->UIGetLayoutMgr()->PaintBkGround( m_hWnd ,&dc,false ) ;	
}

void CDlgLogin::OnCloseClicked()
{
	CDialog::OnCancel();
}

void CDlgLogin::OnRegClicked()
{
	//打开注册的commondlg
}

void CDlgLogin::OnForgetPassClicked()
{
	//打开找回密码的页面
}

void CDlgLogin::OnRememberPassCheckClicked()
{
	m_bRememberPassChecked = false;
	ValidateCheckBox();
}

void CDlgLogin::OnRememberPassUnCheckClicked()
{
	m_bRememberPassChecked = true;
	ValidateCheckBox();
}

void CDlgLogin::ValidateCheckBox()
{
	m_btnRememberPassCheck.ShowWindow(m_bRememberPassChecked ? SW_SHOW : SW_HIDE);
	m_btnRememberPassUnCheck.ShowWindow(!m_bRememberPassChecked ? SW_SHOW : SW_HIDE);

	m_btnAutoLoginCheck.ShowWindow(m_bAutoLoginChecked ? SW_SHOW : SW_HIDE);
	m_btnAutoLoginUnCheck.ShowWindow(!m_bAutoLoginChecked ? SW_SHOW : SW_HIDE);
}

void CDlgLogin::OnAutoLoginCheckClicked()
{
	m_bAutoLoginChecked = false;
	ValidateCheckBox();
}

void CDlgLogin::OnAutoLoginUnCheckClicked()
{
	m_bAutoLoginChecked = true;
	ValidateCheckBox();
}

void CDlgLogin::OnLoginClicked()
{
	CString cstrUserName;
	CString cstrPassWord;
	m_editUserName.GetWindowText(cstrUserName);
	m_editPassWord.GetWindowText(cstrPassWord);
	if (cstrUserName.GetLength() == 0
		|| cstrPassWord.GetLength() == 0) 
	{
		//tooltip 
		return;
	}
	CUserManager::GetInstance()->User_Login(cstrUserName, cstrPassWord);
}

void CDlgLogin::OnCancelClicked()
{
	CUserManager::GetInstance()->User_CancelLog();
	CDialog::OnCancel();
}
