#include "stdafx.h"
#include "resource.h"
#include "UserLogedOutPanelWnd.h"
#include "../../gui/util/CBufferDC.h"
#include "../../Gui/CommonControl/xSkinButton.h"
#include "WebWnd.h"
#include "../../AppConfig/config/ConfigAppDef.h"
#include "DlgLogin.h"
#include "UserManager.h"

#define WM_AUTO_LOGIN 1

IMPLEMENT_DYNAMIC(CUserLogedOutWnd, CBasicWnd)
CUserLogedOutWnd::CUserLogedOutWnd()
{
	m_pBtnLogin = new CxSkinButton();
	m_pBtnRegister = new CxSkinButton();
	AfxGetMessageManager()->AttachMessage(ID_MESSAGE_USER, (IUserMsgObserver*)this);
}

CUserLogedOutWnd::~CUserLogedOutWnd()
{
	delete m_pBtnLogin;
	delete m_pBtnRegister;
	AfxGetMessageManager()->DetachMessage(ID_MESSAGE_USER, (IUserMsgObserver*)this);
}

BEGIN_MESSAGE_MAP(CUserLogedOutWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_BN_CLICKED(ID_BTN_USER_MAN_LOGIN,OnClickedLogin)
	ON_BN_CLICKED(ID_BTN_USER_MAN_REGISTER,OnClickedRegister)
END_MESSAGE_MAP()


int CUserLogedOutWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectNULL(0,0,0,0);
	m_pBtnLogin->Create(NULL,NULL,rectNULL,this,ID_BTN_USER_MAN_LOGIN);
	m_pBtnRegister->Create(NULL,NULL,rectNULL,this,ID_BTN_USER_MAN_REGISTER);
	m_staticError.Create(NULL,WS_VISIBLE, CRect(0,0,0,0),this,ID_STATIC_LOGEDOUT_LOGIN_ERR);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->RegisterCtrl(this,"UserManLogin",m_pBtnLogin);
	pLayoutMgr->RegisterCtrl(this,"UserManRegister",m_pBtnRegister);
	pLayoutMgr->RegisterCtrl(this,"UserManFailText",&m_staticError);
	pLayoutMgr->CreateControlPane( this,"usermanlogedoutpanel","normal");
	pLayoutMgr->CreateBmpPane( this,"usermanlogedoutpanel","normal" );

	SetTimer(WM_AUTO_LOGIN, 50, NULL);
	return 0;
}

void CUserLogedOutWnd::OnClickedLogin()
{
	CDlgLogin dlgLogin;
	dlgLogin.DoModal();
}

void CUserLogedOutWnd::OnClickedRegister()
{
	AfxGetMainWindow()->PostMessage(MSG_OPEN_REG_DIALOG, 0, 0);
}

void CUserLogedOutWnd::UserMsg_Login()
{
}

void CUserLogedOutWnd::UserMsg_LogOut()
{
}

void CUserLogedOutWnd::UserMsg_LogFaild()
{
}

void CUserLogedOutWnd::UserMsg_BeginLogin()
{
}

void CUserLogedOutWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == WM_AUTO_LOGIN)
	{
		KillTimer(nIDEvent);
		CUserManager::GetInstance()->User_AppStartUp();
	}
	__super::OnTimer(nIDEvent);
}