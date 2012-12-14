#include "stdafx.h"
#include <algorithm>
#include "MyList.h"
#include "ScrollControl.h"
#include "headctrol.h"
#include "DrawList.h"
#include "listdata.h"
#include "SortControl.h"
#include "SelectControl.h"
#include "../skinscroll/coolsb_detours.h"
#include "../../Bitmap/DibBitmap.h"

extern LRESULT HandleCustomDraw(UINT ctrlid, NMCSBCUSTOMDRAW *nm,CDibBitmap * bmpV, CDibBitmap * bmpH);

// CMyList

IMPLEMENT_DYNAMIC(CMyList, CWnd)
CMyList::CMyList()
:m_headHiht(18),
m_DisplayX(20),
m_DisplayY(2800),
m_itemHeight(18),
m_itemBigHeight( m_itemHeight*2),
m_buttonDown(false),
mbMouseOnList(false),
m_bHasHscrollBar(false),
m_bPreHasHscrollBar(false),
m_nhoverIndex(-1),
m_nPrehoverIndex(-1)
{
	m_pScrollInfo = new CScrollControl(20);
	m_pScrollInfo->AttachWnd( this );

	m_pHead = new CHeadCtrol;
	m_pDraw = new CDrawList;
	m_pData = new CListData;
	m_pSort = new CSortControl;
	m_pSelect = new CSelectControl;
	m_bInit = false;
}

CMyList::~CMyList()
{
	if(m_pScrollInfo) delete m_pScrollInfo;
	if(m_pHead)		  delete m_pHead;
	m_textFont.DeleteObject();
	if (m_pData )     delete m_pData;
	if( m_pDraw )     delete m_pDraw;
	if( m_pSort )     delete m_pSort;
	if( m_pSelect ) delete m_pSelect;
}

BEGIN_MESSAGE_MAP(CMyList, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE( SORT_MSG, OnSort )
	ON_MESSAGE( CHANGE_SCROOL_MSG, OnChangeScrool )
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(NM_COOLSB_CUSTOMDRAW, OnCoolSBCustomdraw )
	ON_MESSAGE(WM_MOUSELEAVE,   OnMouseLeaveList)
	ON_MESSAGE(WM_MOUSEHOVER,   OnMouseHoverList)
	ON_MESSAGE(MSG_MOUSELEAVE, OnMouseLeaveButton )
	ON_MESSAGE(MSG_MOUSEHOVER, OnMouseHoverButton )
	ON_MESSAGE(MSG_MOUSELEAVE_ITEM, OnMouseLeaveItem )
	ON_MESSAGE(MSG_MOUSEHOVER_ITEM, OnMouseHoverItem )
	ON_MESSAGE(MSG_MOUSELEAVE_TEXT, OnMouseLeaveText )
	ON_MESSAGE(MSG_MOUSEHOVER_TEXT, OnMouseHoverText )
	ON_WM_MOVE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCMBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

HWND CMyList::GetHeadHwnd( )const
{
	if( NULL !=m_pHead ) return m_pHead->m_hWnd;
	return NULL;
}
BOOL CMyList::CreateList(CRect& rect, HWND parentWnd, int ID)
{
	BOOL lnreturn = false;

	CString ClassName   = ::AfxRegisterWndClass(CS_CLASSDC,::LoadCursor(NULL,IDC_ARROW),NULL,NULL);   
	lnreturn = m_pHead->CreateEx(WS_EX_LEFT,ClassName,ClassName,WS_CHILD|WS_VISIBLE,rect,CWnd::FromHandle(parentWnd),ID);
	if( !lnreturn ) return lnreturn;

	ClassName   = ::AfxRegisterWndClass(CS_CLASSDC|CS_DBLCLKS,::LoadCursor(NULL,IDC_ARROW),NULL,NULL);   
	lnreturn =  this->CreateEx(WS_EX_APPWINDOW,ClassName,ClassName,WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL,rect,CWnd::FromHandle(parentWnd),ID+1);

	return lnreturn;

}
CListData* CMyList::GetListData()const
{
	return m_pData;
}
CSelectControl* CMyList::GetSelectControl()const
{
	return m_pSelect;
}
CDrawList* CMyList::GetDrawList()const
{
	return m_pDraw;
}
CHeadCtrol* CMyList::GetHeadWnd()const
{
	return m_pHead;
}
int CMyList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_pHead->AttachListWnd(this->m_hWnd);
	m_pHead->SetHight( m_headHiht );
	m_pDraw->AttachList( this->m_hWnd);
	m_pSort->AttachList( this->m_hWnd );
	m_pData->AttachList( this->m_hWnd );
	m_pSelect->AttachList( this->m_hWnd );

	LoadSkin();
	AfxGetUIManager()->UIGetSkinMgr()->AddSkinWnd( this );
	return 0;
}

