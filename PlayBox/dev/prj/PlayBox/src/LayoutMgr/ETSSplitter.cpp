//------------------------------------------------------------------------------
// File    : SSplitter.cpp
// Version : 1.1
// Date    : 20. January 2004
// Author  : Alexander Atamas
// Email   : atamas@mail.univ.kiev.ua
// Web     : 
// Systems : VC6.0 (Run under Window 98, Windows Nt)
// Remarks : based on Paul DiLascia's WinMgr code
//

// 
// You are free to use/modify this code but leave this header intact.
// This class is public domain so you are free to use it any of your 
// applications (Freeware, Shareware, Commercial). 
// All I ask is that you let me know so that if you have a real winner I can
// brag to my buddies that some of my code is in your app. I also wouldn't 
// mind if you sent me a copy of your application since I like to play with
// new stuff.
//------------------------------------------------------------------------------

#include "stdafx.h"
//#include "SSTest.h"
#include "ETSSplitter.h"
#include "skinmgr.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////splitter
/////////////////////////////////////////////////////////////////////////////
// ETSFrameWnd dialog
//#include "KootingPlayer3.h"
//#include "mainfrm.h"
#define MIN_MAINWND_X	608
#define MIN_MAINWND_Y	416

#define CLOSE_STEP		1
IMPLEMENT_DYNAMIC(ETSSplitter, CStatic)

#pragma warning(disable: 4355)
ETSSplitter::ETSSplitter(LPCTSTR strName /*=NULL*/)
	//: ETSLayoutMgr( this )
{
	m_bHorizSplitter = true;
	m_bMouseButtonDownOnTheBar = false;
	m_bSlim = false;
	bTrackLeave = FALSE;
	m_pane = NULL;
	//m_nSplAbsolutePos = 0;
}
#pragma warning(default: 4355)

BEGIN_MESSAGE_MAP(ETSSplitter, CStatic)
	//{{AFX_MSG_MAP(ETSFormView)
	//ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void ETSSplitter::SetSlim(bool bSlim)
{
	m_bSlim = bSlim;
}
//
//	if(!bSlim)
//	{
//		m_bVert = m_bSplitterVert;
//		m_bHorz= m_bSplitterHorz;
//		m_bBarRight = m_bSpliterBarRight;
//		m_bBarLeft = m_bSpliterBarLeft;
//		m_bBarUp = m_bSpliterBarUp;
//		m_bBarDown = m_bSpliterBarDown;
//	}
//	else
//	{
//		m_bVert = m_bSpliterSlimVert;
//		m_bHorz= m_bSpliterSlimHorz;
//		m_bBarRight = m_bSpliterBarRight;
//		m_bBarLeft = m_bSpliterBarLeft;
//		m_bBarUp = m_bSpliterBarUp;
//		m_bBarDown = m_bSpliterBarDown;
//		this->m_bHideCloseBtn = true;
//	}
//}

void ETSSplitter::SetBitmap(CDibBitmap * vert, CDibBitmap * horz, 
							CDibBitmap *right, CDibBitmap *left, CDibBitmap *up, CDibBitmap * down, 
							CDibBitmap *right_over, CDibBitmap *left_over, CDibBitmap *up_over, CDibBitmap * down_over
							)
{
	m_bVert = vert;
	m_bHorz= horz;
	m_bBarRight = right;
	m_bBarLeft = left;
	m_bBarUp = up;
	m_bBarDown = down;

	m_bSpliterBarDown = down;
	m_bSpliterBarUp = up;
	m_bSpliterBarRight = right;
	m_bSpliterBarLeft = left;

	m_bSpliterBarDownOver = down_over;
	m_bSpliterBarUpOver = up_over;
	m_bSpliterBarRightOver = right_over;
	m_bSpliterBarLeftOver = left_over;
}
//
//void ETSSplitter::LoadSkin(CSkinMgr* skin)
//{
//	m_pSkin = skin;
//	if(m_pSkin == NULL)
//		return; 
//
//	m_bSplitterVert = m_pSkin->GetDibBmp("SpliterVertBK");
//	m_bSplitterHorz = m_pSkin->GetDibBmp("SpliterHorzBK");
//	m_bSpliterBarDown = m_pSkin->GetDibBmp("SpliterBarDown");
//	m_bSpliterBarUp = m_pSkin->GetDibBmp("SpliterBarUp");
//	m_bSpliterBarRight = m_pSkin->GetDibBmp("SpliterBarRight");
//	m_bSpliterBarLeft = m_pSkin->GetDibBmp("SpliterBarLeft");
//
//	m_bSpliterBarDownOver = m_pSkin->GetDibBmp("SpliterBarDownOver");
//	m_bSpliterBarUpOver = m_pSkin->GetDibBmp("SpliterBarUpOver");
//	m_bSpliterBarRightOver = m_pSkin->GetDibBmp("SpliterBarRightOver");
//	m_bSpliterBarLeftOver = m_pSkin->GetDibBmp("SpliterBarLeftOver");
//
//	m_bSpliterSlimVert = m_pSkin->GetDibBmp("SpliterSlimVert");
//	m_bSpliterSlimHorz = m_pSkin->GetDibBmp("SpliterSlimHorz");
//
//	SetSlim(this->m_bSlim);
//}


