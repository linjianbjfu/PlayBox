#include "stdafx.h"
#include "UserLogedInPanelWnd.h"
#include "../../gui/util/CBufferDC.h"
#include "../UserMan/UserInfo.h"
#include "xSkinButton.h"
#include "resource.h"
#include "UserManager.h"

IMPLEMENT_DYNAMIC(CUserLogedInWnd, CBasicWnd)
CUserLogedInWnd::CUserLogedInWnd() : m_pUserInfo(new UserInfo()) 
{
	//×ÖÌå
	LOGFONT  lf;
	ZeroMemory(&lf,sizeof(lf));

	lf.lfHeight = 12;
	lf.lfWidth  = 0;
	lf.lfCharSet = GB2312_CHARSET;
	lf.lfStrikeOut = FALSE;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = FF_DONTCARE | DEFAULT_PITCH;
	lf.lfUnderline = false;
	strcpy(lf.lfFaceName, "ËÎÌå");

	lf.lfWeight = FW_NORMAL;
	m_font.CreateFontIndirect(&lf);
	m_pBtnLogout = new CxSkinButton();
}

CUserLogedInWnd::~CUserLogedInWnd() 
{
	m_font.DeleteObject();
	delete m_pUserInfo;
	delete m_pBtnLogout;
}

BEGIN_MESSAGE_MAP(CUserLogedInWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_BN_CLICKED(ID_BTN_USER_MAN_LOGOUT,OnClickedLogout)
END_MESSAGE_MAP()

int CUserLogedInWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectNULL(0,0,0,0);
	m_pBtnLogout->Create(NULL,NULL,rectNULL,this,ID_BTN_USER_MAN_LOGOUT);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->RegisterCtrl(this,"UserManLogout",m_pBtnLogout);
	pLayoutMgr->CreateControlPane( this,"usermanlogedinpanel","normal");
	pLayoutMgr->CreateBmpPane( this,"usermanlogedinpanel","normal" );
	return 0;
}
void CUserLogedInWnd::OnPaint()
{
	CPaintDC dc(this);
	AfxGetUIManager()->UIGetLayoutMgr()->PaintBkGround(m_hWnd, &dc);
	if (m_pUserInfo->strName.empty())
		return;
	CRect rcInfo(80, 20, 260, 40);
	dc.DrawText( m_pUserInfo->strName.c_str(), 
		-1, &rcInfo, DT_NOPREFIX|DT_LEFT|DT_VCENTER|DT_SINGLELINE);
}

void CUserLogedInWnd::SetUserInfo(UserInfo* pUserInfo)
{
	if (pUserInfo)
		m_pUserInfo->strName = pUserInfo->strName;
}

void CUserLogedInWnd::OnClickedLogout()
{
	CUserManager::GetInstance()->User_Logout();
}