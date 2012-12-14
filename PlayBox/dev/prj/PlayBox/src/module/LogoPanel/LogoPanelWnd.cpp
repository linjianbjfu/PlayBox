#include "stdafx.h"
#include "LogoPanelWnd.h"
#include "../../gui/util/CBufferDC.h"

IMPLEMENT_DYNAMIC(CLogoPanelWnd, CBasicWnd)
CLogoPanelWnd::CLogoPanelWnd()
{
}

CLogoPanelWnd::~CLogoPanelWnd()
{
}

BEGIN_MESSAGE_MAP(CLogoPanelWnd, CBasicWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()


int CLogoPanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->CreateControlPane( this,"logopanel","normal");
	pLayoutMgr->CreateBmpPane( this,"logopanel","normal" );
	return 0;
}

void CLogoPanelWnd::LoadSkin()
{
}