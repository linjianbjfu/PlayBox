#include "stdafx.h"
#include "DownloadPercentWnd.h"
#include "ColorWnd.h"
#include "BitMapOperate.h"
#include "WndColorMgr.h"
#include "DownSubject.h"

extern CBitMapOperate gBitMapOperate;
extern CWndColorMgr   gWndColor;

#define TIMER_CHECK_DOWN	 100

IMPLEMENT_DYNAMIC(CDownloadPercentWnd, CWnd)
CDownloadPercentWnd::CDownloadPercentWnd()
{
	m_pEvnet	= NULL;
	m_pDown		= NULL;
}

CDownloadPercentWnd::~CDownloadPercentWnd()
{
}

BEGIN_MESSAGE_MAP(CDownloadPercentWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_MESSAGE(MSG_DOWNSUBSTART,OnDownSubStart)
	ON_MESSAGE(MSG_DOWNSUBCLOSE,OnDownSubClose)
END_MESSAGE_MAP()

int CDownloadPercentWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect clientlRect;
	GetClientRect( &clientlRect );

	CRect slideRect;
	slideRect = clientlRect;

	if( AfxGetUIManager()->UIGetLayoutMgr()->GetDisplayMode() == DISPLAY_SEPERATE_MODE )
	{
		slideRect.top += 5;
	}
	else
	{
		slideRect.top += 15;
	}
	slideRect.left += 15;
	slideRect.bottom = slideRect.top + 10;
	slideRect.right -= 25;

	m_downLoadSlide.Create( WS_VISIBLE|WS_CHILD, slideRect, this, 10000-6 );
	m_downLoadSlide.SetSliderCtrlType( PERCENT_SLIDE );
	m_downLoadSlide.SetBKColor( RGB(31,233,29),RGB(243,243,243),0 );
	m_downLoadSlide.SetRange( 0, 100 );
	m_downLoadSlide.ShowWindow( SW_SHOW );

	CRect hintRect;
	hintRect = clientlRect;
	if( AfxGetUIManager()->UIGetLayoutMgr()->GetDisplayMode() == DISPLAY_SEPERATE_MODE )
	{
		hintRect.top += 18;
	}
	else
	{
		hintRect.top += 33;
	}
	hintRect.left += 18;
	hintRect.bottom = hintRect.top + 13;

	m_textHit.Create( NULL, 5, hintRect, this, 10000-7 );
	m_textHit.ShowWindow(SW_SHOW);

	CRect percentRect;
	percentRect = clientlRect;
	if( AfxGetUIManager()->UIGetLayoutMgr()->GetDisplayMode() == DISPLAY_SEPERATE_MODE )
	{
		percentRect.top += 5;
	}
	else
	{
		percentRect.top += 15;
	}

	percentRect.bottom = percentRect.top + 13;
	percentRect.left = percentRect.right - 25;
		
	m_textPercent.Create( NULL, 5, percentRect, this, 10000-8 );
	m_textPercent.ShowWindow(SW_SHOW);
	return 0;
}
void CDownloadPercentWnd::SetDownloadHit( CString str )
{
	m_textHit.SetWindowText( str );
}

void CDownloadPercentWnd::SetDownloadPercent( int percentage )
{
	if( ( percentage<0)||(percentage>100) ) 
		return;
	CString str;
	str.Format( "%d", percentage );
	str += "%";
	m_textPercent.SetWindowText( str );
	m_downLoadSlide.SetButtonPos( percentage );
}

void CDownloadPercentWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect( &rect );
	gBitMapOperate.FillRectangle( dc.m_hDC, rect, 
		gWndColor.getColor(TABLE_CTROL)->structColBase->colBK, true );
}

LRESULT CDownloadPercentWnd::OnDownSubStart(WPARAM w,LPARAM)
{
	SetTimer(TIMER_CHECK_DOWN,500,NULL );
	if(false)
	{
		m_iDownPercent = GetTickCount() % 20 + 10;
		SetDownloadPercent( m_iDownPercent );
		m_bNetConnect = TRUE;
	}
	else 
	{
		m_iDownPercent = 0;
		SetDownloadPercent( m_iDownPercent );
		m_bNetConnect = FALSE;
	}

	m_pDown = (YL_CHTTPDownFile*)w;
	return 0;
}

LRESULT CDownloadPercentWnd::OnDownSubClose(WPARAM w,LPARAM)
{
	KillTimer( TIMER_CHECK_DOWN );
	if( m_pEvnet != NULL )
	{
		m_pEvnet->OnFinishDownSubject(w);
	}
	return 0;
}

void CDownloadPercentWnd::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == TIMER_CHECK_DOWN )
	{
		try
		{
			//m_bNetConnect = CCheckNetConnect::GetInstance()->IsNetConnect();
			//if(m_bNetConnect)
			//{
			//	m_iDownPercent += 10;
			//	if(m_iDownPercent < 99)
			//		SetDownloadPercent(m_iDownPercent);
			//}
		}	
		catch (...)
		{			
		}		
	}
	return ;
}