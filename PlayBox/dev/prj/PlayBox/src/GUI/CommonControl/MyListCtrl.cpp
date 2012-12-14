
#include "stdafx.h"
#include ".\mylistctrl.h"
#include "../Bitmap/DibBitmap.h"
#include "./skinscroll/coolsb_detours.h"
#include <algorithm>
#include "HanziToPinyin.h"

#define ITEM_HEIGHT 80
static COLORREF clrOddItemBK  = RGB(51,51,51);
static COLORREF clrEvenItemBK = RGB(45,44,44);

// CMyTreeCtrl
extern LRESULT HandleCustomDraw(UINT ctrlid, NMCSBCUSTOMDRAW *nm,CDibBitmap * bmpV, CDibBitmap * bmpH);
#include "../util/CBufferDC.h"

CMyListCtrl::CMyListCtrl(void):m_pBitmapScrollHorz(NULL),
							   m_pBitmapScrollVert(NULL),
							   m_iOffset(0),
							   m_iCurrentCursorItem(-1),
							   m_iCurrentCursorSubItem(-1),
							   m_iOverImageItem(-1),
							   m_iOverImageSubItem(-1),
							   m_iSortItem(-1),
							   m_bAsc(false),
							   m_bTrackMouseEvent(FALSE),
							   m_nKeySubItem(1)

{
// 	m_bDrawItemTopLine	= false;
	m_clrOddItemBK		= RGB(51,51,51);		
	m_clrEvenItemBK		= RGB(45,44,44);		
	m_clrNormalItemText = RGB(255,0,0);
	m_clrSelItemBK		= RGB(128,128,128);
	m_clrSelItemText	= RGB(255,0,0);
	
	m_font.CreatePointFont(90,_T("宋体"));
	m_bDrawHoverRow = true;
	m_bInit = false;
}

CMyListCtrl::~CMyListCtrl(void)
{
	m_font.DeleteObject();
}

BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclickList)
	ON_NOTIFY_REFLECT(NM_COOLSB_CUSTOMDRAW, OnCoolSBCustomdraw )
	ON_WM_ERASEBKGND()  
	ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

void CMyListCtrl::OnDestroy()
{
	m_tooltip.DestroyWindow();
	if( m_bInit )
	{
		UninitializeCoolSB( m_hWnd );
	}
	__super::OnDestroy();
}


void CMyListCtrl::SetColumnInfo(const vector<MYLIST_COLUMN_INFO>& vecColumn )
{
	m_mapColumnInfo.clear();
	for( size_t idx = 0;idx <vecColumn.size();idx++ )
	{
		m_mapColumnInfo[vecColumn[idx].iColumnIndex] = vecColumn[idx];
	}
}

void CMyListCtrl::SetColor( COLORREF clrOddItemBK,COLORREF clrEvenItemBK,COLORREF clrNormalItemText,COLORREF clrSelItemBK,COLORREF clrSelItemText)
{
	m_clrOddItemBK		= clrOddItemBK;
	m_clrEvenItemBK		= clrEvenItemBK;
	m_clrNormalItemText = clrNormalItemText;
	m_clrSelItemBK		= clrSelItemBK;
	m_clrSelItemText	= clrSelItemText;
}

void CMyListCtrl::SetScrollBitmap(CDibBitmap* pBitmapVert,CDibBitmap* pBitmapHorz)
{
	m_pBitmapScrollHorz = pBitmapHorz;
	m_pBitmapScrollVert = pBitmapVert;

	int w,h;
	w = h = 15;
	if( m_pBitmapScrollHorz && m_pBitmapScrollVert )
	{
		h = m_pBitmapScrollVert->GetHeight() / 3;
		w = m_pBitmapScrollVert->GetWidth() / 6;
	}
	//InitializeCoolSB(m_hWnd);
	//CoolSB_SetStyle(m_hWnd, SB_BOTH, CSBS_HOTTRACKED);
	CoolSB_SetSize(m_hWnd, SB_BOTH, h, w);
	::CoolSB_SetMinThumbSize(m_hWnd, SB_BOTH, 8);

}

void CMyListCtrl::CreateControl( int iID,CWnd* pParent )
{
	Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|LVS_SHOWSELALWAYS|LVS_REPORT|LVS_OWNERDRAWFIXED,CRect(0,0,0,0),pParent,iID);
	ListView_SetExtendedListViewStyleEx ( m_hWnd,LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT );

	m_tooltip.m_pClientWnd = this->GetParent();//AfxGetMainWindow();
	m_tooltip.m_hParent	 = m_hWnd;
	m_tooltip.CreateEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST,TOOLTIPS_CLASS,"",TTS_ALWAYSTIP,CRect(0,0,0,0),NULL,0);	
	m_tooltip.m_clrBK = RGB(255, 255, 225);
}