void CMyList::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if(m_pHead == NULL)
		return;

	CRect rect;
	GetWindowRect(&rect);
	//设置表头宽
	if( m_pHead->m_hWnd )
		m_pHead->SetWidth( rect );

	int add = 0;
	if ( rect.Width() > 0 && rect.Height() > 0 )
	{
		if( m_pHead->m_hWnd)
		{
			m_DisplayX = m_pHead->GetHeadRight();
			m_DisplayY = GetBotton();
			m_pScrollInfo->SetDisplaySize(  m_DisplayX, m_DisplayY);
		}
	}
	int cySB = ::GetSystemMetrics(SM_CYHSCROLL);
	m_pScrollInfo->OnSize(nType, cx, cy);
	Invalidate();
}

void CMyList::OnPaint()
{
	CPaintDC dc(this);


	int w,h;
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
		CoolSB_SetSize(m_hWnd, SB_BOTH,  h,w);
		::CoolSB_SetMinThumbSize(m_hWnd, SB_BOTH, 8);
		m_bInit = true;
	}

	CRect lrect;
	GetClientRect( &lrect );
	CMemDC lDC(&dc, &lrect);
	int x=lrect.right;
	int y=lrect.bottom+m_itemHeight;
	m_pDraw->DrawList( &lDC, x, y );
	m_pDraw->DrawEndBK( &lDC, x, y, GetBotton(), m_itemHeight);
}

void CMyList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	m_pScrollInfo->OnVScroll( nSBCode, nPos, pScrollBar );
	//	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CMyList::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	m_pScrollInfo->OnHScroll( nSBCode, nPos, pScrollBar );
	///	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}
BOOL CMyList::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	BOOL wasScrolled = m_pScrollInfo->OnMouseWheel(nFlags, zDelta, pt);
	return wasScrolled;
}

BOOL CMyList::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

int CMyList::InsertColumn( CString strName, int width, int lessWidth, bool sort )
{
	if( lessWidth>width ) return -1;
	if( lessWidth <8 ) lessWidth =8;

	int size = m_pHead->InsertItem( strName, width, lessWidth,sort );

	m_pData->SetRow( size );
	return size-1;
}
void CMyList::DeleteAllItem()
{
	m_pData->DeleteAllLine();
	m_pSelect->m_nPreSelected = -1;
	m_pSelect->m_SelectedVector.clear();
	m_pScrollInfo->ScrollToOrigin(true,true);
}
void CMyList::DeleteAllColumn()
{
	m_pHead->DeleteAllItem();
	m_pData->SetRow( 0 );
	m_pData->DeleteAllLine();
}
LRESULT CMyList::OnSort( WPARAM wParam, LPARAM lParam )
{
	m_pSort->SortItem( (int)wParam, (int)lParam );
	this->Invalidate();
	return 1;
}
LRESULT CMyList::OnChangeScrool( WPARAM wParam, LPARAM lParam )
{	
	m_DisplayX = m_pHead->GetHeadRight();
	m_pScrollInfo->SetDisplaySize(  m_DisplayX, m_DisplayY, SB_HORZ);
	this->Invalidate();
	return 1;
}
void CMyList::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_buttonDown)
	{
		m_pSelect->CtrolMouseMove(nFlags,point);
		CWnd::OnMouseMove(nFlags, point);
		return;
	}
	int line = -1;
	int row  = -1;
	PointInItemRect(point, line, row );
	bool bReturn = false;

	int hang = m_pData->GetSize();
	int lie = m_pHead->GetCount();
	
	if( line < 0 || line >= hang
		|| row < 0 || row >= lie)  
	{
		m_nhoverIndex = -1;
		if( m_nPrehoverIndex != -1 )
		{
			m_pDraw->DrawARow(m_nPrehoverIndex);
			m_nPrehoverIndex =-1;
		}
	}
	else
	{
		if( m_nPrehoverIndex != line )
		{
			m_nhoverIndex = -1;
			m_pDraw->DrawARow(m_nPrehoverIndex);
		
			m_nhoverIndex = line;
			m_pDraw->DrawARow(line);
		}
	}

	m_nPrehoverIndex = m_nhoverIndex;
	m_pData->GetbmpRectData()->CtrolMouseMove(  nFlags,point);
	m_pData->GetTextRectData()->CtrolMouseMove(nFlags,point);

	if(!mbMouseOnList)
	{
		TRACKMOUSEEVENT   tme; 
		tme.cbSize   =   sizeof(tme); 
		tme.hwndTrack   =   m_hWnd; 
		tme.dwFlags   =   TME_LEAVE   |   TME_HOVER; 
		tme.dwHoverTime   =   1; 
		mbMouseOnList   =   _TrackMouseEvent(&tme); 
	} 

	CWnd::OnMouseMove(nFlags, point);
}
void CMyList::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_pSelect->CtrolRButtonDown(nFlags, point);
	__super::OnRButtonDown(nFlags, point);
}

