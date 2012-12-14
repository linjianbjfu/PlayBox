// MyTreeCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "MyTreeCtrl.h"
#include "./skinscroll/coolsb_detours.h"
#include "../Bitmap/DibBitmap.h"

// CMyTreeCtrl
extern LRESULT HandleCustomDraw(UINT ctrlid, NMCSBCUSTOMDRAW *nm,CDibBitmap * bmpV, CDibBitmap * bmpH);
IMPLEMENT_DYNAMIC(CMyTreeCtrl, CTreeCtrl)


CMyTreeCtrl::CMyTreeCtrl( CDibBitmap* pVert, CDibBitmap* pHorz)
{
//	m_colorSelectItemBK		= RGB(0xAD,0xC2,0xDD);
	m_colorSelectItemBK		= RGB(0x00,0x128,0x00);
	m_colorSelectItemText	= RGB(0x00,0x00,0x00);
	m_colorNormalItemText	= RGB(0x00,0x00,0x00);

	m_pBitmapScrollVert		= pVert;
	m_pBitmapScrollHorz		= pHorz;

	m_bInit = false;
	m_bTrackMouse = false;

	m_ImageList.Create(16, 16, ILC_MASK | ILC_COLORDDB, 1, 1);

}

CMyTreeCtrl::~CMyTreeCtrl()
{
	ClearData();
}


BEGIN_MESSAGE_MAP(CMyTreeCtrl, CTreeCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomdraw )
	ON_NOTIFY_REFLECT(NM_COOLSB_CUSTOMDRAW, OnCoolSBCustomdraw )
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnNMRclick)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()


HTREEITEM CMyTreeCtrl::InsertItem( const string& strText,const string& strImageName,HTREEITEM hParent,HTREEITEM hAfter)
{
	TV_INSERTSTRUCT tvinsert;
	tvinsert.hParent			= hParent;
	tvinsert.hInsertAfter		= hAfter;
	tvinsert.item.mask			= TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT ;
	tvinsert.item.hItem			= NULL;
	tvinsert.item.state			= 0;
	tvinsert.item.stateMask		= 0;
	tvinsert.item.cchTextMax	= 0;

	tvinsert.item.pszText		= (LPTSTR)strText.c_str();
	tvinsert.item.iImage		= m_mapBitmap2Index[strImageName];
	tvinsert.item.iSelectedImage = m_mapBitmap2Index[strImageName];	

	return CTreeCtrl::InsertItem(&tvinsert);
}

BOOL CMyTreeCtrl::OnEraseBkgnd(CDC* pDC)
{
	return true;	
}

void CMyTreeCtrl::OnPaint()
{
	CPaintDC dc(this); 
	Init();
	CRect rcClip, rcClient;
	dc.GetClipBox( &rcClip );
	GetClientRect(&rcClient);

	// Create a compatible memory DC 
	CDC memDC;
	memDC.CreateCompatibleDC( &dc );

	// Select a compatible bitmap into the memory DC
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
	memDC.SelectObject( &bitmap );

	// First let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );
	dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(),
				rcClip.Height(), &memDC,
				rcClip.left, rcClip.top, SRCCOPY );
}

void CMyTreeCtrl::OnDestroy()
{
	if( m_bInit )
	{
		UninitializeCoolSB( m_hWnd );
	}
	__super::OnDestroy();
}

void CMyTreeCtrl::OnSize(UINT nType, int cx, int cy)
{
	CTreeCtrl::OnSize(nType, cx, cy);

	CRect rc;
	GetWindowRect(&rc);
	ScreenToClient(rc);

	int size = 15;
	if( m_pBitmapScrollVert  != NULL )
	{
		size = m_pBitmapScrollVert->GetHeight() / 3;
	}

	int nItemsHeight = GetCount () * GetItemHeight ();

	size = nItemsHeight == 0 ? 15 : nItemsHeight;

// 	if(rc.Height() >= size && rc.Width() >=size)
	if (rc.Height () <= size && rc.Width () <= size)
	{
		m_bInit = false;
		ShowScrollBar (SB_VERT, TRUE);
		Init();
	}
	else
	{
		UninitializeCoolSB(m_hWnd);
		ShowScrollBar (SB_VERT, FALSE);
	}

	InvalidateRect(CRect(rc.left + 1, rc.top +1 , rc.left + 2, rc.top + 2));
}

