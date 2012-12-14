// MyHeaderCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include ".\myheaderctrl.h"
#include "../Bitmap/DibBitmap.h"
#include "MyListCtrl.h"

// CMyHeaderCtrl

IMPLEMENT_DYNAMIC(CMyHeaderCtrl, CHeaderCtrl)
CMyHeaderCtrl::CMyHeaderCtrl(): m_pBitmapAsc(NULL),
								m_pBitmapBk(NULL),
								m_pBitmapMidLine(NULL),
								m_pBitmapDesc(NULL)
{	
	m_fontColor			= RGB(0x00,0x00,0x00);
	m_bTrackMouse		= false;
	m_nHoverItem		= -1;
	m_bLButtonDown		= false;
}

CMyHeaderCtrl::~CMyHeaderCtrl()
{
	if( m_fHeaderFont.m_hObject != NULL )
	{
		m_fHeaderFont.DeleteObject();
	}
}


BEGIN_MESSAGE_MAP(CMyHeaderCtrl, CHeaderCtrl)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(HDM_LAYOUT, OnLayOut)
END_MESSAGE_MAP()

int CMyHeaderCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( m_fHeaderFont.m_hObject != NULL )
	{
		m_fHeaderFont.CreateFont(
		14,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"宋体");                 // lpszFacename
	//A BIGGER FONT MAKES THE CONTROL BIGGER
		SetFont(&m_fHeaderFont);
	}

	return CHeaderCtrl::OnCreate( lpCreateStruct );
}

void CMyHeaderCtrl::SetHeaderColor(COLORREF colorfont )
{
	m_fontColor = colorfont;
}