void CMyList::OnLButtonDown(UINT nFlags, CPoint point)
{
	this->SetFocus();
	SetCapture();
	m_buttonDown =true;
	//鼠标点击选择
	m_pSelect->CtrolLButtonDown(nFlags,point);
	CWnd::OnLButtonDown(nFlags, point);
}
void CMyList::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_pSelect->CtrolLButtonUp(nFlags, point);
	CWnd::OnLButtonUp(nFlags, point);

	// 光标
	CImageList::DragLeave(NULL);
	CImageList::EndDrag();
	ReleaseCapture();
}
void CMyList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	this->SetFocus();

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CMyList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMyList::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}
int CMyList::PointToRow(CPoint& point)
{
	int count = m_pHead->GetCount();
	for( int i=0; i<count; i++)
	{
		if(( point.x>(int)GetItemLeft(  i  ) )&& ( point.x<(int)GetItemRight( i ) ) )
			return i;
	}
	return -1;
}
int CMyList::PointToVirtualIndex(CPoint& point)
{
	if( point.y == 0 ) return 0;
	return ((point.y+m_pScrollInfo->GetScrollPos()->cy) /m_itemHeight);
}
void CMyList::PointInItem(CPoint& point,int& line, int& row)
{
	if( !m_pData->GetRealIndex( PointToVirtualIndex(point), line ) )
	{
		line = -1;
		row =  -1;
	}
	row = PointToRow( point );
}
const CSize* CMyList::GetScrollPos2()
{
	return m_pScrollInfo->GetScrollPos();
}
int CMyList::GetBotton()const
{
	int index = m_pData->GetSize()-1;
	if( index < 0 ) return 0;

	if( m_pData->GetRowInfo(index) == NULL) return 0;
	int botton =  ( index- m_pData->GetRowInfo(index)->frontBigCout)* m_itemHeight
		+(m_pData->GetRowInfo(index)->frontBigCout)* m_itemBigHeight;
	if( m_pData->GetRowInfo(index)->strLineText.GetLength() == 0 ) 
	{
		botton += m_itemHeight;
	}
	else
	{
		botton += m_itemBigHeight;
	}
	return botton;
}
int CMyList::GetRight()const
{
	return m_pHead->GetHeadRight();
}
void CMyList::InvalidateList()
{
	m_pHead->Invalidate();
	this->Invalidate();
}
int CMyList::InsertItem()
{
	return m_pData->InsertLine();
}
int CMyList::InsertItem(UINT index)
{
	return m_pData->InsertLine( index );
}
bool CMyList::SetItemText( UINT index,UINT nSubItem, CString lpszText )
{
	return m_pData->SetItemData(index, nSubItem, lpszText);
}
bool CMyList::SetItemText( UINT index,UINT nSubItem, CString lpszText,CString subRowText )
{
	return m_pData->SetItemData(index, nSubItem, lpszText,subRowText,nSubItem,0);
}
bool CMyList::SetItemData( UINT index,UINT nSubItem, CString lpszText,CString subRowText,int nType,bool mouseTrack)
{
	return m_pData->SetItemData(index, nSubItem, lpszText,subRowText,nSubItem,nType,mouseTrack);
}
bool CMyList::SetItemData( UINT index,UINT nSubItem, CString lpszText,int nType, bool mouseTrack )
{
	return m_pData->SetItemData(index, nSubItem, lpszText,nSubItem,nType,mouseTrack);
}
bool CMyList::GetItemData(UINT nLine, UINT nRow, CString& str )const
{
	return m_pData->GetItemData( nLine, nRow, str );
}
bool CMyList::GetItemData(UINT nLine, UINT nRow, CString& str,CString& strEx)const
{
	return m_pData->GetItemData( nLine, nRow, str,strEx );
}
const char*  CMyList::GetItemStrPtr(UINT& nLine, UINT& nRow)
{
	return m_pData->GetItemStrPtr( nLine, nRow );
}
bool CMyList::UpdataIndex(UINT nIndex)
{
	bool breturn = false;
	m_pData->UpdateBigCount();
	breturn =  m_pData->UpdateIndex( nIndex );
	m_DisplayY = GetBotton();
	m_pScrollInfo->SetDisplaySize(  m_DisplayX, m_DisplayY);
	this->Invalidate();
	return breturn;
}
void CMyList::SetItemHight(UINT hight)
{
	m_itemHeight = hight;
	m_pScrollInfo->SetAItemMinHight( m_itemHeight );
}
UINT CMyList::GetItemRight(int pos)const
{
	if( pos == -1 ) return (UINT)m_pHead->GetHeadRight();
	return m_pHead->GetItemRight(  pos );
}
UINT CMyList::GetLastRowRight()const
{
	return (UINT)m_pHead->GetHeadRight();
}
UINT CMyList::GetItemLeft(int pos)const
{
	if( pos == -1 ) return (UINT)m_pHead->GetHeadRight();
	return m_pHead->GetItemLeft(  pos );
}
void CMyList::GetSelected( vector<int>& selected, int& count )
{
	count = 0;
	for( int i=0; i<m_pData->GetSize(); i++ )
	{
		if( m_pData->IsSelected(i) )
		{
			selected.push_back( i );
			count ++;
		}
	}
}
vector<UINT>* CMyList::GetCtrlKeySelected( int& count )
{
	count = (int)m_pSelect->m_SelectedVector.size();
	return &(m_pSelect->m_SelectedVector);
}
POSITION CMyList::GetFirstSelectedItemPosition()
{
	for( int i=0; i<m_pData->GetSize(); i++ )
	{
		if( m_pData->IsSelected(i) )
		{
			return reinterpret_cast<POSITION>( i+1 );
		}
	}
	return NULL;
}
int CMyList::GetNextSelectedItem(POSITION& position)
{
	int i= (int)( position );
	if( 0 == i ) return -1;
	--i;   // position 加了 1 为了和 NULL 不冲突
	for( ;i<m_pData->GetSize(); i++ )
	{
		if( m_pData->IsSelected(i) )
		{
			position =  reinterpret_cast<POSITION>(i+1+1);
			return i;
		}
	}
	position =  reinterpret_cast<POSITION>(0);
	return -1;
}
int CMyList::GetPreSelected()
{
	return m_pSelect->m_nPreSelected;
}
bool CMyList::IsSelected(UINT nIndex)
{
	if(nIndex>=GetLineSize()) return false;
	if( GetListData()->GetRowInfo(nIndex)->Selected )
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;
}
void CMyList::SetSelected(int sel)
{
	m_pSelect->m_nPreSelected = sel;
}
void CMyList::SetColumnBitmapShowMode(UINT nColumn,int nShowPos, int nShowType)
{
	m_pData->SetRowBmpMode(nColumn, nShowPos, nShowType );
}
void CMyList::SetColumnTextShowMode(UINT nColumn,int nShowPos, int nShowType)
{
	m_pData->SetRowTextMode(nColumn, nShowPos, nShowType );
}
LRESULT CMyList::OnMouseLeaveList(WPARAM   wParam,   LPARAM   lParam) 
{
	mbMouseOnList = false;
	m_pData->CtrolMouseLive();
	m_nhoverIndex = -1;
	m_pDraw->DrawARow(m_nPrehoverIndex);
	m_nPrehoverIndex =-1;
	OnMouseLeaveList();
	return 1;
}
LRESULT CMyList::OnMouseHoverList(WPARAM   wParam,   LPARAM   lParam) 
{
	return 1;
}
LRESULT CMyList::OnMouseLeaveButton(WPARAM wParam, LPARAM lParam)
{
	int index = (int)wParam;
	int row  = (int)lParam;
	if( m_pData->GetRowInfo(index) == NULL) return 1;
	if( !OnMouseLeaveButton(index,row) ) return 1;

	if( (SINGLE_BMP == GetListData()->GetRowBmpShowType( row )) || (SINGLE_MULT_BMP == GetListData()->GetRowBmpShowType( row )) ) return 1;

	if( GetListData()->GetRowInfo(index)->Selected )
	{
		m_pDraw->DrawItemBmp(index, row, BMP_SELECT ); 
	}
	else
	{
		m_pDraw->DrawItemBmp(index, row, BMP_NORMAL );
	}
	return 1;
}
LRESULT CMyList::OnMouseHoverButton(WPARAM wParam, LPARAM lParam)
{
	int index = (int)wParam;
	int row  = (int)lParam;

	if( m_pData->GetRowInfo(index) == NULL) return 1;
	if( !OnMouseHoverButton(index,row) ) return 1;

	if( (SINGLE_BMP == GetListData()->GetRowBmpShowType( row )) || (SINGLE_MULT_BMP == GetListData()->GetRowBmpShowType( row )) ) return 1;
	
	m_pDraw->DrawItemBmp(index, row, BMP_OVER );
	return 1;
}

