#include "stdafx.h"
#include "BasicWnd.h"

IMPLEMENT_DYNAMIC(CBasicWnd, CWnd)
CBasicWnd::CBasicWnd()
{
}

CBasicWnd::~CBasicWnd()
{
}


BEGIN_MESSAGE_MAP(CBasicWnd, CWnd)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPassMouseWnd 消息处理程序

void CBasicWnd::OnLButtonDown(UINT nFlags, CPoint point)
{	
	CWnd * pParent = GetParent();

	pParent = AfxGetMainWindow();

	WPARAM wParam;
	LPARAM lParam;

	ClientToScreen( &point);
	::ScreenToClient( pParent->m_hWnd,&point );

	wParam = nFlags;
	lParam = (WORD)point.x;
	lParam += ((WORD)point.y) << 16;		

//	::PostMessage( AfxGetMainWindow()->m_hWnd,WM_NCLBUTTONDOWN, HTCAPTION, lParam );	

	::SendMessage(pParent->m_hWnd,WM_LBUTTONDOWN, wParam,lParam );	
	__super::OnLButtonDown(nFlags, point);
}

void CBasicWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd * pParent = GetParent();

	pParent = AfxGetMainWindow();

	WPARAM wParam;
	LPARAM lParam;

	ClientToScreen( &point);
	::ScreenToClient( pParent->m_hWnd,&point );

	wParam = nFlags;
	lParam = (WORD)point.x;
	lParam += ((WORD)point.y) << 16;		

	::SendMessage(pParent->m_hWnd,WM_LBUTTONUP, wParam,lParam );	

	__super::OnLButtonUp(nFlags, point);
}

void CBasicWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd * pParent = GetParent();

	pParent = AfxGetMainWindow();

	WPARAM wParam;
	LPARAM lParam;

	ClientToScreen( &point);
	::ScreenToClient( pParent->m_hWnd,&point );

	wParam = nFlags;
	lParam = (WORD)point.x;
	lParam += ((WORD)point.y) << 16;		

	::SendMessage(pParent->m_hWnd, WM_MOUSEMOVE,wParam,lParam );	

	__super::OnMouseMove(nFlags, point);
}

void CBasicWnd::OnPaint()
{
	CPaintDC dc(this);
	AfxGetUIManager()->UIGetLayoutMgr()->PaintBkGround( m_hWnd,&dc,true );
}
void CBasicWnd::OnSize(UINT nType, int cx, int cy)
{
	AfxGetUIManager()->UIGetLayoutMgr()->UpdateLayout( m_hWnd );
}