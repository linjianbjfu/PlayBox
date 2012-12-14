// HeadCtrol.cpp : implementation file
//

#include "stdafx.h"
#include "HeadCtrol.h"
#include ".\headctrol.h"
#include "../../Bitmap/DibBitmap.h"


// CHeadCtrol

IMPLEMENT_DYNAMIC(CHeadCtrol, CWnd)
CHeadCtrol::CHeadCtrol()
:m_width(0),
m_hight(0),
m_top(0),
m_bottonDown(false),
m_hListWnd(NULL),
m_nDragPos(-1),
m_pBitmapBk(NULL),
m_pBitmapMidLine(NULL),
m_pBitmapAsc(NULL),
m_pBitmapDesc(NULL),
m_textCol(0)
{
	m_currentSortInfo.first =  -1;
	m_currentSortInfo.second = 0;
	m_bTrackMouse		= false;
	m_nHoverItem		= -1;
	m_bLButtonDown		= false;
}

CHeadCtrol::~CHeadCtrol()
{
}


BEGIN_MESSAGE_MAP(CHeadCtrol, CWnd)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE( SCROOL_HEAD, OnScroolHead )
END_MESSAGE_MAP()



// CHeadCtrol message handlers

int CHeadCtrol::InsertItem( CString strTitle, int width, int minWidth,bool bSort)
{
	ItemPos pos;
	size_t lnsize =  m_arrayItemPos.size();
	if( lnsize>0 ) 
	{
		ItemPos posLast;
		posLast = m_arrayItemPos[lnsize-1];
		pos.pos[0] = posLast.pos[1];
	}
	else
	{
		pos.pos[0] = 0;
	}
	pos.pos[1] = pos.pos[0]+width;
	pos.pos[2] = pos.pos[0]+minWidth; 

	m_arrayText.push_back( strTitle );
	m_arrayItemPos.push_back( pos );
	m_arrayBoolSort.push_back( bSort );

	return (int)lnsize+1;
}
void CHeadCtrol::DeleteAllItem()
{
	m_arrayText.clear();
	m_arrayItemPos.clear();
	m_arrayBoolSort.clear();
}
void CHeadCtrol::ModifyItemText(int index, CString str)
{
	m_arrayText[index] = str;
}
void CHeadCtrol::GetItemText(int index,CString& str)
{
	str = m_arrayText[index];
}
void CHeadCtrol::ModifyItemData(int index, CString str,int width, int minWidth )
{
	m_arrayText[index] = str;

	ItemPos pos;
	size_t lnsize =  m_arrayItemPos.size();
	if( lnsize <=0 )
	{
		return;
	}
	else
	{
		int addWidth = width - (m_arrayItemPos[index].pos[1] - m_arrayItemPos[index].pos[0]);
		m_arrayItemPos[index].pos[1] += addWidth;
		m_arrayItemPos[index].pos[2] = m_arrayItemPos[index].pos[0] + minWidth;
		index += 1;
		while(index<m_arrayItemPos.size())
		{
			m_arrayItemPos[index].pos[0] += addWidth;
			m_arrayItemPos[index].pos[1] += addWidth;
			m_arrayItemPos[index].pos[2] = m_arrayItemPos[index].pos[0] + minWidth;

			index++;
		}
		if( addWidth != 0 )
		{
			this->Invalidate();
			if( ( m_arrayItemPos.size() > 0) && m_hListWnd ) 
				::SendMessage(m_hListWnd,CHANGE_SCROOL_MSG, 0, 0 );
		}
	}

}
void CHeadCtrol::SetHight(int hight)
{
	if( hight != m_hight &&  this->m_hWnd)
	{
		m_hight = hight;
		this->Invalidate();
	}
}

void CHeadCtrol::SetTop(int nTop)
{
	if( m_top != nTop &&  this->m_hWnd)
	{
		m_top = nTop;
		//this->Invalidate();
	}
}

