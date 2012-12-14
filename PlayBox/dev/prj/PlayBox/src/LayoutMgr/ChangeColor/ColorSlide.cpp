
#include "stdafx.h"
#include "colorslide.h"
#include "BitMapOperate.h"
#include "WndColorMgr.h"
#include "IEvent.h"
extern CBitMapOperate gBitMapOperate;
extern CWndColorMgr   gWndColor;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CColorSlide::CColorSlide()
{
	FPosition = 0;
	FMaxValue = 100;
	FMinValue = 0;

	m_iStatePosX=0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_bButtonDown = FALSE;
	mWndProc = NULL;

	m_SlideType = HUE_SLIDE;

	m_colBegain = 0;
	m_colEnd    = 0;
	m_nHue      = 0;
}

CColorSlide::~CColorSlide()
{
}

BEGIN_MESSAGE_MAP(CColorSlide, CWnd)
	//{{AFX_MSG_MAP(CColorSlide)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CColorSlide::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

#pragma warning(disable:4311)
#pragma warning(disable:4312)

void CColorSlide::PreSubclassWindow() 
{
	/*m_ctlTips.Create(this);
	m_ctlTips.DelTool(this);
	m_ctlTips.AddTool(this);
	m_ctlTips.SetDelayTime(250);*/

	mWndProc = (WNDPROC)SetWindowLong(this->GetSafeHwnd(), GWL_WNDPROC, (LONG)SelfProc);
	CWnd::PreSubclassWindow();
}

LRESULT CALLBACK CColorSlide::SelfProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LONG lReturn = 0;
	CColorSlide* lpThis = NULL;
	lpThis = (CColorSlide*)CWnd::FromHandle(hWnd);
	if (lpThis == NULL || !::IsWindow(lpThis->GetSafeHwnd()))
		return lReturn;

	switch (uMsg)
	{
	case WM_PAINT:
	case WM_NCPAINT:
		{
			lReturn = (LONG)::DefWindowProc(hWnd, uMsg, wParam, lParam);
			lpThis->DynamicStat(set_Normal);
			return lReturn;
		}
		break;
	case WM_ENABLE:
		lpThis->DynamicStat(set_Normal);
		break;
	case WM_ACTIVATE:
		lpThis->DynamicStat(set_Normal);
		break;
	case WM_SIZE:
		lpThis->SaveSize();
		lpThis->SetPos(lpThis->FPosition);

		break;
	}
	// 调用原来的回调函数
	lReturn = (LONG)CallWindowProc(lpThis->mWndProc, hWnd, uMsg, wParam, lParam);
	return lReturn;
}

void CColorSlide::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_SlideType == PERCENT_SLIDE )
	return CSliderCtrl::OnLButtonDown(nFlags, point);
	m_bButtonDown = TRUE;
	SetCapture();
	CWnd *wndUnderMouse = WindowFromPoint(point);

	if(( point.x-TRACKBAR_INTERVAL >m_iStatePosX ) &&( point.x-TRACKBAR_INTERVAL< m_iStatePosX ))
	{
		return CSliderCtrl::OnLButtonDown(nFlags, point);
	}

	m_iStatePosX = point.x;

	if (m_iStatePosX < TRACKBAR_INTERVAL) m_iStatePosX = 0;
	else if (m_iStatePosX > m_nWidth-2*TRACKBAR_INTERVAL ) m_iStatePosX = m_nWidth-2*TRACKBAR_INTERVAL ;
	else
	{
		m_iStatePosX -= TRACKBAR_INTERVAL;
	}

	FPosition =  (int)( FMaxValue * (float)(m_iStatePosX)/(float)(m_nWidthPlay) + 0.5 );
	DynamicStat(set_Down);
	/*::SendMessage(GetParent()->GetSafeHwnd(),WM_HSCROLL,0,(LPARAM)this->m_hWnd);

	if( NULL != mp_slideEventListerner)
	{
	this->mp_slideEventListerner->OnCliekSlide(this);
	}*/
	return CSliderCtrl::OnLButtonDown(nFlags, point);
}

void CColorSlide::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_SlideType == PERCENT_SLIDE )
		return CSliderCtrl::OnLButtonUp(nFlags, point);
	if(( point.x-TRACKBAR_INTERVAL >m_iStatePosX ) &&( point.x-TRACKBAR_INTERVAL< m_iStatePosX ))
	{
		return CSliderCtrl::OnLButtonUp(nFlags, point);
	}
	m_iStatePosX = point.x;

	if (m_iStatePosX < TRACKBAR_INTERVAL) m_iStatePosX = 0;
	else if (m_iStatePosX > m_nWidth-2*TRACKBAR_INTERVAL ) m_iStatePosX = m_nWidth-2*TRACKBAR_INTERVAL ;
	else
	{
		m_iStatePosX -= TRACKBAR_INTERVAL;
	}

	ReleaseCapture();
	m_bButtonDown = FALSE;
	FPosition =  (int)( FMaxValue * (float)(m_iStatePosX)/(float)(m_nWidthPlay) + 0.5 );


	DynamicStat(set_Normal);
	//::SendMessage(GetParent()->GetSafeHwnd(),WM_HSCROLL,0,(LPARAM)this->m_hWnd);
	if( NULL != mp_slideEventListerner)
	{
		this->mp_slideEventListerner->OnCliekSlide(this);
	}

	return CSliderCtrl::OnLButtonUp(nFlags, point);
}