BOOL ETSSplitter::OnEraseBkgnd(CDC* pDC) 
{
	CWnd::OnEraseBkgnd(pDC);
	return true;
}

int  ETSSplitter::GetSplSize()
{
	int nSize = 0;
	//CSize size = m_bVert->GetDimensions();

	CSize size;
	if(!m_bHorizSplitter)
	{
		size = m_bVert->GetDimensions();
		nSize = size.cx;
	}
	else
	{
		size = m_bHorz->GetDimensions();
		nSize = size.cy;
	}

	return nSize;
}
void ETSSplitter::OnPaint()
{
	
	CPaintDC dc(this); 

	CRect rect;
	GetClientRect(rect);	
	CPoint p = rect.CenterPoint();
	bool bSplOri = 0;

	if( (!m_pane->IsClosed() && m_pane->m_nClosedOri == 0)
		||(m_pane->IsClosed() && m_pane->m_nClosedOri == 1))
		bSplOri = 0;
	else
		bSplOri = 1;
	//背景
	if(!m_bHorizSplitter)
	{
		m_bVert->SetCDibRect(rect);
		m_bVert->Draw(&dc, false);

		if(!m_bHideCloseBtn)
		{
			int h = this->m_bBarLeft->GetHeight();
			int w = this->m_bBarLeft->GetWidth();

			CRect r(p.x - w/2, p.y - h/2, p.x + w/2 + w%2 , p.y + h/2 + h%2);

			if(bSplOri == 0)
			{
				m_bBarLeft->SetCDibRect(r);
				m_bBarLeft->Draw(&dc, false);
			}
			else
			{
				m_bBarRight->SetCDibRect(r);
				m_bBarRight->Draw(&dc, false);
			}
		}
	}
	else
	{
		m_bHorz->SetCDibRect(rect);
		m_bHorz->Draw(&dc, false);

		if(!m_bHideCloseBtn)
		{
			int w = m_bBarDown->GetWidth();
			CRect r(p.x - w/2, rect.top, p.x + w/2 + 1, rect.bottom);

			if(bSplOri == 0)
			{
				m_bBarUp->SetCDibRect(r);
				m_bBarUp->Draw(&dc, false);
			}
			else
			{
				m_bBarDown->SetCDibRect(r);
				m_bBarDown->Draw(&dc, false);
			}
		}
	}
}


ETSSplitter::~ETSSplitter() 
{
	// Cleanup
}
BOOL ETSSplitter::IsMouseInCenterBar(CPoint point)
{
	CRect rc;
	this->GetClientRect(&rc);
	CPoint p = rc.CenterPoint();

	if(this->m_bHideCloseBtn)
		return false;

	if(!m_bHorizSplitter)
	{
		int h = this->m_bBarLeft->GetHeight();
		if(p.y - h/2 < point.y && p.y + h/2 > point.y)
			return true;
	}
	if(m_bHorizSplitter)
	{
		int w = this->m_bBarDown->GetWidth();
		if(p.x - w/2 < point.x && p.x + w/2 > point.x)
			return true;
	}
	return false;
}

void ETSSplitter::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_bSlim && this->IsClosed())
		return;

	if(IsMouseInCenterBar(point))
	{
		::SetCursor( AfxGetApp()->LoadStandardCursor ((LPCTSTR)MAKEINTRESOURCE(32649)));
		return;
	}
	if(IsClosed())
	{
	//	m_pane->setOpenConstrain();
	//	if(m_pane->m_nSplOpendPos == 0 && IsClosed())
		return;
	}
	if(!m_bHorizSplitter)
	{
		::SetCursor( AfxGetApp()->LoadStandardCursor ((LPCTSTR)IDC_SIZEWE));
		m_nMouseOrgPosition = point.x;
	}
	else
	{
		::SetCursor( AfxGetApp()->LoadStandardCursor ((LPCTSTR)IDC_SIZENS));
		m_nMouseOrgPosition = point.y;
	}

	m_nMouseOffset = 0;
	SetCapture();