void	CMyHeaderCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	static CPoint s_pt(0,0);

	if( s_pt == point )
	{
		return __super::OnMouseMove(nFlags,point);
	}
	s_pt = point;

	if (!m_bTrackMouse)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE;
		_TrackMouseEvent(&tme);
		m_bTrackMouse = TRUE;
	}

	int nItems = GetItemCount();
	CRect rect;
	for(int i = 0; i <nItems; i++)
	{
		GetItemRect(i, &rect);
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

	__super::OnMouseMove(nFlags, point);
}
BOOL CMyHeaderCtrl::SubclassWindow( HWND hwnd )
{
	//m_imgList.Add(&(theApp.skin.m_bListBtnListen),RGB(0,0,0));
	//m_imgList.Add(&(theApp.skin.m_bListBtnDownload),RGB(0,0,0));

	CHeaderCtrl::SubclassWindow(hwnd);
	m_fHeaderFont.CreateFont(
			14,                        // nHeight
			0,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			FW_NORMAL,                 // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			0,                         // cStrikeOut
			ANSI_CHARSET,              // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			"宋体");                 // lpszFacename
		//A BIGGER FONT MAKES THE CONTROL BIGGER
		SetFont(&m_fHeaderFont);
		return TRUE;
}

void CMyHeaderCtrl::SetBitmap(CDibBitmap* pBk,CDibBitmap* pMidLine,CDibBitmap* pAsc,CDibBitmap* pDesc)
{
	m_pBitmapBk			= pBk;
	m_pBitmapAsc		= pAsc;
	m_pBitmapDesc		= pDesc;
	m_pBitmapMidLine	= pMidLine;
	ISkinMgr* pSkinMgr	= AfxGetUIManager()->UIGetSkinMgr();
	m_pBitmapBkOver		= pSkinMgr->GetDibBmp("ListHeaderBK");//Over");
	m_pBitmapMidLineOver= pSkinMgr->GetDibBmp("ListHeaderMidLine");//Over");
	m_pBitmapBkDown		= pSkinMgr->GetDibBmp("ListHeaderBK");//Down");
}

void CMyHeaderCtrl::OnPaint()
{
	CPaintDC dc(this); 
	CRect rect,rectItem;
	GetClientRect(&rect);

	CDC hdcMem;
	hdcMem.CreateCompatibleDC(NULL);
	CBitmap hBitmap;
	hBitmap.CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());
	hdcMem.SelectObject(&hBitmap);

	//CMemDC memDC(&dc, rect);
	int nItems = GetItemCount();

	if( m_pBitmapBk != NULL )
	{
		m_pBitmapBk->SetCDibRect( rect );
		m_pBitmapBk->Draw( &hdcMem,FALSE );
	}

	
	CFont* def_font;
	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 12;
	strcpy(lf.lfFaceName, "宋体");
	font.CreateFontIndirect(&lf);
	def_font = hdcMem.SelectObject(&font);	


	hdcMem.SetBkMode(TRANSPARENT);
	hdcMem.SetTextColor(m_fontColor) ;
			
	for(int i = 0; i <nItems; i++)
	{
		TCHAR buf1[256];
		HD_ITEM hditem1;
		
		hditem1.mask = HDI_TEXT | HDI_FORMAT | HDI_ORDER;
		hditem1.pszText = buf1;
		hditem1.cchTextMax = 255;
		GetItem( i, &hditem1 );	
		GetItemRect(i, &rect);

		if( m_nHoverItem == i )
		{
			if(this->m_bLButtonDown)
			{
				m_pBitmapBkDown->SetCDibRect( rect );
				m_pBitmapBkDown->Draw( &hdcMem,FALSE );
			}
			else
			{
				m_pBitmapBkOver->SetCDibRect( rect );
				m_pBitmapBkOver->Draw( &hdcMem,FALSE );
			}
		}
		if( m_pBitmapMidLine != NULL )
		{
			if(m_nHoverItem == i)
			{
				m_pBitmapMidLineOver->SetCDibRect(CRect(rect.BottomRight().x-1,rect.TopLeft().y,rect.BottomRight().x,rect.BottomRight().y));
				m_pBitmapMidLineOver->Draw(&hdcMem,FALSE);
// 				int start = rect.TopLeft().x == 0 ? 0 : rect.TopLeft().x - 1;
// 				m_pBitmapMidLineOver->SetCDibRect(CRect(start ,rect.TopLeft().y,start + 1,rect.BottomRight().y));
// 				m_pBitmapMidLineOver->Draw(&hdcMem,FALSE);
			}
			else
			{
// 				m_pBitmapMidLine->SetCDibRect(CRect(rect.BottomRight().x-1,rect.TopLeft().y,rect.BottomRight().x+2,rect.BottomRight().y));
// 				m_pBitmapMidLine->Draw(&hdcMem,FALSE);

				m_pBitmapMidLineOver->SetCDibRect(CRect(rect.BottomRight().x-1,rect.TopLeft().y,rect.BottomRight().x,rect.BottomRight().y));
				m_pBitmapMidLineOver->Draw(&hdcMem,FALSE);
			}
		}

		GetItemRect(i, &rectItem);
		//DRAWITEMSTRUCT	DrawItemStruct;
		//		
		//DrawItemStruct.CtlType		= ODT_HEADER;
		//DrawItemStruct.hDC			= dc.GetSafeHdc();
		//DrawItemStruct.itemAction	= ODA_DRAWENTIRE; 
		//DrawItemStruct.hwndItem 	= GetSafeHwnd(); 
		//DrawItemStruct.rcItem	= rectItem;
		//DrawItemStruct.itemID	= i;*/
////		DrawItem(&DrawItemStruct);
		
		UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_TOP |DT_CENTER | DT_END_ELLIPSIS | DT_VCENTER;

		rectItem.DeflateRect(2,2,2,2);
		
		TCHAR buf[256];
		HD_ITEM hditem;
		
		hditem.mask = HDI_TEXT | HDI_FORMAT | HDI_ORDER ;
		hditem.pszText = buf;
		hditem.cchTextMax = 255;
		GetItem( i, &hditem );
		
		CRect calRect = rectItem;
		hdcMem.DrawText(buf, &calRect, DT_CALCRECT | DT_CENTER |DT_VCENTER);
		hdcMem.DrawText(buf, &rectItem, uFormat);
		

		if( i == m_pList->m_iSortItem )
		{
			CDibBitmap * pBmp = NULL;

			if( !m_pList->m_bAsc )
			{
				pBmp = m_pBitmapAsc;
			}
			else
			{
				pBmp = m_pBitmapDesc;
			}

			CSize imaSize;
			CPoint p;

			imaSize = pBmp->GetDimensions();
			p.x = calRect.right + 2 + rectItem.Width() / 2 - calRect.Width() / 2;
			p.y = rectItem.CenterPoint().y - imaSize.cy / 2;
			
			if(p.x  < rectItem.right)
			{
				if(p.x + imaSize.cx > rectItem.right)
				{
					imaSize.cx = rectItem.right - p.x;
				}
				pBmp->SetCDibRect(CRect(p, imaSize));
				pBmp->Draw(&hdcMem,  true);
			}
		}
	}

	GetClientRect(&rect);
	dc.BitBlt(rect.TopLeft().x,rect.TopLeft().y,rect.Width(),rect.Height(),&hdcMem,0,0,SRCCOPY);

	hdcMem.SelectObject(def_font);
	font.DeleteObject();
}

BOOL CMyHeaderCtrl::OnEraseBkgnd(CDC* pDC)
{
	return true;
}

LPARAM  CMyHeaderCtrl::OnMouseLeave(WPARAM wp, LPARAM lp)
{
	m_bTrackMouse = false;
	
	m_nHoverItem = -1;
	Invalidate();
	return 1;
}

void CMyHeaderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = true;
	Invalidate();
	__super::OnLButtonDown(nFlags, point);
}

void CMyHeaderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDown = false;
	Invalidate();
	__super::OnLButtonUp(nFlags, point);
}

LRESULT CMyHeaderCtrl::OnLayOut (WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = CHeaderCtrl::DefWindowProc(HDM_LAYOUT, 0, lParam);   

	HD_LAYOUT &hdl = *( HD_LAYOUT * ) lParam;   

	RECT *prc = hdl.prc;   
	WINDOWPOS *pwpos = hdl.pwpos;   

	const int nHeight = 20;
	pwpos->cy = nHeight;
	prc->bottom = nHeight;

	return lResult;
}