int CMyListCtrl::OnCreate(LPCREATESTRUCT lp)
{
	InitColumnInfo();
	LoadSkin();

	if( __super::OnCreate(lp) == -1 )
	{
		return -1;
	}

	for(map<int,MYLIST_COLUMN_INFO>::iterator it = m_mapColumnInfo.begin();
		it != m_mapColumnInfo.end();
		it++)
	{
		MYLIST_COLUMN_INFO& columnInfo = it->second;
		if( columnInfo.enumColumnType == MYLIST_COLUMN_INFO::COLUMN_TYPE_IMAGE)
		{
			InsertColumn( columnInfo.iColumnIndex, columnInfo.strColumnName.c_str(), LVCFMT_IMAGE,columnInfo.iColumnWidth  );
		}
		else
		{
			LVCOLUMN lvColumn;
			lvColumn.mask		= LVCF_IMAGE | LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
			lvColumn.fmt		= LVCFMT_LEFT; //LVCFMT_IMAGE | LVCF_TEXT;
			//lvColumn.iSubItem	= columnInfo.iColumnIndex;
			lvColumn.cx			= columnInfo.iColumnWidth;
			lvColumn.pszText	= (char*) columnInfo.strColumnName.c_str();

			InsertColumn( columnInfo.iColumnIndex,&lvColumn);
		}
	}
	AfxGetUIManager()->UIGetSkinMgr()->AddSkinWnd( this );
	return 1;
}

void CMyListCtrl::OnPaint()
{
	if(!m_bInit )
	{
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
		m_bInit = true;
	}

    CRect rectClient;
	GetClientRect(rectClient);
	CRect rectClip;
	GetUpdateRect(rectClip,FALSE);
	if( rectClip.right < rectClient.right )
	{
		rectClip.right = rectClient.right;
	}

	int width = 0;
	for(int i = 0 ; i<m_mapColumnInfo.size(); i++)
		width += GetColumnWidth(i);

	if(rectClip.left >= width)
		rectClip.left = width -1;

	InvalidateRect( rectClip );

	__super::OnPaint();
}


BOOL CMyListCtrl::OnEraseBkgnd(CDC* pDC )
{
	CRect rcClient;
	GetClientRect(rcClient);
	CRect rectHeader;
	GetHeaderCtrl()->GetWindowRect(rectHeader);
	bool bNeedDraw = true;

	int iItemCount = GetItemCount();

	if (iItemCount > 0)
	{
		CRect rectItem;
		GetItemRect(iItemCount-1,rectItem,LVIR_BOUNDS);
		rcClient.top = rectItem.bottom;

		COLORREF clr;
		if (iItemCount % 2)
			clr = clrOddItemBK;
		else
			clr = clrEvenItemBK;

		pDC->FillSolidRect (&rcClient, clr);//RGB(45,44,44));

		return TRUE;
	}
/*  
	if( iItemCount > 0 )
	{
		CRect rectItem;
		GetItemRect(iItemCount-1,rectItem,LVIR_BOUNDS);
		if( rectItem.bottom < rcClient.bottom )
		{
			rcClient.top = rectItem.bottom;			
		}
		else
		{
			bNeedDraw = false;
		}
	}	
	else
	{
		rcClient.top = rectHeader.Height();
	}

	if( bNeedDraw )
	{
		int id =iItemCount+1;
		for(int btop=rcClient.top ;btop<rcClient.bottom;btop+=ITEM_HEIGHT)
		{
			if(id%2)
			{
				pDC->FillSolidRect(CRect(rcClient.left,btop, rcClient.right ,btop+ITEM_HEIGHT),m_clrEvenItemBK);
			}else
			{
				pDC->FillSolidRect(CRect(rcClient.left,btop, rcClient.right ,btop+ITEM_HEIGHT),m_clrOddItemBK);
			}
			id++;
		}
	}
*/
// 	rcClient.top = 0;
// 	rcClient.top += (rectHeader.bottom - rectHeader.top );
// 	rcClient.bottom = rcClient.top/* + 2*/;
// 	pDC->FillSolidRect(rcClient, m_clrOddItemBK );

	return TRUE;
}

void CMyListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpmis)
{
	lpmis->itemHeight = ITEM_HEIGHT;
}


void CMyListCtrl::HideToolTip()
{
	m_tooltip.HideTooTips();
}


void CMyListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	static CPoint s_pt(0,0);

//	TRACE("%d,%d",point.x,point.y);

	if( s_pt == point )
	{
//		__super::OnMouseMove( nFlags,point );
		return;
	}
	s_pt = point;

	HideToolTip();

	LVHITTESTINFO lvhti;
	lvhti.pt = point;
	int ret = SubItemHitTest(&lvhti);
	if( ret == -1 )
	{
		if( m_iCurrentCursorItem != -1 )
		{
			CRect r;
			GetItemRect(m_iCurrentCursorItem, &r,LVIR_BOUNDS);
			InvalidateRect(&r, true);
		}
		m_iCurrentCursorItem = m_iCurrentCursorSubItem = -1;
		m_iOverImageItem	= -1;
		m_iOverImageSubItem	= -1;
		__super::OnMouseMove( nFlags,point );
		return;
	}

	if( m_iCurrentCursorItem != -1 )
	{
		CRect r;
		GetItemRect(m_iCurrentCursorItem, &r,LVIR_BOUNDS);
		InvalidateRect(&r, true);
	}
	m_iCurrentCursorItem	= lvhti.iItem;
	m_iCurrentCursorSubItem	= lvhti.iSubItem;
	if( m_iCurrentCursorItem != -1 )
	{
		CRect r;
		GetItemRect(m_iCurrentCursorItem, &r,LVIR_BOUNDS);
		InvalidateRect(&r, true);
	}

	CRect rectSubItem;
	GetSubItemRect( m_iCurrentCursorItem,m_iCurrentCursorSubItem,LVIR_BOUNDS,rectSubItem);

	if( NeedChangeCursor( point ) )
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32649)));			
		if( m_iOverImageItem == -1 && m_iOverImageSubItem == -1 )
		{
			if( m_iCurrentCursorSubItem == 0 )
			{
				rectSubItem.right = rectSubItem.left + GetColumnWidth(0);
			}

			InvalidateRect( rectSubItem );
			m_iOverImageItem	= lvhti.iItem;
			m_iOverImageSubItem	= lvhti.iSubItem;
		}
		ShowSubItemToolTip( m_iOverImageItem,m_iOverImageSubItem);
	}
	else
	{
		if( m_iOverImageItem != -1 && m_iOverImageSubItem != -1 )
		{
			GetSubItemRect( m_iOverImageItem,m_iOverImageSubItem,LVIR_BOUNDS,rectSubItem);
			if( m_iCurrentCursorSubItem == 0 )
			{
				rectSubItem.right = rectSubItem.left + GetColumnWidth(0);
			}
			InvalidateRect( rectSubItem );			
			m_iOverImageItem	= -1;
			m_iOverImageSubItem	= -1;
			::SetCursor(::LoadCursor(NULL, IDC_ARROW));	
		}	
		ShowItemToolTip( m_iCurrentCursorItem,m_iCurrentCursorSubItem);
	}
	if(!m_bTrackMouseEvent) 
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE; 
		tme.dwHoverTime = 1;
		m_bTrackMouseEvent = _TrackMouseEvent(&tme); 
	}
	__super::OnMouseMove( nFlags,point );

}

LRESULT CMyListCtrl::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTrackMouseEvent = FALSE;
	if( m_iCurrentCursorItem != -1 )
	{
		CRect r;
		GetItemRect(m_iCurrentCursorItem, &r,LVIR_BOUNDS);
		InvalidateRect(&r, true);
	}
	m_iCurrentCursorItem = -1;
	m_iCurrentCursorSubItem = -1;
	m_iOverImageItem = -1;
	m_iOverImageSubItem = -1;

	HideToolTip();
	return DefWindowProc(WM_MOUSELEAVE, wparam, lparam); 
}

void CMyListCtrl::OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	m_bAsc		= !m_bAsc;
	m_iSortItem = pNMListView->iSubItem;

	SortItems(MyCompareProc, (LPARAM)this);
	for(int i = 0 ; i<GetItemCount(); i++)
	{
		SetItemData(i, i);
	}

	*pResult = 0;
}

int CALLBACK CMyListCtrl::MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CMyListCtrl * pWnd = (CMyListCtrl *)lParamSort;
	if(pWnd == NULL)
		return 0;
	return pWnd->SortFunction( (int)lParam1,(int)lParam2);
}

void CMyListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct )
{
/*
	CRect rcWnd;
	GetClientRect(&rcWnd);
	
	CRect rcItem(lpDrawItemStruct->rcItem);
	rcItem.right = rcWnd.right;
	CBufferDC* pDC = new CBufferDC( lpDrawItemStruct->hDC,rcItem);

	int nItem = lpDrawItemStruct->itemID;

	// Save dc state
	int nSavedDC = pDC->SaveDC();

	LV_ITEM lvi;
	lvi.mask = LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;     // get all state flags
	GetItem(&lvi);

	// Should the item be highlighted
	BOOL bHighlight =( (lvi.state & LVIS_DROPHILITED) || ( (lvi.state & LVIS_SELECTED)&& ((GetFocus() == this)|| (GetStyle() & LVS_SHOWSELALWAYS)	)	)		);

	// Get rectangles for drawing
	CRect rcBounds;
	GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
		
	CRect rcHighlight;

	rcHighlight = rcBounds;
	rcHighlight.left = rcWnd.left;
	rcHighlight.right = rcWnd.right;

	// Draw the background color
	SetItemTextColor( pDC,bHighlight,nItem );
	SetItemBkColor( pDC,bHighlight,nItem ,rcHighlight );
	
	int iColumnCount = GetHeaderCtrl()->GetItemCount();	

	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;
	
	CFont* pFont = pDC->SelectObject(&m_font);
	m_iOffset = pDC->GetTextExtent(_T(" "), 1 ).cx / 2;

	for( int i = 0;i < iColumnCount;i++ )
	{
		GetColumn( i,&lvc );
		rcBounds.right = rcBounds.left + lvc.cx;
		if( m_mapColumnInfo[i].enumColumnType == MYLIST_COLUMN_INFO::COLUMN_TYPE_IMAGE )
		{
			DrawImageColumn( pDC,nItem,i,rcBounds,bHighlight);
		}
		else
		{
			DrawTextColumn( pDC,nItem,i,rcBounds );
		}
		rcBounds.left = rcBounds.right;
	}

	pDC->SelectObject(pFont);
	// Restore dc

	pDC->RestoreDC( nSavedDC );

	delete pDC;
*/
}


void CMyListCtrl::SetItemTextColor( CDC* pDC,BOOL bHighlight,int iItem) 
{
	// Draw the background color
	if( bHighlight )
	{
		pDC->SetTextColor( m_clrSelItemText);
	}
	else
	{
		pDC->SetTextColor( m_clrNormalItemText);	
	}
}

void CMyListCtrl::SetItemBkColor(CDC* pDC,BOOL bHighlight,int iItem,const CRect& rect)
{
	// Draw the background color
	if( bHighlight )
	{
		pDC->SetBkColor( m_clrSelItemBK);
		pDC->FillRect(rect, &CBrush( m_clrSelItemBK));
	}
	else
	{
		vector<int>::iterator ite=find(m_vecSearchedItems.begin(),m_vecSearchedItems.end(),iItem);
		if (ite!=m_vecSearchedItems.end())
		{
			pDC->SetBkColor( m_colorSearchHightLight);
			pDC->FillRect(rect, &CBrush( m_colorSearchHightLight));
		}
		else
		{
			if( iItem % 2 )
			{
				pDC->SetBkColor( m_clrOddItemBK);
				pDC->FillRect(rect, &CBrush( m_clrOddItemBK));
			}
			else
			{
				pDC->SetBkColor( m_clrEvenItemBK );
				pDC->FillRect(rect, &CBrush( m_clrEvenItemBK ));
			}
		}
	}
/*
	if( m_bDrawItemTopLine )
	{
		//Item上画一条分割线
		CRect rcTopLine = rect;
		rcTopLine.bottom = rcTopLine.top + 1;
		pDC->FillRect(rcTopLine, &CBrush(RGB(241,246,249)));
	}
*/
	if( !bHighlight && m_bDrawHoverRow && m_iCurrentCursorItem == iItem )
	{
		CDibBitmap *pDib = ::AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("ListHoverBK");
		pDib->SetCDibRect(rect);
		pDib->Draw(pDC, true);
	}
}