//	::SetCursor( AfxGetApp()->LoadStandardCursor ((LPCTSTR)IDC_SIZEWE));
	m_bMouseButtonDownOnTheBar = true;
	CStatic::OnLButtonDown(nFlags, point);
}

bool ETSSplitter::IsClosed()
{
	if(m_pane == NULL)
		return true;
	return (m_pane->IsClosed() == TRUE);
}
void ETSSplitter::CloseSplitter()
{
	if(m_pane == NULL)
		return;
	if(!m_pane->IsClosed())
	{
		m_pane->setOpenConstrain();
		SetTimer(10010, 20,NULL);
	}
	else
	{
		SetTimer(10011, 20, NULL);
	}

}
void ETSSplitter::OnLButtonUp(UINT nFlags, CPoint point) 
{

	if(!m_bMouseButtonDownOnTheBar && IsMouseInCenterBar(point))
	{
		::SetCursor( AfxGetApp()->LoadStandardCursor ((LPCTSTR)MAKEINTRESOURCE(32649)));
		CloseSplitter();
		return;
	}
	m_bMouseButtonDownOnTheBar = false;
	if(IsClosed())
	{
	//	m_pane->setOpenConstrain();
	//	if(m_pane->m_nSplOpendPos == 0 && IsClosed())
		return;
	}
	if(!m_bHorizSplitter)
	{
		DrawVertBar(m_nMouseOffset);
		m_nMouseOffset = point.x - this->m_nMouseOrgPosition;
	}
	else
	{
		DrawHorizBar(m_nMouseOffset);
		m_nMouseOffset = point.y - this->m_nMouseOrgPosition;
	}

	if(m_nMouseOffset != 0)
		ConstrainOffset();

	ReleaseCapture();

	this->m_pane->resizeTo(m_nMouseOffset, true);

	CStatic::OnLButtonUp(nFlags, point);
}


void ETSSplitter::OnTimer(UINT nIDEvent)
{	
	DWORD count = ::GetTickCount();

	if(nIDEvent == 10010)
	{
		int step = m_pane->m_nSplOpendPos / CLOSE_STEP;
		int offset = 0;
		if(m_pane->m_nClosedOri == 0)
			offset = m_pane->getCloseConstrain(-step);
		else
			offset = m_pane->getCloseConstrain(step);
		if(abs(offset) <= step)
		{
			m_pane->SetClosed(true);
			KillTimer(nIDEvent);
			this->m_pane->resizeTo(offset, true);
		}
		else
			m_pane->resizeTo(offset);
		m_nMouseOffset = 0;
		TRACE("resize time eclipse %d \n", ::GetTickCount() - count);

	}
	if(nIDEvent == 10011)
	{
		int offset = 0;
		int step = m_pane->m_nSplOpendPos / CLOSE_STEP;
		if(m_pane->m_nClosedOri == 0)
			offset = m_pane->getOpenConstrain(step);
		else
			offset = m_pane->getOpenConstrain(-step);
		if(abs(offset) <= step)
		{
			m_pane->SetClosed(false);
			KillTimer(nIDEvent);
			this->m_pane->resizeTo(offset, true);
		}
		else
			m_pane->resizeTo(offset);

		m_nMouseOffset = 0;

	}
	CWnd::OnTimer(nIDEvent);

}
	