LRESULT CMyList::OnMouseLeaveItem(WPARAM wParam, LPARAM lParam)
{
	return 1;
}
LRESULT CMyList::OnMouseHoverItem(WPARAM wParam, LPARAM lParam)
{
	return 1;
}
LRESULT CMyList::OnMouseLeaveText(WPARAM wParam, LPARAM lParam)
{
	int index = (int)wParam;
	int row  = (int)lParam;
	if(row < 0)
		return 0;
	m_pDraw->DrawAItemText(index,row,NORMAL_TEXT);
	return 1;
}
LRESULT CMyList::OnMouseHoverText(WPARAM wParam, LPARAM lParam)
{
	int index = (int)wParam;
	int row  = (int)lParam;
	m_pDraw->DrawAItemText(index,row,LINK_TEXT);
	return 1;
}
void CMyList::SetColumnBitmap( int nRow, int nType, CDibBitmap* bmp)
{
	m_pData->GetbmpRectData()->SetBitmap( nRow, nType, bmp );
}
bool CMyList::PointInItemRect(CPoint& point)
{
	if( point.x< GetRight() && point.y< GetBotton() ) return true;
	return false;
}
bool CMyList::PointInItemRect(CPoint& point,int& nLine, int& nRow)
{
	PointInItem(point, nLine, nRow);
	if(( nLine >=0 ) && (nRow>= 0) ) return true;
	return false;
}
bool CMyList::PointInItemBmpRect( CPoint& point, int& nLine, int& nRow, int& bmpIndex)
{
	PointInItem( point, nLine, nRow );
	if(( nLine >=0 ) && (nRow>= 0) )
	{   
		// 此行没图片return false;
		if( GetListData()->GetRowInfo(nLine) == NULL) return false;
		if( GetListData()->GetRowInfo(nLine)->rowData[nRow].bmpItem.second == -1 )
			return false; 
		CRect bmpRect;
		m_pData->GetbmpRectData()->GetBitmapRect( bmpRect,nLine,nRow );

		CPoint virtualPoint = point;	
		virtualPoint.y += GetScrollPos2()->cy;

		if( PtInRect( &bmpRect , virtualPoint ) ) return true;
	}
	return false;
}
bool CMyList::PointInItemTextRect( CPoint& point, int& nLine, int& nRow, int& bmpIndex)
{
	PointInItem( point, nLine, nRow );
	if(( nLine >=0 ) && (nRow>= 0) )
	{   
		UINT line = nLine;
		UINT lRow  = nRow;
		if( GetListData()->GetItemStrPtr(line,lRow) == NULL) return false;
		CRect textRect;
		m_pData->GetTextRectData()->GetTextRect( textRect,nLine,nRow );

		//CPoint virtualPoint = point;	
		//virtualPoint.y += GetScrollPos2()->cy;

		//if( PtInRect( &textRect , virtualPoint ) ) return true;
		//下面的代码是朝华写的，上面是周鹏写的
		//朝华写这些代码是为了解决这个bug
		//D001507	低	鼠标移动到歌手、专辑列的空白区域时变成小手状，且歌手名、专辑名出现下划线，但单击鼠标左键未发生搜索操作，下划线消失。	陈创	
		if( textRect.left < point.x && point.x < textRect.right )
		{
			return true;
		}
	}
	return false;
}
void CMyList::SetTextColor(COLORREF colText,COLORREF colSelText)
{
	m_pDraw->SetTextCorlor(colText,colSelText);
}
void CMyList::SetBKcolor( COLORREF bk1,COLORREF bk2,COLORREF select)
{
	m_pDraw->SetBkCorlor( bk1, bk2, select );
}
void CMyList::GetColumnText(int index, CString& str)
{
	m_pHead->GetItemText( index, str );
}
void CMyList::SetColumnText(int index, CString str)
{
	m_pHead->ModifyItemText( index, str );
}
void CMyList::SetColumnData(int index, CString str,int width, int minWidth )
{
	m_pHead->ModifyItemData( index, str, width,minWidth );
}

