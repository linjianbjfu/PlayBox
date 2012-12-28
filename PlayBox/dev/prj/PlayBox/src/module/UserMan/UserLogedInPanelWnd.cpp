#include "stdafx.h"
#include "UserLogedInPanelWnd.h"
#include "../../gui/util/CBufferDC.h"

IMPLEMENT_DYNAMIC(CUserLogedInWnd, CBasicWnd)
CUserLogedInWnd::CUserLogedInWnd()
{
}

CUserLogedInWnd::~CUserLogedInWnd()
{
}

BEGIN_MESSAGE_MAP(CUserLogedInWnd, CBasicWnd)
	ON_WM_CREATE()
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