void CColorSlide::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_SlideType == PERCENT_SLIDE )
		return CSliderCtrl::OnMouseMove(nFlags, point);
	if (m_bButtonDown)
	{
		if (nFlags == MK_LBUTTON)
		{
			m_iStatePosX = point.x;

			if (m_iStatePosX < TRACKBAR_INTERVAL) m_iStatePosX = 0;
			else if (m_iStatePosX > m_nWidth-2*TRACKBAR_INTERVAL ) m_iStatePosX = m_nWidth-2*TRACKBAR_INTERVAL ;
			else
			{
				m_iStatePosX -= TRACKBAR_INTERVAL;
			}

			FPosition =  (int)( FMaxValue * (float)(m_iStatePosX)/(float)(m_nWidthPlay) + 0.5 );
			DynamicStat(set_Down);
			::SendMessage(GetParent()->GetSafeHwnd(),WM_HSCROLL,0,(LPARAM)this->m_hWnd);
		}
	}
	return CSliderCtrl::OnMouseMove(nFlags, point);
}

void CColorSlide::DynamicStat(ENUM_SLIDERSTATE aState)
{
	if (m_nWidth <= 0)
	{
		return;
	}
	CDC* pDC =GetWindowDC();	
	if (pDC == NULL)
	{
		return;
	}
	pDC->SetBkColor(TRANSPARENT);
	CRect lrcClientRect;
	GetClientRect(lrcClientRect);
	m_nWidth = lrcClientRect.Width();
	lrcClientRect.bottom;
	m_nHeight = lrcClientRect.Height();

	CMemDC TempDC(pDC, CRect(lrcClientRect.left, lrcClientRect.top,lrcClientRect.right, lrcClientRect.bottom));
	PaintBK(lrcClientRect,TempDC.GetSafeHdc());
	if( m_SlideType != PERCENT_SLIDE ) 
	DrawSlider(aState,lrcClientRect,TempDC.GetSafeHdc());
	BitBlt(pDC->GetSafeHdc(),0,0,m_nWidth,m_nHeight,TempDC.GetSafeHdc(),0,0,SRCCOPY);
	ReleaseDC(pDC);
}

void CColorSlide::DrawSlider(ENUM_SLIDERSTATE aState,CRect aRect,HDC hMemDC)
{

	CRect lrRect = aRect;
	lrRect.top = aRect.top + 9;
	lrRect.bottom = lrRect.top + 7;
	lrRect.left = m_iStatePosX+TRACKBAR_INTERVAL-1;
	if( m_iStatePosX == 0)
	{
		lrRect.left = m_iStatePosX+TRACKBAR_INTERVAL;
	}
	if( lrRect.left< 0)
	{
		lrRect.left = 0;
	}
	lrRect.right = lrRect.left + 4;
	
	gBitMapOperate.DrawArrow(hMemDC, RGB(100,100,100),lrRect.left,lrRect.top,lrRect.left-4,lrRect.bottom,lrRect.right,lrRect.bottom,true);
}

