#include "stdafx.h"
#include "resource.h"
#include "UserLogedOutPanelWnd.h"
#include "../../gui/util/CBufferDC.h"
#include "../../Gui/CommonControl/xSkinButton.h"
#include "WebWnd.h"
#include "../../AppConfig/config/ConfigAppDef.h"

IMPLEMENT_DYNAMIC(CUserLogedOutWnd, CBasicWnd)
CUserLogedOutWnd::CUserLogedOutWnd()
{
	m_pBtnLogin = new CxSkinButton();
	m_pBtnRegister = new CxSkinButton();
}

CUserLogedOutWnd::~CUserLogedOutWnd()
{
	delete m_pBtnLogin;
	delete m_pBtnRegister;
}

BEGIN_MESSAGE_MAP(CUserLogedOutWnd, CBasicWnd)
	ON_WM_CREATE()
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

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->RegisterCtrl(this,"UserManLogin",m_pBtnLogin);
	pLayoutMgr->RegisterCtrl(this,"UserManRegister",m_pBtnRegister);
	pLayoutMgr->CreateControlPane( this,"usermanlogedoutpanel","normal");
	pLayoutMgr->CreateBmpPane( this,"usermanlogedoutpanel","normal" );
	return 0;
}

void CUserLogedOutWnd::OnClickedLogin()
{

}

void CUserLogedOutWnd::OnClickedRegister()
{
	// 获取注册地址
	string strUrl;
	AfxGetUserConfig()->GetConfigStringValue(CONF_APP_MODULE_NAME, CONF_APP_REGIST_URL, strUrl);

	// 获取注册页面大小
	CRect rctPageSize(0, 0, 518, 298);

	CWebDlg dlg(this);
	AfxGetUIManager()->UIAddDialog(&dlg);
	dlg.DoModal(_T("注册"), strUrl.c_str(), rctPageSize.Width(), rctPageSize.Height());
	AfxGetUIManager()->UIRemoveDialog(&dlg);
}