void CMyList::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	m_pHead->SetTop( y - m_headHiht );
}
void CMyList::OnCoolSBCustomdraw(NMHDR* pNMHDR, LRESULT* pResult )
{
	NMCSBCUSTOMDRAW* pNMCD = reinterpret_cast<NMCSBCUSTOMDRAW*>(pNMHDR);	
	if( m_pBitmapScrollHorz != NULL && m_pBitmapScrollVert != NULL )
	{
		*pResult = HandleCustomDraw(0, (NMCSBCUSTOMDRAW*)pNMHDR, m_pBitmapScrollVert,m_pBitmapScrollHorz );
	}
}
void CMyList::SetScrollBitmap(CDibBitmap* pBitmapVert,CDibBitmap* pBitmapHorz)
{
	m_pBitmapScrollHorz = pBitmapHorz;
	m_pBitmapScrollVert = pBitmapVert;
}
void   CMyList::SetHeadBitmap( CDibBitmap* pBk,CDibBitmap* pMidLine,CDibBitmap* pAsc,CDibBitmap* pDesc)
{
	m_pHead->SetHeadBitmap( pBk, pMidLine, pAsc, pDesc );
}

void CMyList::SetHeadTextColor( COLORREF col )
{
	m_pHead->SetHeadTextColor( col );
}
void CMyList::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	SetFocus();
	__super::OnNcLButtonDown(nHitTest, point);
}