void ETSSplitter::ChangeBarBtn(BOOL bOver)
{
	if(m_bHorizSplitter)
	{
		if(bOver)
		{
			this->m_bBarDown = m_bSpliterBarDownOver;
			this->m_bBarUp = m_bSpliterBarUpOver;
		}
		else
		{
			this->m_bBarDown = m_bSpliterBarDown;
			this->m_bBarUp = m_bSpliterBarUp;
		}
	}
	else
	{
		if(bOver)
		{
			this->m_bBarLeft = m_bSpliterBarLeftOver;
			this->m_bBarRight = m_bSpliterBarRightOver;
		}
		else
		{
			this->m_bBarLeft = m_bSpliterBarLeft;
			this->m_bBarRight = m_bSpliterBarRight;
		}
	}
	this->Invalidate(true);
}
void ETSSplitter::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_bSlim && this->IsClosed())
		return;

	if (!bTrackLeave)
	{
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.hwndTrack = m_hWnd;
        tme.dwFlags = TME_LEAVE;
        _TrackMouseEvent(&tme);
        bTrackLeave = TRUE;
    }


	if(nFlags != 1 && IsMouseInCenterBar(point))
	{
		::SetCursor( AfxGetApp()->LoadStandardCursor ((LPCTSTR)MAKEINTRESOURCE(32649)));
		ChangeBarBtn(true);
		return;
	}

	if(IsClosed())
	{
	//	m_pane->setOpenConstrain();
	//	if(m_pane->m_nSplOpendPos == 0 && IsClosed())
		return;
	}
	ChangeBarBtn(false);
	if(!this->m_bHorizSplitter)
	{
		::SetCursor( AfxGetApp()->LoadStandardCursor ((LPCTSTR)IDC_SIZEWE));
		if(nFlags == 1 && m_bMouseButtonDownOnTheBar)
		{
			DrawVertBar(m_nMouseOffset);
			m_nMouseOffset = point.x - m_nMouseOrgPosition;

			if(m_nMouseOffset != 0)
				ConstrainOffset();

			DrawVertBar(m_nMouseOffset);
		}
 	}

	if(m_bHorizSplitter)
	{
		::SetCursor( AfxGetApp()->LoadStandardCursor ((LPCTSTR)IDC_SIZENS));
		if(nFlags == 1 && m_bMouseButtonDownOnTheBar)
		{
			DrawHorizBar(m_nMouseOffset);
			m_nMouseOffset = point.y - m_nMouseOrgPosition;
			if(m_nMouseOffset != 0)
				ConstrainOffset();

			DrawHorizBar(m_nMouseOffset);
 		}
	}
	
	CStatic::OnMouseMove(nFlags, point);
}

void ETSSplitter::DrawVertBar(int offset)
{
	if(offset == 0)
		return;
	CWnd* pParentWnd = GetParent();
	CWindowDC dc(pParentWnd);
	CRect rcWin;
	pParentWnd->GetWindowRect(&rcWin);	 
	
	CRect rc;
	this->GetWindowRect(&rc);
	rc -= rcWin.TopLeft();					 

	CBrush brush(GetSysColor(COLOR_3DFACE));
	CBrush* pOldBrush = dc.SelectObject(&brush);

	dc.PatBlt(
		rc.left + offset, 
		rc.top, 
		rc.Width(), 
		rc.Height(), 
		PATINVERT);

	dc.SelectObject(pOldBrush);
}


void ETSSplitter::DrawHorizBar(int offset)
{
	if(offset == 0)
		return;

	CWnd* pParentWnd = GetParent();
	CWindowDC dc(pParentWnd);
	CRect rcWin;
	pParentWnd->GetWindowRect(&rcWin);	 

	CRect rc;
	this->GetWindowRect(&rc);
	rc -= rcWin.TopLeft();	

	CBrush brush(GetSysColor(COLOR_3DFACE));
	CBrush* pOldBrush = dc.SelectObject(&brush);

	dc.PatBlt(
		rc.left, 
		rc.top + offset, 
		rc.Width(), 
		rc.Height(), 
		PATINVERT
	);

	dc.SelectObject(pOldBrush);
}

BOOL ETSSplitter::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= SS_NOTIFY; // to notify its parent of mouse events	
	return CStatic::PreCreateWindow(cs);
}

void  ETSSplitter::ConstrainOffset()
{
	m_nMouseOffset = m_pane->getOffsetConstrain(this->m_nMouseOffset);
}

LPARAM ETSSplitter::OnMouseLeave(WPARAM wp, LPARAM lp)
{
	bTrackLeave = FALSE;
	this->ChangeBarBtn(false);
    return 0;
}


void ETSOuterSplitter::CloseSplitter()
{
	CWnd *pWnd = (CWnd*)this->GetParent();
	pWnd->SetRedraw(false);

	//int min = m_pane->GetOuterPaneMinPosition();
	//m_nMinConstrainHorz = (min == 0) ? m_nMinConstrainHorz : min;


	if(!m_pane->IsClosed())
	{
		m_pane->setOpenConstrain();
		int step = m_pane->m_nSplOpendPos / CLOSE_STEP;
		//TRACE("dd%d\n", step);
		int offset = 0;
		if(m_pane->m_nClosedOri == 0)
			offset = m_pane->getCloseConstrain(-step);
		else
			offset = m_pane->getCloseConstrain(step);
		if(abs(offset) <= step)
		{
			m_pane->SetClosed(true);
			m_pane->resizeTo(offset, true);
		}
		else
			m_pane->resizeTo(offset);
	////	LoadSkin(m_pSkin);
		pWnd->SetRedraw(true);
		CRect r;
		pWnd->GetWindowRect(&r);
		r.right -= offset;
		pWnd->SetWindowPos(NULL, r.left, r.top , r.Width(), r.Height(), NULL);
		::AfxGetUIManager()->UIGetLayoutMgr()->UpdateLayout(::AfxGetMainWindow()->m_hWnd);
		//m_rootPanel->UpdateLayout(this->GetParent()->m_hWnd);
		m_nMouseOffset = 0;

	}
	else
	{
		int offset = 0;
		int step = m_pane->m_nSplOpendPos / CLOSE_STEP;
		if(step == 0)
			step = m_pane->GetOuterPaneMinPosition();

		if(m_pane->m_nClosedOri == 0)
			offset = m_pane->getOpenConstrain(step);
		else
			offset = m_pane->getOpenConstrain(-step);
		if(abs(offset) <= step)
		{
			m_pane->SetClosed(false);
			m_pane->resizeTo(offset, true);
		}
		else
			m_pane->resizeTo(offset);

////		LoadSkin(this->m_pSkin);
		pWnd->SetRedraw(true);
		m_nMouseOffset = 0;
		CRect r;
		pWnd->GetWindowRect(&r);
		r.right -= offset;
		pWnd->SetWindowPos(NULL, r.left, r.top , r.Width(), r.Height(), NULL);

	}
}

