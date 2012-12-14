#include "stdafx.h"
#include "FlashloadingWnd.h"
#include "../../Global/GlobalSwfPath.h"


IMPLEMENT_DYNAMIC(CFlashWnd, CWnd)
CFlashWnd::CFlashWnd()
{
	m_colBk = RGB( 39, 39, 39 );
	//netloadflash大小为134*120，可等比例放大缩小
	m_rcFlash = CRect (0, 0, 558, 283);
}

CFlashWnd::~CFlashWnd()
{
}

BEGIN_MESSAGE_MAP(CFlashWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int CFlashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( __super::OnCreate( lpCreateStruct) == -1 )
	{
		return -1;
	}
	m_flash.Create("WebInfo", WS_CHILD, CRect(0,0,0,0), this, 12456);
	m_flash.ShowWindow(SW_SHOW);
	m_flash.SetBGColor( 0 );
	m_flash.SetMovie(GlobalSwfPath::Net_Loading());
	return 1;
}

void CFlashWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetClientRect(rect);	
	CBrush brush( m_colBk );
	CBrush* pOldBrush = dc.SelectObject(&brush);
	dc.FillRect(rect,&brush);
	dc.SelectObject(pOldBrush);
	__super::OnPaint();
}

BOOL CFlashWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CFlashWnd::OnSize(UINT nType, int cx, int cy)
{	
	int ix = cx/2-m_rcFlash.Width()/2;
	int iy = cy/2-m_rcFlash.Height()/2;

	m_flash.MoveWindow( ix, iy, m_rcFlash.Width(), m_rcFlash.Height());
	__super::OnSize(nType,cx,cy);
}