void CHeadCtrol::SetWidth(CRect& rect)
{
	CRect lrect ;
	this->GetClientRect( &lrect);
	lrect.top = m_top;
	lrect.bottom = m_top + m_hight;
	lrect.right = rect.Width();
	this->MoveWindow( lrect, FALSE );
	if( rect.Width()!=m_width && this->m_hWnd)
	{
		m_width = rect.Width();
		this->Invalidate(true);
	}
}
//paint
void CHeadCtrol::PaintRight( CDC* pdc) //»­±íÎ²²¿
{
	size_t lnsize = m_arrayItemPos.size();
	CRect lrect;
	if( 0 != lnsize )
	{
		ItemPos litemPos = m_arrayItemPos[lnsize-1];

		lrect = CRect(litemPos.pos[1]+1, 0, m_width, m_hight ); // ÖØ»æÇøÓò
	}
	else
	{
		lrect =  CRect(0, 0, m_width, m_hight ); // ÖØ»æÇøÓò
	}
	m_pBitmapBk->SetCDibRect( lrect );
	m_pBitmapBk->Draw( pdc, FALSE );
}
void CHeadCtrol::PaintItem(CDC* pdc, CRect& rect)
{
	size_t lnsize = m_arrayItemPos.size();
	if( lnsize != 0 )
	{
		m_pBitmapBk->SetCDibRect( rect );
		m_pBitmapBk->Draw( pdc, FALSE );
	}
	for(UINT i=0; i<lnsize; i++ )
	{
		ItemPos litemPos = m_arrayItemPos[i];

		CRect lrectItem( litemPos.pos[0], 0, litemPos.pos[1]-1, m_hight );
		CRect lrectLine( litemPos.pos[1]-1, 0, litemPos.pos[1], m_hight );
		//»­hoverµÄ±³¾°
		if( m_nHoverItem == i )
		{
			if(this->m_bLButtonDown)
			{
				m_pBitmapBkDown->SetCDibRect( lrectItem );
				m_pBitmapBkDown->Draw( pdc,FALSE );
			}
			else
			{
				m_pBitmapBkOver->SetCDibRect( lrectItem );
				m_pBitmapBkOver->Draw( pdc,FALSE );
				TRACE( "left:%d-right%d\n", lrectItem.left, lrectItem.right);
			}
		}
		//»­hoverµÄmidline
		if(m_nHoverItem == i)
		{
			m_pBitmapMidLineOver->SetCDibRect( lrectLine );
			m_pBitmapMidLineOver->Draw( pdc, FALSE );
			int start = lrectLine.TopLeft().x == 0 ? 0 : lrectItem.TopLeft().x - 1;
			m_pBitmapMidLineOver->SetCDibRect(CRect(start , 0, start+1, m_hight));
			m_pBitmapMidLineOver->Draw( pdc, FALSE );
		}
		else
		{
			m_pBitmapMidLine->SetCDibRect( lrectLine );
			m_pBitmapMidLine->Draw(pdc,FALSE);
		}
		AfxGdiOperator()->DrawText( pdc, m_arrayText[i], m_textCol, lrectItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
	}
}
void CHeadCtrol::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect clientRect;
	GetClientRect( &clientRect );

	CDC hdcMem;
	hdcMem.CreateCompatibleDC(NULL);
	CBitmap hBitmap;
	hBitmap.CreateCompatibleBitmap(&dc,clientRect.Width(),clientRect.Height());
	hdcMem.SelectObject(&hBitmap);

	PaintItem( &hdcMem ,clientRect);
	PaintRight( &hdcMem );
	PaintSortArrow( &hdcMem,m_currentSortInfo.first, m_currentSortInfo.second );

	dc.BitBlt(clientRect.TopLeft().x, clientRect.TopLeft().y,
		clientRect.Width(), clientRect.Height(),&hdcMem,0,0,SRCCOPY);
}
static int excursion  = 0;  // Æ«ÒÆ
void CHeadCtrol::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTrackMouse)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE;
		_TrackMouseEvent(&tme);
		m_bTrackMouse = TRUE;
	}
	static CPoint s_pt(0,0);
	if( s_pt != point )
	{
		s_pt = point;
		int nItems = m_arrayItemPos.size();
		CRect rect;
		for(int i = 0; i <nItems; i++)
		{
			rect.left = m_arrayItemPos[i].pos[0];
			rect.top = 0;
			rect.right = m_arrayItemPos[i].pos[1];
			rect.bottom = m_hight;

			if(rect.PtInRect(point))
			{
				if(i != m_nHoverItem)
				{	
					m_nHoverItem = i;
					Invalidate();
					break;
				}
			}
		}
	}
	////////////////////
	size_t lnsize = m_arrayItemPos.size();
	ItemPos litemPos;
	for(UINT i=0; i<lnsize; i++ )
	{
		litemPos = m_arrayItemPos[i];
		CRect lrect1( litemPos.pos[1]-MOUSE_EXCURSION, 0, litemPos.pos[1]+MOUSE_EXCURSION, m_hight );
		if( PtInRect( &lrect1, point ) )
		{
			::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
			break;
		}
	}
	if( m_bottonDown )
	{
		int pos = point.x - excursion;
		if( pos< m_arrayItemPos[m_nDragPos].pos[2] )
		{
			pos = m_arrayItemPos[m_nDragPos].pos[2];
		}
		PaintDragLine( pos );
		::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CHeadCtrol::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = true;
	FromHandle(m_hListWnd)->SetFocus();
	size_t lnsize = m_arrayItemPos.size();
	SetCapture();
	for(UINT i=0; i<lnsize; i++ )
	{
		ItemPos litemPos = m_arrayItemPos[i];
		CRect lrect1( litemPos.pos[1]-MOUSE_EXCURSION, 0, litemPos.pos[1]+MOUSE_EXCURSION, m_hight );
		if( PtInRect( &lrect1, point ) )
		{
			//ÍÏ×§
			excursion  = point.x - litemPos.pos[1];  // Æ«ÒÆ
			m_nDragPos = i;
			::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
			PaintDragLine( litemPos.pos[1] );

			m_bottonDown = true;
			break;

		}
		else
		{
			// ÅÅÐò
			if ( m_arrayBoolSort[i]  )
			{
				int nStart = 0;
				if( 0 != i )
				{
					nStart = m_arrayItemPos[i-1].pos[1]+MOUSE_EXCURSION+1;
				}

				CRect lrectItem( nStart, 0, litemPos.pos[1]-MOUSE_EXCURSION-1, m_hight );
				if( PtInRect( &lrectItem,point  ) )
				{
					if( m_currentSortInfo.first == i )
					{
						if( m_currentSortInfo.second == 2 )
						{
							m_currentSortInfo.second  = 1;
							SortArrow( m_currentSortInfo.first, m_currentSortInfo.second );
						}
						else
						{
							m_currentSortInfo.second  = 2;
							SortArrow( m_currentSortInfo.first, m_currentSortInfo.second );
						}
					}
					else
					{
						SortArrow( m_currentSortInfo.first, 0 );
						m_currentSortInfo.first  = i;
						m_currentSortInfo.second  = 1;
						SortArrow( m_currentSortInfo.first, m_currentSortInfo.second );
					}
				}
			}
		}
	}
	CWnd::OnLButtonDown(nFlags, point);
	Invalidate();
}
static int posPre = -1;

