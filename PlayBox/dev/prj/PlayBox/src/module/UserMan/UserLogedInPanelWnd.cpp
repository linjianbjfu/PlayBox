#include "stdafx.h"
#include "UserLogedInPanelWnd.h"
#include "../../gui/util/CBufferDC.h"
#include ".\userlogedinpanelwnd.h"

IMPLEMENT_DYNAMIC(CUserLogedInWnd, CBasicWnd)
CUserLogedInWnd::CUserLogedInWnd()
{
}

CUserLogedInWnd::~CUserLogedInWnd()
{
}

BEGIN_MESSAGE_MAP(CUserLogedInWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

int CUserLogedInWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->CreateControlPane( this,"usermanlogedinpanel","normal");
	pLayoutMgr->CreateBmpPane( this,"usermanlogedinpanel","normal" );
	return 0;
}
void CUserLogedInWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	AfxGetUIManager()->UIOnPaint(&dc);
}
