#include "stdafx.h"
#include "BottomPanelWnd.h"
#include "../../gui/util/CBufferDC.h"

IMPLEMENT_DYNAMIC(CBottomPanelWnd, CBasicWnd)
CBottomPanelWnd::CBottomPanelWnd()
{
}

CBottomPanelWnd::~CBottomPanelWnd()
{
}

BEGIN_MESSAGE_MAP(CBottomPanelWnd, CBasicWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()


int CBottomPanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->CreateControlPane( this,"bottompanel","normal");
	pLayoutMgr->CreateBmpPane( this,"bottompanel","normal" );
	return 0;
}

void CBottomPanelWnd::LoadSkin()
{
}