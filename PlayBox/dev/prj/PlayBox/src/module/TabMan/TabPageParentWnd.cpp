#include "stdafx.h"
#include "TabPageparentWnd.h"
#include "TabPageControl.h"
#include "CDataManager.h"

IMPLEMENT_DYNAMIC(CTabPageParentWnd, CBasicWnd)
CTabPageParentWnd::CTabPageParentWnd()
{
}

CTabPageParentWnd::~CTabPageParentWnd()
{
	CTabPageControl::DelInstance();
}

BEGIN_MESSAGE_MAP(CTabPageParentWnd, CBasicWnd)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


int CTabPageParentWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->CreateBmpPane( this,"tabpanelparent","normal" );
	CTabPageControl::GetInstance()->SetParentWnd(this);
	return 0;
}

void CTabPageParentWnd::OnSize(UINT nType, int cx, int cy)
{
	CTabPageControl::GetInstance()->ResizePage();
	__super::OnSize(nType,cx,cy);
}

BOOL CTabPageParentWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}