void CHeadCtrol::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDown = false;
	if( -1 != posPre )
	{
		PaintDragLine( posPre, true );
		if( posPre+excursion < m_arrayItemPos[m_nDragPos].pos[1]  )
		{
			size_t lnsize = m_arrayItemPos.size();
			static int interval = 0;
			for(UINT i= m_nDragPos; i<lnsize; i++ )
			{
				ItemPos litemPos = m_arrayItemPos[i];
				if(  i== m_nDragPos )
				{
					interval =	litemPos.pos[1] - posPre;
					m_arrayItemPos[i].pos[1] -= interval;
				}
				else
				{
					m_arrayItemPos[i].pos[0] -= interval;
					m_arrayItemPos[i].pos[1] -= interval;
					m_arrayItemPos[i].pos[2] -= interval;
				}
			}
			interval = 0;
		}
		if( posPre+excursion > m_arrayItemPos[m_nDragPos].pos[1] )
		{
			size_t lnsize = m_arrayItemPos.size();
			static int interval = 0;
			for(UINT i= m_nDragPos; i<lnsize; i++ )
			{
				ItemPos litemPos = m_arrayItemPos[i];
				if(  i== m_nDragPos )
				{
					interval =	posPre - litemPos.pos[1];
					m_arrayItemPos[i].pos[1] += interval;
				}
				else
				{
					m_arrayItemPos[i].pos[0] += interval;
					m_arrayItemPos[i].pos[1] += interval;
					m_arrayItemPos[i].pos[2] += interval;
				}
			}
			interval = 0;
		}
		if( m_bottonDown )
		{
			if( ( m_arrayItemPos.size() > 0) && m_hListWnd ) 
			{
				CWnd::FromHandle(m_hListWnd)->SendMessage( CHANGE_SCROOL_MSG, 0,0);
			}
		}
		posPre = -1;
	}
	m_bottonDown = false;
	CWnd::OnLButtonUp(nFlags, point);
	ReleaseCapture();

	excursion = 0;
	m_nDragPos = -1;
	Invalidate();
}
void CHeadCtrol::AttachListWnd( HWND hwnd )
{
	m_hListWnd = hwnd;
}

