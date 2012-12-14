

#include "stdafx.h"
#include "ScrollControl.h"
#include "time.h"
#include "mylist.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef SCROOL_HEAD
#define SCROOL_HEAD (WM_USER+10002)
#endif 

static void GetClientRectSB(CWnd* pWnd, CRect& rect)
{
	ASSERT( pWnd != NULL );

	CRect winRect;
	pWnd->GetWindowRect(&winRect);
	pWnd->ScreenToClient(&winRect);

	pWnd->GetClientRect(&rect);

	int cxSB = ::GetSystemMetrics(SM_CXVSCROLL);
	int cySB = ::GetSystemMetrics(SM_CYHSCROLL);

	if ( winRect.right >= (rect.right + cxSB) )
		rect.right += cxSB;
	if ( winRect.bottom >= (rect.bottom + cySB) )
		rect.bottom += cySB;


}

CScrollControl::CScrollControl(int itemHight)
:m_hight(itemHight),
m_attachWnd(NULL)
{
	m_pageSize    = CSize(0,0);
	m_displaySize = CSize(0,0);
	m_scrollPos   = CSize(0,0);

}

CScrollControl::~CScrollControl()
{
	DetachWnd();
}

void CScrollControl::AttachWnd(CWnd* pWnd)
{
	m_attachWnd = pWnd;
}

void CScrollControl::DetachWnd()
{
	m_attachWnd = NULL;
}

void CScrollControl::SetDisplaySize(int displayWidth, int displayHeight,int nBar)
{
	m_displaySize = CSize(displayWidth+GetScrollPos()->cx, displayHeight);

	if ( m_attachWnd != NULL && ::IsWindow(m_attachWnd->m_hWnd) )
		UpdateScrollInfo(nBar);
}

const CSize& CScrollControl::GetDisplaySize() const
{
	return m_displaySize;
}

const CSize* CScrollControl::GetScrollPos()
{
	return &m_scrollPos;
}

const CSize& CScrollControl::GetPageSize() const
{
	return m_pageSize;
}

void CScrollControl::ScrollToOrigin(bool scrollLeft, bool scrollTop)
{
	if ( m_attachWnd == NULL )
		return;

	if ( scrollLeft )
	{
		if(!( m_scrollPos.cx <= 0 ) )
		{
		if ( m_displaySize.cx > 0 && m_pageSize.cx > 0 && m_scrollPos.cx > 0 )
		{
			int deltaPos = -m_scrollPos.cx;
			m_scrollPos.cx += deltaPos;
			m_attachWnd->SetScrollPos(SB_HORZ, m_scrollPos.cx, TRUE);
			m_attachWnd->ScrollWindow(-deltaPos, 0);
			ScrollHead( -deltaPos );

		}
		}
	}

	if ( scrollTop )
	{
		if( m_scrollPos.cy <= 0 )
		return;
		if ( m_displaySize.cy > 0 && m_pageSize.cy > 0 && m_scrollPos.cy > 0 )
		{
			int deltaPos = -m_scrollPos.cy;
			m_scrollPos.cy += deltaPos;
			m_attachWnd->SetScrollPos(SB_VERT, m_scrollPos.cy, TRUE);
			m_attachWnd->ScrollWindow(0, -deltaPos);
		}
	}
}