void CMyList::OnNcMButtonDblClk(UINT nHitTest, CPoint point)
{
	SetFocus();
	__super::OnNcMButtonDblClk(nHitTest, point);
}
void CMyList::GetBmpRect( int nLine, int nRow,int nBmpPosIndex, CRect& rect )
{
	GetListData()->GetbmpRectData()->GetBitmapRect(rect,nLine, nRow );
	rect.top	 -= GetScrollPos2()->cy;	
	rect.bottom	 -= GetScrollPos2()->cy;
}

void CMyList::SetSortIndex( vector<int>* pIndexData)
{
	m_pSort->SetSortIndex( pIndexData );
}

void CMyList::ChangeItemShowBmpIndex( int line, int row, int nbmp, int nbmpIndex ) //nbmp 不同位置的图片, 音乐盒只有一个位置默认0
{
	int count = GetListData()->GetbmpRectData()->GetRowBmpCount( row );

	if( nbmpIndex > GetListData()->GetbmpRectData()->GetRowBmpCount( row ) ) return;
	GetListData()->GetRowInfo(line)->rowData[row].bmpItem.second = nbmpIndex;
	m_pDraw->DrawItemBmp(line, row, nbmpIndex );
}
void CMyList::ChangeItmeBmpMouseTrack( int line, int row, bool bTrack )
{ 
	if( line>=GetLineSize() ) return ;
	if( row>=m_pHead->GetCount() )return ;

	GetListData()->GetRowInfo(line)->rowData[row].bmpMouseTrack.second = bTrack;
	bool test = true;
	 test = GetListData()->GetRowInfo(line)->rowData[row].bmpMouseTrack.second;

	test = true; 
}
void CMyList::OnDestroy()
{
	__super::OnDestroy();

	UninitializeCoolSB( m_hWnd );
}
UINT CMyList::GetLineSize()
{
	return m_pData->GetSize();
}