void CMyTreeCtrl::OnCoolSBCustomdraw(NMHDR* pNMHDR, LRESULT* pResult )
{
	NMCSBCUSTOMDRAW* pNMCD = reinterpret_cast<NMCSBCUSTOMDRAW*>(pNMHDR);	
	if( m_pBitmapScrollHorz != NULL && m_pBitmapScrollVert != NULL )
	{
		*pResult = HandleCustomDraw(0, (NMCSBCUSTOMDRAW*)pNMHDR, m_pBitmapScrollVert,m_pBitmapScrollHorz );
	}
}

void CMyTreeCtrl::OnCustomdraw ( NMHDR* pNMHDR, LRESULT* pResult )
{
 	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
 	// Take the default processing unless we set this to something else below.
 	*pResult = CDRF_DODEFAULT;

	HTREEITEM item = GetSelectedItem();
//	bool bDrop = GetItemState( item,TVIS_DROPHILITED ) & TVIS_DROPHILITED ;

	CRect rc, rctext;
	GetItemRect(item, &rctext, true);
	GetItemRect(item, &rc, false);
	CString text = GetItemText(item);

	int m = 0;
	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_ITEMPOSTPAINT :
		if(rc == pLVCD->nmcd.rc)
		{
			CRect newRc(rc);
			newRc.left = rctext.left;
			HBRUSH hBrush = CreateSolidBrush(pLVCD->clrTextBk);
		//	::FillRect(pLVCD->nmcd.hdc, &newRc, hBrush);
			DeleteObject(hBrush);

			
			::SetBkMode(pLVCD->nmcd.hdc, TRANSPARENT);
			newRc.left+=2;
			if ( rc == pLVCD->nmcd.rc )
			{				
				::SetTextColor(pLVCD->nmcd.hdc, m_colorSelectItemText);
			}else
				::SetTextColor(pLVCD->nmcd.hdc, m_colorNormalItemText);
			DrawText(pLVCD->nmcd.hdc, text, text.GetLength(), &newRc, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX);
		}
		break;	
	case CDDS_PREPAINT :
		*pResult = CDRF_NOTIFYITEMDRAW  ;
		break ;
	case CDDS_ITEMPREPAINT :
		if ( rc == pLVCD->nmcd.rc /*|| bDrop*/ )
		{
			pLVCD->clrTextBk = m_colorSelectItemBK;//RGB(0x0,0x0,0xFF);
			pLVCD->clrText   = m_colorSelectItemText;
			*pResult = CDRF_NOTIFYPOSTPAINT;
		}
		else
		{
			pLVCD->clrText   = m_colorNormalItemText;
		}
		break ;
	default:
		*pResult = CDRF_DODEFAULT;
	}	
}

void CMyTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// Select the item that is at the point myPoint.
	UINT uFlags;
	HTREEITEM hItem = HitTest(point, &uFlags);

	if(hItem != NULL)
	{
		SelectItem(hItem);
	}

	CWnd::OnRButtonDown(nFlags, point);
}

void CMyTreeCtrl::SetSelectItemBKColor(COLORREF color)
{
	m_colorSelectItemBK = color;
}

void CMyTreeCtrl::SetSelectItemTextColor(COLORREF color)
{
	m_colorSelectItemText = color;
}

void CMyTreeCtrl::SetNormalItemTextColor(COLORREF color)
{
	m_colorNormalItemText = color;
}

void CMyTreeCtrl::SetBackColor(COLORREF color)
{
	SetBkColor( color );
}