void CHeadCtrol::PaintDragLine(int pos, bool bOne ) //ÍÏ×§ÏßÌõ
{
	CDC* pDC = CWnd::FromHandle(m_hListWnd)->GetWindowDC();
	int nRop = pDC->SetROP2(R2_NOTXORPEN);
	CRect lrect;
	CWnd::FromHandle(m_hListWnd)->GetClientRect( &lrect );

	CPen  pen;
	pen.CreatePen(0, 1, RGB(0, 0, 0));
	CPen *pOP = pDC->SelectObject(&pen);



	if( posPre != -1  ) 
	{
		pDC->MoveTo( posPre,0 );
		pDC->LineTo( posPre,lrect.bottom );
	}

	if(  !bOne  ) 
	{
		pDC->MoveTo( pos,0 );
		pDC->LineTo( pos,lrect.bottom );
	}


	pDC->SetROP2(nRop);
	pDC->SelectObject(pOP);
	pen.DeleteObject();

	ReleaseDC(pDC);

	posPre = pos;
}
void CHeadCtrol::PaintSortArrow(CDC* pdc,int nItem, int nType)
{
	if( -1 == nItem ) return;
	// »æÖÆ
	CSize size;
	size = pdc->GetTextExtent(m_arrayText[nItem] );
	CRect ArrowRect,  ArrowBk;
	ArrowRect.left = m_arrayItemPos[nItem].pos[0]+(m_arrayItemPos[nItem].pos[1]-m_arrayItemPos[nItem].pos[0]) /2 + size.cx/2 +2;


	int addTop ;


	if( nItem>= 0)
	{
		if( nType == 2 )
		{
			addTop = (m_hight - m_pBitmapAsc->GetHeight())/2;
			ArrowRect.top  = addTop>0 ? addTop:0 ;
			ArrowRect.bottom = ArrowRect.top+m_pBitmapAsc->GetHeight();
			ArrowRect.right  = ArrowRect.left+m_pBitmapAsc->GetWidth();
			ArrowRect.right = ArrowRect.right >  m_arrayItemPos[nItem].pos[1]? m_arrayItemPos[nItem].pos[1]:ArrowRect.right;

			ArrowBk.left = ArrowRect.left;
			ArrowBk.top = 0;
			ArrowBk.bottom = m_hight;
			ArrowBk.right = ArrowRect.left;
			ArrowBk.right = ArrowRect.right;

			if( ArrowBk.right > ArrowBk.left )
			{
				if( nItem == m_nHoverItem )
				{
					if(m_bLButtonDown)
					{
						m_pBitmapBkDown->SetCDibRect( ArrowBk );
						m_pBitmapBkDown->Draw( pdc,FALSE );
					}
					else
					{
						m_pBitmapBkOver->SetCDibRect( ArrowBk );
						m_pBitmapBkOver->Draw( pdc,FALSE );
					}
				}else
				{
					m_pBitmapBk->SetCDibRect( ArrowBk );
					m_pBitmapBk->Draw( pdc,FALSE );
				}

				m_pBitmapAsc->SetCDibRect( ArrowRect );
				m_pBitmapAsc->Draw( pdc,TRUE );
			}
		}
		if( nType == 1 )
		{
			addTop = (m_hight - m_pBitmapDesc->GetHeight())/2;
			ArrowRect.top  = addTop>0 ? addTop:0 ;
			ArrowRect.bottom = ArrowRect.top+m_pBitmapDesc->GetHeight();
			ArrowRect.right  = ArrowRect.left+m_pBitmapDesc->GetWidth();
			ArrowRect.right = ArrowRect.right >  m_arrayItemPos[nItem].pos[1]? m_arrayItemPos[nItem].pos[1]:ArrowRect.right;

			ArrowBk.left = ArrowRect.left;
			ArrowBk.top = 0;
			ArrowBk.bottom = m_hight;
			ArrowBk.right = ArrowBk.left + m_pBitmapAsc->GetWidth();
			ArrowBk.right = ArrowBk.right >  m_arrayItemPos[nItem].pos[1]? m_arrayItemPos[nItem].pos[1]:ArrowBk.right;

			if( ArrowBk.right > ArrowBk.left )
			{
				if( nItem == m_nHoverItem )
				{
					if(m_bLButtonDown)
					{
						m_pBitmapBkDown->SetCDibRect( ArrowBk );
						m_pBitmapBkDown->Draw( pdc,FALSE );
					}
					else
					{
						m_pBitmapBkOver->SetCDibRect( ArrowBk );
						m_pBitmapBkOver->Draw( pdc,FALSE );
					}
				}else
				{
					m_pBitmapBk->SetCDibRect( ArrowBk );
					m_pBitmapBk->Draw( pdc,FALSE );
				}
				m_pBitmapDesc->SetCDibRect( ArrowRect );
				m_pBitmapDesc->Draw( pdc,TRUE );
			}
		}
		if( nType == 0 )
		{	
			ArrowBk.left = ArrowRect.left;
			ArrowBk.top = 0;
			ArrowBk.bottom = m_hight;

			int width = ( m_pBitmapAsc->GetWidth())>(m_pBitmapDesc->GetWidth())?( m_pBitmapAsc->GetWidth()):(m_pBitmapDesc->GetWidth());
			ArrowBk.right = ArrowBk.left + width;
			ArrowBk.right = ArrowBk.right >  m_arrayItemPos[nItem].pos[1]? m_arrayItemPos[nItem].pos[1]:ArrowBk.right;

			if( ArrowBk.right > ArrowBk.left )
			{
				if( nItem == m_nHoverItem )
				{
					if(m_bLButtonDown)
					{
						m_pBitmapBkDown->SetCDibRect( ArrowBk );
						m_pBitmapBkDown->Draw( pdc,FALSE );
					}
					else
					{
						m_pBitmapBkOver->SetCDibRect( ArrowBk );
						m_pBitmapBkOver->Draw( pdc,FALSE );
					}
				}else
				{
					m_pBitmapBk->SetCDibRect( ArrowBk );
					m_pBitmapBk->Draw( pdc,FALSE );
				}
			}
		}
	}
}
void CHeadCtrol::SortArrow( int nItem, int nType )
{
	// »æÖÆ
	CDC* pdc = this->GetDC();
	PaintSortArrow( pdc, nItem, nType );
	ReleaseDC( pdc );
	//ÊÂ¼þ
	if( 2==nType || 1==nType ) 
	{
		::SendMessage( m_hListWnd, SORT_MSG, (WPARAM)nItem, (LPARAM)nType );
	}
}
LRESULT CHeadCtrol::OnScroolHead( WPARAM wParam, LPARAM lParam )
{
	size_t lnsize = m_arrayItemPos.size();
	for(UINT i=0; i<lnsize; i++ )
	{
		m_arrayItemPos[i].pos[0] += int(lParam);
		m_arrayItemPos[i].pos[1] += int(lParam);
		m_arrayItemPos[i].pos[2] += int(lParam);
	}
	this->Invalidate();
	return 1;
}
int CHeadCtrol::GetHeadRight()const
{
	if( m_arrayItemPos.size() > 0)
		return m_arrayItemPos[m_arrayItemPos.size()-1].pos[1];
	else return 0;
}
int CHeadCtrol::GetItemRight(int pos)const
{
	if( pos>( m_arrayItemPos.size() -1) ) return 0;
	return m_arrayItemPos[pos].pos[1];
}
int CHeadCtrol::GetItemLeft(int pos)const
{
	if( pos>( m_arrayItemPos.size() -1) ) return 0;
	return m_arrayItemPos[pos].pos[0];
}

void   CHeadCtrol::SetHeadBitmap( CDibBitmap* pBk,CDibBitmap* pMidLine,CDibBitmap* pAsc,CDibBitmap* pDesc)
{
	m_pBitmapBk = pBk;
	m_pBitmapMidLine = pMidLine;
	m_pBitmapAsc = pAsc;
	m_pBitmapDesc = pDesc;
	ISkinMgr* pSkinMgr	= AfxGetUIManager()->UIGetSkinMgr();
	m_pBitmapBkOver		= pSkinMgr->GetDibBmp("ListHeaderBKOver");
	m_pBitmapMidLineOver= pSkinMgr->GetDibBmp("ListHeaderMidLineOver");
	m_pBitmapBkDown		= pSkinMgr->GetDibBmp("ListHeaderBKDown");
}
void CHeadCtrol::SetHeadTextColor( COLORREF testCol )
{
	m_textCol = testCol;
}
LPARAM CHeadCtrol::OnMouseLeave(WPARAM wp, LPARAM lp)
{
	m_bTrackMouse = false;
	m_nHoverItem = -1;
	Invalidate();
	return 1;
}