void CScrollControl::ScrollToEnd(bool scrollRight, bool scrollBotton)
{
	if ( m_attachWnd == NULL )
		return;
	

	if ( scrollRight )
	{
		/*if ( m_displaySize.cx > 0 && m_pageSize.cx > 0 && m_scrollPos.cx < m_displaySize.cx )
		{
		int deltaPos = m_displaySize.cx-m_scrollPos.cx;
		m_scrollPos.cx = m_displaySize.cx;
		m_attachWnd->SetScrollPos(SB_HORZ, m_scrollPos.cx, TRUE);
		m_attachWnd->ScrollWindow(-deltaPos, 0);
		ScrollHead( -deltaPos );

		}*/
	}

	if ( scrollBotton )
	{
		if ( m_displaySize.cy > 0 && m_pageSize.cy > 0 && m_scrollPos.cy < m_displaySize.cy )
		{
			CRect rect;
			GetClientRectSB( m_attachWnd, rect );
			if( m_scrollPos.cy+rect.Height() >= m_displaySize.cy )
			return;
			int deltaPos =m_displaySize.cy-m_scrollPos.cy-rect.Height();
			m_scrollPos.cy = deltaPos;
			m_attachWnd->SetScrollPos(SB_VERT, m_scrollPos.cy, TRUE);
			m_attachWnd->ScrollWindow(0, -deltaPos);
		}
	}
}
void CScrollControl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ( m_attachWnd == NULL )
		return;
	static DWORD t1 = 0;
	static DWORD t2 = 0;

	t2 = GetTickCount();

	if( ((t2-t1)<35) ) 
	{
		return;
	}
	t1 = t2;

	const int lineOffset = m_hight;

	int deltaPos = 0;
	switch( nSBCode )
	{
	case SB_LINELEFT:
		deltaPos = -lineOffset;
		break;

	case SB_LINERIGHT:
		deltaPos = lineOffset;
		break;

	case SB_PAGELEFT:
		deltaPos = -m_pageSize.cx;
		break;

	case SB_PAGERIGHT:
		deltaPos = m_pageSize.cx;
		break;

	case SB_THUMBTRACK:
		deltaPos = GetScrollPos(SB_HORZ, pScrollBar) - m_scrollPos.cx;
		break;
	case SB_THUMBPOSITION:
		deltaPos = GetScrollPos(SB_HORZ, pScrollBar) - m_scrollPos.cx;
		break;

	default:
		return;
	}
	int newScrollPos = m_scrollPos.cx + deltaPos;
	if ( newScrollPos < 0 )
		deltaPos = -m_scrollPos.cx;

	int maxScrollPos = m_displaySize.cx - m_pageSize.cx;
	if ( newScrollPos > maxScrollPos )
		deltaPos = maxScrollPos - m_scrollPos.cx;
	if ( deltaPos !=0 )
	{
		m_scrollPos.cx += deltaPos;
		ScrollHead(-deltaPos);
		m_attachWnd->SetScrollPos(SB_HORZ, m_scrollPos.cx, TRUE);
		m_attachWnd->ScrollWindow(-deltaPos, 0);
	}
}