void CColorSlide::PaintBK(CRect aRect,HDC hMemDC)
{
	::SetBkMode(hMemDC,TRANSPARENT);



	COLORREF lcolorBack = ::GetBkColor(hMemDC);
	gBitMapOperate.FillRectangle(hMemDC, aRect, lcolorBack, true ) ;

	RECT lrcTrack     = aRect;
	lrcTrack.top     += 2;
	lrcTrack.bottom   = lrcTrack.top+7;
	lrcTrack.left    += TRACKBAR_INTERVAL;
	lrcTrack.right   -= TRACKBAR_INTERVAL;

	if( (m_iStatePosX)>=0 )
	{
		CDC::FromHandle(hMemDC)->FrameRect( &lrcTrack,&CBrush( gWndColor.getColor(SLIDER_CTROL)->structColBase->colFrame1) );
		::InflateRect(&lrcTrack,-1,-1);
		switch(m_SlideType)
		{
		case LIGHT_SLIDE:
			gBitMapOperate.FillRectangleLight( hMemDC,lrcTrack,m_colBegain,m_colEnd,m_nHue, gBitMapOperate.edoHorizontal, true, (lrcTrack.right - lrcTrack.left) );
			break;
		case HUE_SLIDE:
			gBitMapOperate.FillRectangleHue( hMemDC,lrcTrack,m_colBegain,m_colEnd, gBitMapOperate.edoHorizontal, true, (lrcTrack.right - lrcTrack.left) );
			break;
		case PERCENT_SLIDE:
			{
				CRect lrect1 = lrcTrack;
				lrect1.right = lrect1.left>m_iStatePosX? lrect1.left:m_iStatePosX;
				CDC::FromHandle(hMemDC)->FillRect( lrect1, &CBrush(m_colBegain ) );
				
				CRect lrect2 = lrcTrack;
				lrect2.left = lrect2.left>m_iStatePosX? lrect2.left:m_iStatePosX ;
				CDC::FromHandle(hMemDC)->FillRect( lrect2, &CBrush( m_colEnd ) );
			}
			break;
		case S_SLIDE:
			gBitMapOperate.FillRectangleS( hMemDC,lrcTrack,m_colBegain,m_colEnd,m_nHue, gBitMapOperate.edoHorizontal, true, (lrcTrack.right - lrcTrack.left) );
        break;
		default:
			break;
		}
	}
}

void CColorSlide::SetButtonPos(int aiValue)
{
	if(aiValue > FMaxValue)
	{
		FPosition = FMaxValue;
	}
	else
	{
		if(aiValue < FMinValue)
		{
			FPosition = FMinValue;
		}
		else
		{
		FPosition = aiValue;
		}
	}
	m_nWidthPlay = m_nWidth-2*TRACKBAR_INTERVAL;
	if(FMaxValue <=FMinValue)
	{
		m_iStatePosX = 0;
	}
	else
	{
		m_iStatePosX = (int)( ((float)FPosition-(float)FMinValue)/(float)(FMaxValue - FMinValue) * m_nWidthPlay+0.5);// +0.5 浮点计算进位补偿
	}
	::SendMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_HSCROLL,0,(LPARAM)this->m_hWnd);
	Invalidate();
}

UINT CColorSlide::GetButtonPos()
{
	return FPosition;
}

void CColorSlide::SetRangeMax(int aiMax)
{
	FMaxValue = aiMax;
	Invalidate();
}

INT CColorSlide::GetRangeMax() const
{
	return FMaxValue;
}

void CColorSlide::SetRangeMin(INT aiMin)
{
	FMinValue = aiMin;
	Invalidate();
}

INT CColorSlide::GetRangeMin() const
{
	return FMinValue;
}

void CColorSlide::SetRange(int nMin, int nMax)
{
	SetRangeMin(nMin);
	SetRangeMax(nMax);
}

void CColorSlide::GetRange(int& nMin, int& nMax) const
{
	ASSERT(::IsWindow(m_hWnd));
	nMin = GetRangeMin();
	nMax = GetRangeMax();
}

void CColorSlide::SetTicFreq(int nFreq)
{ 
	ASSERT(::IsWindow(m_hWnd)); 
	::SendMessage(m_hWnd, TBM_SETTICFREQ, nFreq, 0L); 
}
void CColorSlide::SetSliderCtrlType(ENUM_SLIDE_TYPE aSlideType)
{
	m_SlideType = aSlideType ;

	CRect mrClientRect;
	GetClientRect(mrClientRect);
	m_nWidth = mrClientRect.Width();
	m_nHeight = mrClientRect.Height();
	Invalidate();
	DynamicStat(set_Normal);
	::SendMessage(AfxGetMainWnd()->GetSafeHwnd(),WM_HSCROLL,0,(LPARAM)this->m_hWnd);
	Invalidate();

}
BOOL CColorSlide::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	/*if (  m_ctlTips.m_hWnd != NULL  )
	m_ctlTips.RelayEvent(pMsg);*/

	return CSliderCtrl::PreTranslateMessage(pMsg);
}

void CColorSlide::UpdateToolTips()
{
	//m_ctlTips.UpdateTipText(m_HintText, this);
}
void CColorSlide::SetHintText(CString strValue)
{
	/*m_HintText = strValue;
	m_ctlTips.UpdateTipText(m_HintText, this);*/

}
void CColorSlide::SaveSize()
{
	CRect mrClientRect;
	GetClientRect(mrClientRect);
	m_nWidth = mrClientRect.Width();
	m_nHeight = mrClientRect.Height();
}
void CColorSlide::SetBKColor(COLORREF acolBegain, COLORREF acolEnd,UINT aHue)
{
	m_colBegain = acolBegain;
	m_colEnd    = acolEnd;
	m_nHue =  aHue;
}
void CColorSlide::SetBKColor(COLORREF acolBegain)
{
	m_colBegain = acolBegain;
}
