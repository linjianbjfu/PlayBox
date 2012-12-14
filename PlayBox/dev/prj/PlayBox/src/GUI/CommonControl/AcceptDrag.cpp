#include "StdAfx.h"
#include ".\acceptdrag.h"


IMPLEMENT_DYNAMIC(CAcceptDrag,CObject)

CAcceptDrag::CAcceptDrag(void)
{
}

CAcceptDrag::~CAcceptDrag(void)
{
}

void CAcceptDrag::DropDown( CWnd* pSrc,CWnd* pDes,void* pp)
{
	::SendMessage(pDes->m_hWnd,WM_DROP_MSG,(WPARAM)pSrc,(LPARAM)pp);
}