void CScrollControl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if ( m_attachWnd == NULL )
		return;

	// 滚动频率限制优化
	static DWORD t1 = 0;
	static DWORD t2 = 0;

	t2 = GetTickCount();

	if( ((t2-t1)<35) ) 
		return;
	t1 = t2;
	const int lineOffset = m_hight;


	CRect clentRect;
	m_attachWnd->GetClientRect( &clentRect );
	int deltaPos = 0;
	switch( nSBCode )
	{
	case SB_LINEUP:
		deltaPos = -lineOffset;
		break;

	case SB_LINEDOWN:
		deltaPos = lineOffset;
		break;

	case SB_PAGEUP:
		deltaPos = -m_pageSize.cy;
		break;

	case SB_PAGEDOWN:
		deltaPos = m_pageSize.cy;
		break;

	case SB_THUMBTRACK:
		deltaPos = GetScrollPos(SB_VERT, pScrollBar) - m_scrollPos.cy;
		if( deltaPos>0 )
		{
			if( deltaPos%m_hight >= m_hight/2 )
			{
				deltaPos = deltaPos+m_hight -((deltaPos+m_hight)%m_hight);
			}
			else if( m_displaySize.cy == m_scrollPos.cy+clentRect.Height()+ deltaPos%m_hight )
			{
				deltaPos = deltaPos+m_hight -((deltaPos+m_hight)%m_hight);
			}
			else
			{
				deltaPos -= deltaPos%m_hight;
			}
		}
		if(  deltaPos<0 ) 
		{
			if( deltaPos%m_hight < -m_hight/2 )
			{
				deltaPos = -(m_hight-deltaPos - ((m_hight-deltaPos)%m_hight) );
			}
			else if( 0 == m_scrollPos.cy+ deltaPos%m_hight )
			{
				deltaPos = deltaPos%m_hight;
			}
			else
			{
				deltaPos -= deltaPos%m_hight;
			}
		}
		break;

	case SB_THUMBPOSITION:
		deltaPos = GetScrollPos(SB_VERT, pScrollBar) - m_scrollPos.cy;
		if( deltaPos>0 )
		{
			if( deltaPos%m_hight >= m_hight/2 )
			{
				deltaPos = deltaPos+m_hight -((deltaPos+m_hight)%m_hight);
			}
			else if( m_displaySize.cy == m_scrollPos.cy+clentRect.Height()+ deltaPos%m_hight )
			{
				deltaPos = deltaPos+m_hight -((deltaPos+m_hight)%m_hight);
			}
			else
			{
				deltaPos -= deltaPos%m_hight;
			}
		}
		if(  deltaPos<0 ) 
		{
			if( deltaPos%m_hight < -m_hight/2 )
			{
				deltaPos = -(m_hight-deltaPos - ((m_hight-deltaPos)%m_hight) );
			}
			else if( 0 == m_scrollPos.cy+ deltaPos%m_hight )
			{
				deltaPos = deltaPos%m_hight;
			}
			else
			{
				deltaPos -= deltaPos%m_hight;
			}
		}
		break;
	case SB_POS:
		{
			deltaPos = nPos;
			if( deltaPos>0 )
			{
				if( deltaPos%m_hight >= m_hight/2 )
				{
					deltaPos = deltaPos+m_hight -((deltaPos+m_hight)%m_hight);
				}
				else
				{
					deltaPos -= deltaPos%m_hight;
				}
			}
			if(  deltaPos<0 ) 
			{
				if( deltaPos%m_hight < -m_hight/2 )
				{
					deltaPos = -(m_hight-deltaPos - ((m_hight-deltaPos)%m_hight) );
				}
				else
				{
					deltaPos -= deltaPos%m_hight;
				}
			}
			int newScrollPos;
			if( ( deltaPos > m_scrollPos.cy ) || deltaPos < m_scrollPos.cy  )
			{
				CRect rect;
				GetClientRectSB( m_attachWnd, rect );
				deltaPos = (deltaPos > m_scrollPos.cy)?deltaPos - m_scrollPos.cy-rect.Height() :deltaPos-m_scrollPos.cy;
				m_scrollPos.cy += deltaPos;
				m_attachWnd->SetScrollPos(SB_VERT, m_scrollPos.cy, TRUE);
				m_attachWnd->ScrollWindow(0, -deltaPos);
				return;
			}
			else
			{
				deltaPos = 0;
			}
			break;
		}
		
	default:

		return;
	}

	int newScrollPos = m_scrollPos.cy + deltaPos;

	if ( newScrollPos < 0 )
		deltaPos = -m_scrollPos.cy;


	int maxScrollPos = m_displaySize.cy - m_pageSize.cy;
	if ( newScrollPos > maxScrollPos )
		deltaPos = maxScrollPos - m_scrollPos.cy;

	if ( deltaPos !=0 )
	{
		m_scrollPos.cy += deltaPos;
		m_attachWnd->SetScrollPos(SB_VERT, m_scrollPos.cy, TRUE);
		m_attachWnd->ScrollWindow(0, -deltaPos);
	}
}
void CScrollControl::SetAItemMinHight(int nHight)
{
	m_hight = nHight;
}
BOOL CScrollControl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if ( m_attachWnd == NULL )
		return FALSE;


	int scrollMin = 0, scrollMax = 0;
	m_attachWnd->GetScrollRange(SB_VERT, &scrollMin, &scrollMax);
	if ( scrollMin == scrollMax )
		return FALSE;

	int numScrollIncrements = abs(zDelta) / WHEEL_DELTA;

	int numScrollLinesPerIncrement = 0;
	::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &numScrollLinesPerIncrement, 0);

	if ( numScrollLinesPerIncrement == WHEEL_PAGESCROLL )
	{
		OnVScroll(zDelta > 0 ? SB_PAGEUP : SB_PAGEDOWN, 0, NULL);
		return TRUE;
	}


	int numScrollLines = numScrollIncrements * numScrollLinesPerIncrement;

	numScrollLines = max(numScrollLines/3, 1);

	OnVScroll(zDelta > 0 ? SB_LINEUP : SB_LINEDOWN, 0, NULL);

	return TRUE;
}

void CScrollControl::OnSize(UINT nType, int cx, int cy)
{
	UpdateScrollInfo(SB_BOTH);

}