int  CMyList::GetItemBmpType(int nLine, int nRow)
{
	if(nLine>=GetLineSize()) return -1;
	if( nRow >=m_pHead->GetCount() ) return -1;
	return m_pData->GetRowInfo(nLine)->rowData[nRow].bmpItem.second ;
}
void CMyList::DrawItemBmp(int nLine, int nRow, int bmpType)
{
	if(nLine>=GetLineSize()) return ;
	if( nRow >=m_pHead->GetCount() ) return;
	m_pDraw->DrawItemBmp(nLine, nRow, bmpType );
}

static WPARAM mkey1 = 0;
static WPARAM mkey2 = 0;
BOOL CMyList::PreTranslateMessage(MSG* pMsg)
{
	if( (pMsg->message   ==   WM_KEYDOWN) &&  ( pMsg->wParam == VK_LEFT )  )
	{
		return true;
	}
	if( (pMsg->message   ==   WM_KEYDOWN) &&  ( pMsg->wParam == VK_RIGHT )  )
	{
		return true;
	}
	if( (pMsg->message   ==   WM_KEYDOWN) && ( pMsg->wParam == VK_CONTROL )  )
	{
		mkey1 = VK_CONTROL;
	}
	if( (pMsg->message   ==   WM_KEYDOWN) && ( pMsg->wParam == 'A' )  )
	{
		if( mkey1 != 0 )
		{
			if( mkey2 == 0 )
			{
				mkey2 = 'A';
				// 选择所有
				m_pSelect->CtrolAKeyDown();
			}
		}
	}
	if( pMsg->message   ==   WM_KEYUP  )
	{
		mkey2 = 0;
		if(pMsg->wParam == VK_CONTROL)
			mkey1 = 0;
	}
	if( (pMsg->message   ==   WM_KEYDOWN) && (( pMsg->wParam == VK_DOWN )|| ( pMsg->wParam == VK_UP ))  )
	{
		// 滚动频率限制优化
		static DWORD t1 = 0;
		static DWORD t2 = 0;

		t2 = GetTickCount();

		if( ((t2-t1)<50) ) 
		{
			return 1;
		}
		t1 = t2;
		if( (pMsg->message   ==   WM_KEYDOWN) && ( pMsg->wParam == VK_DOWN ) )
		{
			if( GetLineSize() <= 0 )  	return __super::PreTranslateMessage(pMsg);
			m_pSelect->CtrolDownKeyDown();

			int  showTop =  m_pSelect->m_nPreSelected* m_itemHeight+( GetListData()->GetRowInfo(m_pSelect->m_nPreSelected)->frontBigCout)* m_itemHeight;  
			if( m_pSelect->m_nPreSelected ==  0 )
			{
				m_pScrollInfo->ScrollToOrigin( false, true);
			}
			else
			{
				CRect rect;
				this->GetClientRect( &rect );
				if( showTop-GetScrollPos2()->cy >= rect.bottom  )
				{
					this->OnVScroll( SB_POS,showTop+m_itemHeight, this->GetScrollBarCtrl(SB_VERT) );
				}
			}
			return 1;
		}

		//
		if( (pMsg->message   ==   WM_KEYDOWN) && ( pMsg->wParam == VK_UP ) )
		{
			if( GetLineSize() <= 0 )  	return __super::PreTranslateMessage(pMsg);
			m_pSelect->CtrolUpKeyDown();

			int  showTop = m_pSelect->m_nPreSelected* m_itemHeight+( GetListData()->GetRowInfo(m_pSelect->m_nPreSelected)->frontBigCout)* m_itemHeight;	
			if( m_pSelect->m_nPreSelected == GetLineSize()-1 )
			{
				m_pScrollInfo->ScrollToEnd( false, true);
			}
			else
			{
				CRect rect;
				this->GetClientRect( &rect );
				if(  showTop-GetScrollPos2()->cy < 0  )
				{
					this->OnVScroll( SB_POS,showTop, this->GetScrollBarCtrl(SB_VERT) );
				}
				return 1;
			}
		}
	}

	if( (pMsg->message   ==   WM_KEYDOWN) && ( pMsg->wParam == VK_HOME ) )
	{
		if(m_pScrollInfo)
			m_pScrollInfo->ScrollToOrigin( false, true);
	}
	if( (pMsg->message   ==   WM_KEYDOWN) && ( pMsg->wParam == VK_END ) )
	{
		if(m_pScrollInfo)
			m_pScrollInfo->ScrollToEnd( false, true);
	}
	if( (pMsg->message   ==   WM_KEYDOWN) && ( pMsg->wParam == VK_PRIOR ) )
	{
		if(m_pScrollInfo)
			this->OnVScroll( SB_PAGEUP,0, this->GetScrollBarCtrl(SB_VERT) );			
	}
	if( (pMsg->message   ==   WM_KEYDOWN) && ( pMsg->wParam == VK_NEXT ) )
	{
		if(m_pScrollInfo)
			this->OnVScroll( SB_PAGEDOWN,0, this->GetScrollBarCtrl(SB_VERT) );			
	}
	if( (pMsg->message   ==   WM_KEYDOWN) && (((pMsg->wParam > 0X40)&&(pMsg->wParam < 0X5B))||((pMsg->wParam > 0X60)&&(pMsg->wParam < 0X7B))) )
	{
		if( mkey1 != VK_CONTROL )
		{
			int nReturn = m_pSelect->Ctrol_a_z_KeyDown( short(pMsg->wParam) );
			if( nReturn == 1)
			{
				if( m_pSelect->m_nPreSelected != -1 )
					ScrollToALine(m_pSelect->m_nPreSelected,true );
			}
			else if( nReturn == 0 )
			{
				if( m_pSelect->m_nPreSelected != -1 )
					ScrollToALine(m_pSelect->m_nPreSelected,false );
			}
		}
	}
	return __super::PreTranslateMessage(pMsg);
}
void CMyList::ScrollToALine(UINT nLine, bool bDown)
{
	if( bDown )
	{
		if( nLine ==  0 )
		{
			m_pScrollInfo->ScrollToOrigin( false, true);
			return;
		}

		int  showTop =  nLine * m_itemHeight+( GetListData()->GetRowInfo(nLine)->frontBigCout)* m_itemHeight;  
		CRect rect;
		this->GetClientRect( &rect );
		if( showTop-GetScrollPos2()->cy >= rect.bottom  )
		{
			this->OnVScroll( SB_POS,showTop+m_itemHeight, this->GetScrollBarCtrl(SB_VERT) );
		}
	}
	else
	{
		if( nLine == GetLineSize()-1 )
		{
			m_pScrollInfo->ScrollToEnd( false, true);
			return;
		}

		int  showTop = nLine* m_itemHeight+( GetListData()->GetRowInfo(nLine)->frontBigCout)* m_itemHeight;	
		CRect rect;
		this->GetClientRect( &rect );
		if(  showTop-GetScrollPos2()->cy < 0  )
		{
			this->OnVScroll( SB_POS,showTop, this->GetScrollBarCtrl(SB_VERT) );
		}
	}
}