void CMyListCtrl::DrawTextColumn(CDC* pDC,int iItem,int iColumn,const CRect& rectColumn )
{
	CString sLabel = GetItemText(iItem, iColumn);
	if (sLabel.GetLength() == 0)
	{
		return;
	}

	// Get the text justification
	UINT nJustify = DT_LEFT;
	CRect rcLabel = rectColumn;
	rcLabel.left += m_iOffset;
	rcLabel.right -= m_iOffset;

	pDC->SetBkMode(TRANSPARENT);

	pDC->DrawText(sLabel, -1, rcLabel, nJustify | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS);

	if( m_mapColumnInfo[iColumn].bOperatorBtn )
	{
		LV_ITEM lvi;
		lvi.mask		= LVIF_STATE;
		lvi.iItem		= iItem;
		lvi.iSubItem	= 0;
		lvi.stateMask	= 0xFFFF;   
		GetItem(&lvi);

		BOOL bHighlight =( (lvi.state & LVIS_DROPHILITED) || ( (lvi.state & LVIS_SELECTED)&& ((GetFocus() == this)|| (GetStyle() & LVS_SHOWSELALWAYS)	)	)		);

		if( !bHighlight )
		{
			return;
		}

		CRect rc = CRect(rectColumn.right-20, rectColumn.top, rectColumn.right, rectColumn.bottom);
		if( m_iOverImageItem == iItem && m_iOverImageSubItem == iColumn )
		{
			DrawImage(  m_mapColumnInfo[iColumn].ColumnOperatorImage.pBitmapOver,rc,pDC );
		}
		else
		{
			DrawImage(  m_mapColumnInfo[iColumn].ColumnOperatorImage.pBitmapNormal,rc,pDC );
		}
	}
}

BOOL CMyListCtrl::NeedChangeCursor( CPoint& pt)
{
	return FALSE;
}

void  CMyListCtrl::DrawImageColumn(CDC* pDC,int iItem,int iColumn,const CRect& rect,BOOL bSelected )
{
	if( m_iOverImageItem == iItem && m_iOverImageSubItem == iColumn )
	{
		DrawImage( m_mapColumnInfo[iColumn].ColumnImage.pBitmapOver,rect,pDC);
		return;
	}

	if( bSelected )
	{
		DrawImage( m_mapColumnInfo[iColumn].ColumnImage.pBitmapSelected,rect,pDC);
		return;
	}	
	DrawImage( m_mapColumnInfo[iColumn].ColumnImage.pBitmapNormal,rect,pDC);
}

BOOL CMyListCtrl::PointInSubitemImage(CPoint& pt)
{
	MYLIST_COLUMN_INFO columnInfo;
	CPoint pointImage;
	CRect  rectSubItem;

	if( m_iCurrentCursorItem == -1 || m_iCurrentCursorSubItem == -1 )
	{
		return FALSE;
	}

	GetSubItemRect( m_iCurrentCursorItem,m_iCurrentCursorSubItem,LVIR_BOUNDS,rectSubItem);

	columnInfo			= m_mapColumnInfo[m_iCurrentCursorSubItem];
	rectSubItem.right	= rectSubItem.left + GetColumnWidth(m_iCurrentCursorSubItem);
	pointImage			= rectSubItem.CenterPoint();
	pointImage.x		= pointImage.x - columnInfo.ColumnImage.pBitmapNormal->GetWidth() /2;
	pointImage.y		= pointImage.y - columnInfo.ColumnImage.pBitmapNormal->GetHeight() /2;

	if( PtInRect( CRect(pointImage,CSize(columnInfo.ColumnImage.pBitmapNormal->GetWidth(),columnInfo.ColumnImage.pBitmapNormal->GetHeight() ) ),pt))
	{
		pt.x = pointImage.x;
		pt.y = pointImage.y + columnInfo.ColumnImage.pBitmapNormal->GetHeight();
		return TRUE;
	}
	return FALSE;
}