int CScrollControl::GetScrollPos(int bar, CScrollBar* pScrollBar)
{
	ASSERT( m_attachWnd != NULL );
	HWND hWndScroll;
	if ( pScrollBar == NULL )
		hWndScroll = m_attachWnd->m_hWnd;
	else
		hWndScroll = pScrollBar->m_hWnd;

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_TRACKPOS;
	::GetScrollInfo(hWndScroll, bar, &si);

	int scrollPos = si.nTrackPos;

	return scrollPos;
}

void CScrollControl::UpdateScrollInfo(int nBar)
{
	if ( m_attachWnd == NULL )
		return;

	CRect rect;
	GetClientRectSB(m_attachWnd, rect);
	CSize windowSize(rect.Width(), rect.Height());

	CSize deltaPos(0,0);
	if( nBar==SB_BOTH )
	{
		UpdateScrollBar(SB_HORZ, windowSize.cx, m_displaySize.cx,
		m_pageSize.cx, m_scrollPos.cx, deltaPos.cx);
		UpdateScrollBar(SB_VERT, windowSize.cy, m_displaySize.cy,
		m_pageSize.cy, m_scrollPos.cy, deltaPos.cy);
	}
	else if( nBar==SB_HORZ )
	{
		UpdateScrollBar(SB_HORZ, windowSize.cx, m_displaySize.cx,
		m_pageSize.cx, m_scrollPos.cx, deltaPos.cx);
	}
	else if( nBar==SB_VERT )
	{
		UpdateScrollBar(SB_VERT, windowSize.cy, m_displaySize.cy,
		m_pageSize.cy, m_scrollPos.cy, deltaPos.cy);
	}

	if ( deltaPos.cx != 0 || deltaPos.cy != 0 )
	{
		if( deltaPos.cy>0 )
		{
			if( deltaPos.cy%m_hight>= m_hight/2 )
			{
				deltaPos.cy = deltaPos.cy+m_hight -((deltaPos.cy+m_hight)%m_hight);
			}
			else
			{
				deltaPos.cy -= deltaPos.cy%m_hight;
			}
		}
		if(  deltaPos.cy<0 ) 
		{
			if( deltaPos.cy%m_hight < - m_hight/2 )
			{
				deltaPos.cy = -(m_hight-deltaPos.cy - ((m_hight-deltaPos.cy)%m_hight) );
			}
			else
			{
				deltaPos.cy -= deltaPos.cy%m_hight;
			}
		}
		m_attachWnd->ScrollWindow(deltaPos.cx, deltaPos.cy);
		ScrollHead( deltaPos.cx );


	}                                                        
}
void CScrollControl::UpdateScrollBar(int bar, int windowSize, int displaySize,
									 LONG& pageSize, LONG& scrollPos, LONG& deltaPos)
{
	int scrollMax = 0;
	deltaPos = 0;
	if ( windowSize < displaySize )
	{
		scrollMax = displaySize - 1;
		if ( pageSize > 0 && scrollPos > 0 )
		{
			scrollPos = (LONG)(1.0 * scrollPos * windowSize / pageSize);
		}
		pageSize = windowSize;
		scrollPos = min(scrollPos, displaySize - pageSize - 1);

		deltaPos = m_attachWnd->GetScrollPos(bar) - scrollPos;

	}
	else
	{
		pageSize = 0;
		scrollPos = 0;
		deltaPos = m_attachWnd->GetScrollPos(bar);
	}

	SCROLLINFO si;
	memset(&si, 0, sizeof(SCROLLINFO));
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask  = SIF_ALL;    // SIF_ALL = SIF_PAGE | SIF_RANGE | SIF_POS;
	si.nMin   = 0;
	si.nMax   = scrollMax;
	si.nPage  = pageSize;
	si.nPos   = scrollPos;
	m_attachWnd->SetScrollInfo(bar, &si, TRUE);
}
//class CMyList;
void CScrollControl::ScrollHead(int pos)
{
	if( m_attachWnd )
	{
		CMyList* pwnd = (CMyList*)m_attachWnd;
		::SendMessage( pwnd->GetHeadHwnd(), SCROOL_HEAD, 0, pos );
	}
}