void CMyTreeCtrl::SetScrollBitmap( CDibBitmap* pBitVert,CDibBitmap* pBitHorz )
{
	m_pBitmapScrollVert = pBitVert;
	m_pBitmapScrollHorz = pBitHorz;
}

int CMyTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}
	
	m_treeFont.CreateFont(
		12,                        // nHeight
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
		"宋体");  

	SetFont(&m_treeFont);
	SetItemHeight(18);
	return 0;
}

void CMyTreeCtrl::Init()
{
	if( !m_bInit )
	{			
		m_bInit = true;
		int w,h;
		w = h = 15;
	
		if( m_pBitmapScrollHorz && m_pBitmapScrollVert )
		{
			h = m_pBitmapScrollVert->GetHeight() / 3;
			w = m_pBitmapScrollVert->GetWidth() / 6;
		}

		InitializeCoolSB(m_hWnd);
		CoolSB_SetStyle(m_hWnd, SB_BOTH, CSBS_HOTTRACKED);
		CoolSB_SetSize(m_hWnd, SB_BOTH, h, w);
		::CoolSB_SetMinThumbSize(m_hWnd, SB_BOTH, 8);
	}
}

void CMyTreeCtrl::ClearData()
{
	if( m_treeFont.m_hObject != NULL )
	{
		m_treeFont.DeleteObject();
	}

	while(m_ImageList.GetImageCount() > 0)
	{
		m_ImageList.Remove(0);
	}

	for(vector<CBitmap*>::iterator iter=m_vecBitmap.begin(); iter!=m_vecBitmap.end(); iter++)
	{
		(*iter)->DeleteObject();
		delete (*iter);
	}
	m_vecBitmap.clear();
	m_mapBitmap2Index.clear();
}

void CMyTreeCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);	

	HTREEITEM hTreeItem = HitTest(pt);
	if( hTreeItem != NULL)// && !ItemHasChildren(hTreeItem) )
	{
		SelectItem( hTreeItem );
		ClickTreeItem( hTreeItem );
	}
	*pResult = 0;
}

void CMyTreeCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);	

	HTREEITEM hTreeItem = HitTest(pt);
	if( hTreeItem != NULL )// && !ItemHasChildren(hTreeItem) )
	{
		DClickTreeItem( hTreeItem );
	}

	*pResult = 0;
}

void CMyTreeCtrl::OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);	

	HTREEITEM hTreeItem = HitTest(pt);
	if( hTreeItem != NULL )// && !ItemHasChildren(hTreeItem))
	{
		RClickTreeItem( hTreeItem );
	}

	*pResult = 0;
}

void CMyTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	HTREEITEM htree = HitTest(point);
	
	if (!m_bTrackMouse)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE;
		_TrackMouseEvent(&tme);
		m_bTrackMouse = TRUE;
	}

	if( htree == NULL )
	{
		if(	m_iHoverItem != NULL )
		{
			CRect rcHover;
			GetItemRect( m_iHoverItem, &rcHover, FALSE );
			InvalidateRect(rcHover);
			m_iHoverItem = NULL;
		}
		return __super::OnMouseMove( nFlags,point );
	}else
	{
		if( m_iHoverItem != htree )
		{
			//更新原来的item
			CRect rcHover;
			GetItemRect( m_iHoverItem, &rcHover, FALSE );
			InvalidateRect(rcHover);
			//更新现在的item
			m_iHoverItem = htree;
			GetItemRect( m_iHoverItem, &rcHover, FALSE );
			InvalidateRect(rcHover);
		}
	}	
	__super::OnMouseMove(nFlags, point);
}

LPARAM CMyTreeCtrl::OnMouseLeave(WPARAM wp, LPARAM lp)
{
	if( m_iHoverItem != NULL )
	{
		CRect rcHover;
		GetItemRect( m_iHoverItem, &rcHover, FALSE );
		InvalidateRect(rcHover);
		m_iHoverItem = NULL;
	}
	return 0;
}