BOOL CMyListCtrl::PointInOperatorBtn(CPoint& pt)
{
	LV_ITEM lvi;
	lvi.mask		= LVIF_STATE;
	lvi.iItem		= m_iCurrentCursorItem;
	lvi.iSubItem	= m_iCurrentCursorSubItem;
	lvi.stateMask	= 0xFFFF;   
	GetItem(&lvi);

	BOOL bHighlight =( (lvi.state & LVIS_DROPHILITED) || ( (lvi.state & LVIS_SELECTED)&& ((GetFocus() == this)|| (GetStyle() & LVS_SHOWSELALWAYS)	)	)		);

	if( !bHighlight )
	{
		return FALSE;
	}

	if( m_iCurrentCursorItem == -1 || m_iCurrentCursorSubItem == -1 )
	{
		return FALSE;
	}	

	MYLIST_COLUMN_INFO columnInfo;
	columnInfo			= m_mapColumnInfo[m_iCurrentCursorSubItem];
	if( !columnInfo.bOperatorBtn )
	{
		return FALSE;
	}

	CPoint pointImage;
	CRect  rectSubItem;

	GetSubItemRect( m_iCurrentCursorItem,m_iCurrentCursorSubItem,LVIR_BOUNDS,rectSubItem);
	
	rectSubItem.right	= rectSubItem.left + GetColumnWidth(m_iCurrentCursorSubItem);
	rectSubItem.left	= rectSubItem.right - 20;
	pointImage			= rectSubItem.CenterPoint();
	pointImage.x		= pointImage.x - columnInfo.ColumnOperatorImage.pBitmapNormal->GetWidth() /2;
	pointImage.y		= pointImage.y - columnInfo.ColumnOperatorImage.pBitmapNormal->GetHeight() /2;

	if( PtInRect( CRect(pointImage,CSize(columnInfo.ColumnOperatorImage.pBitmapNormal->GetWidth(),columnInfo.ColumnOperatorImage.pBitmapNormal->GetHeight() ) ),pt))
	{
		pt.x = pointImage.x;
		pt.y = pointImage.y + columnInfo.ColumnOperatorImage.pBitmapNormal->GetHeight();
		return TRUE;
	}
	return FALSE;
}

void CMyListCtrl::DrawImage( CDibBitmap* pDibBitmap, const CRect &r, CDC* memDC )
{		
	if( !pDibBitmap )
	{
		return;
	}
	CPoint pt = r.CenterPoint();
	pt.x = pt.x - pDibBitmap->GetWidth() /2;
	pt.y = pt.y - pDibBitmap->GetHeight() /2;
	//画图范围能放下该图，才画
	if( r.Width() >= pDibBitmap->GetWidth()
		&& r.Height() >= pDibBitmap->GetHeight() )
	{
		pDibBitmap->SetCDibRect(CRect(pt.x, pt.y, pt.x + pDibBitmap->GetWidth() , pt.y + pDibBitmap->GetHeight()));
		pDibBitmap->Draw(memDC, true);	
	}
}

void CMyListCtrl::OnCoolSBCustomdraw(NMHDR* pNMHDR, LRESULT* pResult )
{
	NMCSBCUSTOMDRAW* pNMCD = reinterpret_cast<NMCSBCUSTOMDRAW*>(pNMHDR);	
	if( m_pBitmapScrollHorz != NULL && m_pBitmapScrollVert != NULL )
	{
		*pResult = HandleCustomDraw(0, (NMCSBCUSTOMDRAW*)pNMHDR, m_pBitmapScrollVert,m_pBitmapScrollHorz );
	}
}

void CMyListCtrl::SearchHighLight(vector<int> &vecItems,COLORREF &colorHighLight)
{
	m_colorSearchHightLight=colorHighLight;
	m_vecSearchedItems.clear();
	copy(vecItems.begin(),vecItems.end(),back_inserter(m_vecSearchedItems));
	Invalidate(FALSE);
}

static WPARAM mkey1 = 0;
static WPARAM mkey2 = 0;
BOOL CMyListCtrl::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_CONTROL )
	{
		mkey1 = VK_CONTROL;
	}
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == 'A' )
	{
		if( mkey1 != 0 )
		{
			if( mkey2 == 0 )
			{
				mkey2 = 'A';
				for (int i=0;i<GetItemCount();++i)
				{
					SetItemState( i,LVIS_SELECTED,LVIS_SELECTED);		
				}
				return TRUE;
			}
		}
	}
	if( pMsg->message == WM_KEYUP )
	{
		mkey2 = 0;
		if(pMsg->wParam == VK_CONTROL)
			mkey1 = 0;
	}
	if( pMsg->hwnd == m_hWnd && mkey1 == 0
		&& pMsg->message == WM_KEYDOWN 
		&& (((pMsg->wParam > 0X40)&&(pMsg->wParam < 0X5B))||((pMsg->wParam > 0X60)&&(pMsg->wParam < 0X7B))) )
	{
		//Ctrol_a_z_KeyDown( short(pMsg->wParam) );
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

void  CMyListCtrl::SetKeySubItem(int nIndex)
{
	m_nKeySubItem = nIndex;
}