void  ETSOuterSplitter::ConstrainOffset()
{
	//最小限制
	m_nMouseOffset = m_pane->getOffsetConstrain(this->m_nMouseOffset);
	
	//最大限制
	CRect r;
	CWnd *pWnd = (CWnd*)this->GetParent();
	pWnd->GetWindowRect(&r);

	m_pane->setOpenConstrain();

	if(r.Width() - m_pane->m_nSplOpendPos + m_nMouseOffset < MIN_MAINWND_X)
		m_nMouseOffset = MIN_MAINWND_X + m_pane->m_nSplOpendPos - r.Width();
}


void ETSOuterSplitter::SetMainWndConstrain(int& minX, int& minY, LPRECT lpRect, bool redraw)
{
	if(m_pane != NULL && !m_pane->IsClosed())
	{
		m_pane->setOpenConstrain();

		CRect r;
		CWnd *pWnd = (CWnd*)this->GetParent();
		pWnd->GetWindowRect(&r);

		int delta = MIN_MAINWND_X - (lpRect->right - lpRect->left - m_pane->m_nSplOpendPos);
				
		minX = m_pane->m_nSplOpendPos + MIN_MAINWND_X;
		if(delta > 0)
		{
			if(redraw)
			{
				m_pane->ForceResizeOuterPane(delta , false);
				m_pane->setOpenConstrain();	
				minX = m_pane->m_nSplOpendPos + MIN_MAINWND_X;
			}
			else
			{
				minX = m_pane->GetOuterPaneMinPosition() + MIN_MAINWND_X;
			}
		}
		minY = MIN_MAINWND_Y;
	}
	else
	{
		minX = MIN_MAINWND_X;
		minY = MIN_MAINWND_Y;
	}
}

//void ETSOuterSplitter::LoadSkin(CSkinMgr* skin)
//{
//	m_pSkin = skin;
//	if(m_pSkin == NULL)
//		return; 
//
//	CRect r;
//	CWnd *pWnd = (CWnd*)this->GetParent();
//	pWnd->GetWindowRect(&r);
//
//	CRect r1;
//	this->GetWindowRect(&r1);
//
//	if(m_pane == NULL || !m_pane->IsClosed())
//	{
//		m_bSpliterBarRightOver = m_pSkin->GetDibBmp("SpliterOuterBarLeftOver");
//		m_bSplitterVert = m_pSkin->GetDibBmp("SpliterOuterVertOpenBK");
//		m_bSpliterBarRight = m_pSkin->GetDibBmp("SpliterOuterBarLeft");
//		m_bSpliterBarLeft = m_pSkin->GetDibBmp("SpliterOuterBarLeft");
//		m_bSpliterBarLeftOver = m_pSkin->GetDibBmp("SpliterOuterBarLeftOver");
//	}
//	else
//	{
//		m_bSpliterBarRightOver = m_pSkin->GetDibBmp("SpliterOuterBarRightOver");
//		m_bSplitterVert = m_pSkin->GetDibBmp("SpliterOuterVertBK");
//		m_bSpliterBarRight = m_pSkin->GetDibBmp("SpliterOuterBarRight");
//		m_bSpliterBarLeft = m_pSkin->GetDibBmp("SpliterOuterBarRight");
//		m_bSpliterBarLeftOver = m_pSkin->GetDibBmp("SpliterOuterBarRightOver");
//	}
//
//	SetSlim(this->m_bSlim);
//}

void ETSSplitter::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);
}