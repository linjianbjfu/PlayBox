//***************************************************************************
// Copyright 1999                                  
//***************************************************************************
// @Author: Christopher Brown
// @E-Mail: AllKnowing@EarthLink.net
// @WEB:	home.earthlink.net/~allknowing
// @ICQ:	875098
//
// @Cedits:
//
//	  Giving credit where credit is due...  Thanks for the help!!!
//    -----------------------------------------------------------
//		  Michael Dunn   - mdunn@inreach.com
//
//		  Anyone else?
//		  Sorry if I forgot to include your name... 
//    -----------------------------------------------------------
//
// @Blah, blah, blah...
//
// THIS CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
// You may use and distribute this code provided that you do not
// remove this title header and that you do not charge money for it.  
//
//***************************************************************************
#include "stdafx.h"
#include "WndSlider.h"
#include "COMMCTRL.h"
#include "../../Bitmap/DibBitmap.h"
#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "Cursor.h"
#include "Mouse.h"

const int MOUSE_TRACK_EVENT = 0x1111;

//***************************************************************************
//
//***************************************************************************
IMPLEMENT_DYNCREATE(CWndSliderView, CWnd)

//***************************************************************************
//
//***************************************************************************
BEGIN_MESSAGE_MAP(CWndSliderView, CWnd)
	//{{AFX_MSG_MAP(CWndSliderView)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_SYSCOLORCHANGE()
	ON_MESSAGE(WM_SHOW_CONTENTWND,OnShowContentWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//***************************************************************************
// CWndSliderView
//***************************************************************************
CWndSliderView::CWndSliderView() :
m_nButtonHeight(SOB_BAR_DEFAULT_BTN_HEIGHT),
m_nViewCount(0),
m_nActiveView(0),
m_nMouseOnButton(-1),
m_hHandCursor(0),
m_nPriviousButton(-1),
m_bLButtonDown(FALSE),
m_bTrackLeave(FALSE),
m_bTrackingMouse(FALSE),
m_dwViewStyle( SOB_VIEW_HORZ | SOB_VIEW_ANIMATE )
{
	m_pWndParent = NULL;
}

//***************************************************************************
//
//***************************************************************************
CWndSliderView::~CWndSliderView()
{
// 	if( m_hHandCursor ) 
// 		DestroyCursor( m_hHandCursor );

	for( int x = (int)m_buttonArray.GetUpperBound(); x >= 0; x-- )
	{
		CSliderButton* pButton = m_buttonArray.GetAt( x );
		delete pButton;
	}

}

//***************************************************************************
// CTestView drawing
//***************************************************************************
void CWndSliderView::OnDraw(CDC* pDC)
{
	UNUSED_ALWAYS( pDC );
}

//***************************************************************************
// @Returns SOB_ERROR on error, or index of added view on success.
//
//***************************************************************************
int CWndSliderView::CreateView( const WND_SLIDER_DATA& wndSliderData )
{
	int nViewID = GetViewCount();

	CSliderButton* pButton = NULL;
	try
	{
		pButton = new CSliderButton();
	}
	catch( CMemoryException* memex )
	{
		memex->ReportError();
		memex->Delete();

		return SOB_ERROR;
	}

	pButton->SetParentWnd( wndSliderData.pWnd );
	pButton->SetButtonTitle( wndSliderData.strTitle );
	pButton->SetBitmap( wndSliderData.pBitmapLeftNormal, wndSliderData.pBitmapLeftOver, wndSliderData.pBitmapLeftDown,
		wndSliderData.pBitmapMidNormal,wndSliderData.pBitmapMidOver,wndSliderData.pBitmapMidDown,
		wndSliderData.pBitmapRightNormal,wndSliderData.pBitmapRightOver,wndSliderData.pBitmapRightDown,		
		wndSliderData.pBitmapIcon,wndSliderData.pBitmapIconRight);
	pButton->SetButtonFont( wndSliderData.pFont,wndSliderData.colorFont);
	pButton->m_bNew = wndSliderData.bNew;
	return (int)m_buttonArray.Add( pButton );
}

void CWndSliderView::LoadSkin(const WND_SLIDER_DATA& wndSliderData,const string& str )
{
	for(int i = 0; i<m_buttonArray.GetSize(); i++)
	{
		CSliderButton* pButton = m_buttonArray.GetAt(i);
		if( pButton->m_strButtonTitle.Find(CString(str.c_str())) != -1)
		{
			pButton->SetBitmap( wndSliderData.pBitmapLeftNormal, wndSliderData.pBitmapLeftOver, wndSliderData.pBitmapLeftDown,
				wndSliderData.pBitmapMidNormal,wndSliderData.pBitmapMidOver,wndSliderData.pBitmapMidDown,
				wndSliderData.pBitmapRightNormal,wndSliderData.pBitmapRightOver,wndSliderData.pBitmapRightDown,		
				wndSliderData.pBitmapIcon,wndSliderData.pBitmapIconRight);
			pButton->SetButtonFont( wndSliderData.pFont,wndSliderData.colorFont);
			InvalidateButton(i);
			break;
		}
	}
}
//***************************************************************************
//
//***************************************************************************
int CWndSliderView::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

//	LoadHandCursor( m_hHandCursor );
	
	return 0;
}

//***************************************************************************
//
//***************************************************************************
CWnd* CWndSliderView::GetView( const int nView ) const 
{
	ASSERT( nView <= GetViewCount() );

	return m_buttonArray.GetAt( nView )->GetParentWnd();
}

//***************************************************************************
//
//***************************************************************************
int CWndSliderView::GetViewPos( const CWnd* pWnd ) 
{

	for(int i = 0 ; i<m_buttonArray.GetSize(); i++)
		if(m_buttonArray.GetAt( i )->GetParentWnd()->m_hWnd == pWnd->m_hWnd)
			return i;
	return -1;
}
void CWndSliderView::SetButtonTitle( int nView, CString title )
{
	ASSERT( nView <= GetViewCount() );

	CSliderButton* pBtn = m_buttonArray.GetAt( nView );
	pBtn->SetButtonTitle(title);
	this->Invalidate(true);
}

void CWndSliderView::GetButtonTitle( int nView,CString& title )
{
	ASSERT( nView <= GetViewCount() );

	CSliderButton* pBtn = m_buttonArray.GetAt( nView );
	title = pBtn->GetButtonTitle();
}
//***************************************************************************
//
//***************************************************************************
int CWndSliderView::GetViewCount( void ) const 
{
	return (int)m_buttonArray.GetSize();
}

//***************************************************************************
//
//***************************************************************************
int CWndSliderView::GetActiveView( void ) const
{
	return m_nActiveView;
}

//***************************************************************************

//***************************************************************************
int CWndSliderView::SetActiveView(	const int nNewView, 
									const BOOL bRedraw /*= TRUE*/ )
{
	ASSERT( nNewView < GetViewCount() );

	int nOldView = m_nActiveView;

	m_nActiveView = nNewView;

	if( bRedraw )
		Invalidate( TRUE );
	

	return nOldView;
}

BOOL CWndSliderView::RemoveView(const CWnd *pWnd)
{
	int nView = GetViewPos( pWnd );
	if(m_nActiveView == nView)
		SetActiveView(0);
	RemoveView(nView);
	return true;
}
//***************************************************************************
//
//***************************************************************************
BOOL CWndSliderView::RemoveView( const int nView )
{
	BOOL bResult = FALSE;
	
	if( GetViewCount() )
	{
		CWnd* pWnd = GetView( nView );

		if( pWnd )
		{
			CSliderButton* pBtn = m_buttonArray.GetAt( nView );
			
			if( pBtn )
			{
				delete pBtn;
				m_buttonArray.RemoveAt( nView );
				//pWnd->PostMessage( WM_CLOSE );
				RecalcLayout();
				Invalidate(true);
				bResult = TRUE;
			}
		}
	}
	return bResult;
}

//***************************************************************************
//
//***************************************************************************
void CWndSliderView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd::OnMouseMove(nFlags, point);

	int  nButton		= ButtonHitTest( point );
	BOOL bIsLButtonDown = (nFlags & MK_LBUTTON); 

	if( -1 != nButton )  // The mouse is over one of the buttons...
	{
		::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(32649)));
		
		//
		// left bustton is down...
		//
		if( bIsLButtonDown )
		{
			if( nButton != m_nPriviousButton )
			{
				if( nButton == m_nMouseOnButton )
				{
					InvalidateButton( nButton );
					m_nPriviousButton = m_nMouseOnButton;
				}
				else
				{
					if( nButton != m_nMouseOnButton && -1 != m_nPriviousButton )
					{
						InvalidateButton( m_nMouseOnButton );
						m_nPriviousButton = -1;;
					}
				}

			}//END_IF
		}
		else if( nButton != m_nPriviousButton )
		{
			if( -1 != m_nPriviousButton )
				InvalidateButton( m_nPriviousButton );

			InvalidateButton( nButton );

			m_nMouseOnButton = nButton;

			if( !m_bTrackingMouse )
			{
				SetTimer( MOUSE_TRACK_EVENT, 200, NULL );
				m_bTrackingMouse = TRUE;
			}

			m_nPriviousButton = nButton;
		}
	
	}
	else // The mouse is not over a button...
	{
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));			

		if( -1 != m_nPriviousButton && -1 != m_nMouseOnButton )
		{
			if( bIsLButtonDown )
			{
				InvalidateButton( m_nMouseOnButton );
			}
			else
			{
				InvalidateButton( m_nPriviousButton );
			}

			if( m_bTrackingMouse )
			{
				KillTimer( MOUSE_TRACK_EVENT );
				m_bTrackingMouse = FALSE;
			}
		
		}//END_IF

		m_nPriviousButton = -1;
	}

	return;	
}


//***************************************************************************
//
//***************************************************************************
void CWndSliderView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDown(nFlags, point);

	if( GetCapture() != this )
		Mouse.Capture( this );

	int nButton = ButtonHitTest( point );

	if( -1 != nButton )
	{
	    InvalidateButton( nButton );
	}
	
	m_bLButtonDown = TRUE;
}

void CWndSliderView::MySetActiveView(int nNiew)
{
	if(nNiew > 0 && nNiew >= m_buttonArray.GetSize())
		nNiew = m_buttonArray.GetSize() - 1;
	int nButton = nNiew;

	if( -1 != nButton )  // The mouse is over a button. 
	{
		//
		// Clean up old button...
		//
		/*if( nButton != m_nMouseOnButton ) 
		{
			if( -1 != m_nMouseOnButton )
				InvalidateButton( m_nMouseOnButton );
		}*/

		//
		// Draw new MouseOnButton...
		//
		InvalidateButton( nButton );

		//if( nButton == m_nMouseOnButton )
		{
			if( nButton != m_nActiveView )
			{				
				//
				// Only animat if both the global animate and individual 
				// button animate flags are turned on.
				//
		
				m_nActiveView = nButton;
				RecalcLayout();
				Invalidate();
				
			}
		}

		//
		//Save the new MouseOnButton location...
		//
		m_nMouseOnButton = nButton;
	}
	else if( -1 != m_nMouseOnButton ) // The mouse is NOT over a button. 
	{ 
		//
		// Clean up old button...
		//
		InvalidateButton( m_nMouseOnButton );
		m_nMouseOnButton = -1;
	}
	m_pWndParent->SendMessage(WM_QQSHIFT,nNiew,-1);

	//Mouse.Release();
	
	//m_bLButtonDown = FALSE;

	//CWnd::OnLButtonUp(nFlags, point);
}
//***************************************************************************
//
//***************************************************************************

void CWndSliderView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int nButton = ButtonHitTest( point );

	if( -1 != nButton )  // The mouse is over a button. 
	{
		//
		// Clean up old button...
		//
		if( nButton != m_nMouseOnButton ) 
		{
			if( -1 != m_nMouseOnButton )
				InvalidateButton( m_nMouseOnButton );
		}

		//
		// Draw new MouseOnButton...
		//
		InvalidateButton( nButton );

 		if( nButton == m_nMouseOnButton )
		{
			if( nButton != m_nActiveView )
			{				
				//
				// Only animat if both the global animate and individual 
				// button animate flags are turned on.
				//				
				m_nActiveView = nButton;
				RecalcLayout();
				Invalidate();
				m_pWndParent->PostMessage(WM_QQSHIFT,m_nActiveView, 0);
				
			}
			else
				m_pWndParent->PostMessage(WM_QQSHIFT,m_nActiveView, 1);
		}

		//
		//Save the new MouseOnButton location...
		//
		m_nMouseOnButton = nButton;
	}
	else if( -1 != m_nMouseOnButton ) // The mouse is NOT over a button. 
	{ 
		//
		// Clean up old button...
		//
		InvalidateButton( m_nMouseOnButton );
		m_nMouseOnButton = -1;
	}

	Mouse.Release();
	
	m_bLButtonDown = FALSE;

	CWnd::OnLButtonUp(nFlags, point);
}

//***************************************************************************
//
//***************************************************************************
BOOL CWndSliderView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	BOOL bResult = FALSE;

	CPoint pt = Mouse;
	ScreenToClient( &pt );

	int nButton = ButtonHitTest( pt );

    if( NULL != m_hHandCursor && -1 != nButton )
    {
        ::SetCursor( m_hHandCursor );

        bResult =  TRUE;
    }
	else
	{
		bResult = CWnd::OnSetCursor(pWnd, nHitTest, message)>0;
	}

    return bResult;
}

//***************************************************************************
//
//***************************************************************************
int CWndSliderView::ButtonHitTest( const CPoint& point ) 
{
	int nButton = -1;

	int nViewCount = GetViewCount();

	for( int x = 0; x < nViewCount; x++ )
	{
		CRect rectButton = GetButtonRect( x );

		rectButton.InflateRect( 1, 1);

		if( rectButton.PtInRect( point ) )
		{
			nButton = x; 
		}

	} //END_FOR

	return nButton;}


//***************************************************************************
//
//***************************************************************************
void CWndSliderView::OnTimer( UINT nIDEvent ) 
{
	if( MOUSE_TRACK_EVENT == nIDEvent )
	{
		CPoint pt = Mouse;
		ScreenToClient( &pt );

		int nButton = ButtonHitTest( pt );

		if( -1 == nButton )
		{
			PostMessage( WM_MOUSEMOVE, 
				         m_bLButtonDown?MK_LBUTTON:0, 
						 MAKELPARAM( pt.x, pt.y ) );
		}
	}
	
	CWnd::OnTimer(nIDEvent);
}

//***************************************************************************
//
//***************************************************************************
void CWndSliderView::OnRButtonDown( UINT nFlags, CPoint point ) 
{
	CWnd::OnRButtonDown( nFlags, point );
}

//***************************************************************************
//
//***************************************************************************
void CWndSliderView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CWnd::OnRButtonUp( nFlags, point );
}

//***************************************************************************
//
//***************************************************************************
void CWndSliderView::RecalcLayout( const BOOL bRepaint /*=TRUE*/ )
{
	CWnd  *pWnd;
	CRect rectParent;

	//
	// Get parent rect...
	//
	GetWindowRect( rectParent );

	//
	// Here I'm practicing a bit of voodo.  All inactive child windows are 
	// moved off the parent window rect, in effect making inactive child 
	// windows hidden.  This technique was implemented over using SW_HIDE 
	// because the animation requires inactive windows to be able to paint 
	// to a temporary bitmap.  Windows hidden with SW_HIDE are unable to 
	// paint to a temporary bitmap - to the best of my humble knowledge.  
	// If someone knows of a different / better technique, please send 
	// me a note.  
	// Thanks.
	//
	for( int x = (int)m_buttonArray.GetUpperBound(); x >= 0; x-- )
	{
		CRect rectChildWnd = GetInsideRect( x );
		pWnd = GetView(x);
		
		if( x != m_nActiveView )
		{
			if( m_dwViewStyle & SOB_VIEW_HORZ )
			{
				//
				// HORZ - Move child rect off the parent rect...
				//
				rectChildWnd.OffsetRect( rectParent.right, 0 );
			}
			else
			{
				//
				// VERT - Move child rect off the parent rect...
				//
				rectChildWnd.OffsetRect( 0, rectParent.bottom );
			}
			pWnd->ShowWindow(SW_HIDE);
		}
		else
		{	
			pWnd->ShowWindow(SW_SHOW);
			pWnd->MoveWindow( rectChildWnd, bRepaint );
		}
	}
}

//***************************************************************************
//
//***************************************************************************
void CWndSliderView::OnSize( UINT nType, int cx, int cy ) 
{
	CWnd::OnSize(nType, cx, cy);

	for( int x = (int)m_buttonArray.GetUpperBound(); x >= 0; x-- )
	{
		CRect rectButton = GetButtonRect( x );
		CSliderButton *pButton = m_buttonArray.GetAt( x );
		pButton->Size( rectButton, m_dwViewStyle );
	}

	RecalcLayout();
	
	return;	
}

//***************************************************************************
//
//***************************************************************************
CRect CWndSliderView::GetButtonRect( const int nButton ) const
{
	CRect rectClient;
	GetClientRect( rectClient );
	
	return GetButtonRect( nButton, rectClient );
}

//***************************************************************************
//
//***************************************************************************
CRect CWndSliderView::GetButtonRect( const int nButton,
								 const CRect& rectClient ) const
{
	ASSERT( nButton < GetViewCount() );  // Request greater than max view count.

	CRect rc( rectClient );
	CRect rectButton; 
	
	const int nMaxCount = GetViewCount();

	if( m_dwViewStyle & SOB_VIEW_HORZ )
	{
		if( nButton > m_nActiveView )
		{
			rectButton.SetRect( rc.left, 
								rc.bottom - (((nMaxCount - nButton)) * m_nButtonHeight), 
								rc.right,
								rc.bottom - (((nMaxCount - nButton)  -1) * m_nButtonHeight));
            //
			// Subtract one for border line.
            //
			//rectButton.bottom--;
		}
		else
		{
			rectButton.SetRect( rc.left,
								rc.top + (nButton * m_nButtonHeight),
								rc.right,
								rc.top + (((1 + nButton) * m_nButtonHeight)));
		}

	}
	else // BUTTONS_VERTICAL   
	{
		if( nButton > m_nActiveView )
		{
			rectButton.SetRect( rc.right - (((nMaxCount - nButton)) * m_nButtonHeight),
								rc.top,
								rc.right - (((nMaxCount - nButton)-1) * m_nButtonHeight),
								rc.bottom );
			//					
			// Subtract one for border line.
            //
			rectButton.right--;
		}
		else
		{
			rectButton.SetRect( rc.left + (nButton * m_nButtonHeight),
								rc.top ,
								(((1 + nButton) * m_nButtonHeight) -1),
								rc.bottom );
		}
	}
	
	return rectButton;
}

//***************************************************************************
//
//***************************************************************************
CRect CWndSliderView::GetInsideRect( int nButton ) const
{
	CRect rectInside; 
	GetClientRect( rectInside );

	const int nViewCount = GetViewCount();

	if( m_dwViewStyle & SOB_VIEW_HORZ  )
	{
		if( nViewCount > 0 )
		{
			rectInside.top    += (m_nButtonHeight * (nButton + 1)) -1;
			rectInside.bottom -= ((nViewCount - nButton) - 1) * m_nButtonHeight;
			rectInside.top++;    
		}

	}
	else // BUTTONS_VERTICAL   
	{
		if( nViewCount > 0 )
		{
			rectInside.left += (m_nButtonHeight * (nButton + 1)) -1;
			rectInside.right-= ((nViewCount - nButton) - 1) * m_nButtonHeight;

			rectInside.left++;    
		}
	}

	return rectInside;
}


//***************************************************************************
// CWndSliderView message handlers
//***************************************************************************
BOOL CWndSliderView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}


//***************************************************************************
//
//***************************************************************************
void CWndSliderView::InvalidateButton( const int n ) 
{
	int nButton = n;
	if(nButton >= GetViewCount())
		nButton = GetViewCount()-1;
//	ASSERT( nButton < GetViewCount() );

	CRect rectButton = GetButtonRect( nButton );

	InvalidateRect( rectButton );
}

//***************************************************************************
//
//***************************************************************************
void CWndSliderView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CPoint pt = Mouse;
	ScreenToClient( &pt );

	CPen pen( PS_SOLID, 1, GetSysColor(COLOR_3DDKSHADOW) );
	CPen *pOldPen = dc.SelectObject( &pen );

	int nButton	= ButtonHitTest( pt );

	for( int x = (int)m_buttonArray.GetUpperBound(); x >= 0; x-- )
	{
		CRect rectButton = GetButtonRect( x );

		CSliderButton *pButton = m_buttonArray.GetAt( x );

		BUTTON_STATE dwButtonStyle = SOB_BTN_UP;
		
		if( nButton == x )
		{
			if( m_bLButtonDown )
            {
				dwButtonStyle = SOB_BTN_HIT;
            }
			else
            {
				dwButtonStyle = SOB_BTN_OVER;
            }
		}
		else if( m_bLButtonDown && x == m_nMouseOnButton )
		{
			dwButtonStyle = SOB_BTN_OVER;
		}

		pButton->Draw( &dc, rectButton, dwButtonStyle|m_dwViewStyle, (x==m_nActiveView) );
		
		if(x == this->m_nActiveView + 1)
		{
		//	dc.MoveTo( 0,  rectButton.top );
		//	dc.LineTo( rectButton.right, rectButton.top );
		}
		if(x == this->m_nActiveView)
		{
		//	dc.MoveTo( 0,  rectButton.bottom );
		//	dc.LineTo( rectButton.right, rectButton.bottom );
		}

	}	
	return; // Do not call CWnd::OnPaint() for painting messages
}

//***************************************************************************
//
//***************************************************************************
LRESULT CWndSliderView::DefWindowProc
(
	UINT message, 
	WPARAM wParam, 
	LPARAM lParam
) 
{
	switch( message )
	{

	case WM_PRINTCLIENT :
		{
			OnPrintClient( (HDC)wParam, (UINT)lParam );

			return TRUE;
		}
		break;
	}
	
	return CWnd::DefWindowProc(message, wParam, lParam);
}

//***************************************************************************
//
//***************************************************************************
void CWndSliderView::OnPrintClient( HDC hDC, UINT uFlags) 
{
	UNUSED_ALWAYS( uFlags );

	CDC *pDC = CDC::FromHandle(hDC);;
	
	for( int x = (int)m_buttonArray.GetUpperBound(); x >= 0; x-- )
	{
		CRect rectButton = GetButtonRect( x );

		CSliderButton *pButton = m_buttonArray.GetAt( x );
		pButton->Draw( pDC, rectButton, SOB_BTN_UP|m_dwViewStyle , (x==m_nActiveView));
	}
	
}

//***************************************************************************
//
//***************************************************************************
void CWndSliderView::OnSysColorChange( void )
{
	CClientDC dc( this );
	RecalcLayout();
}

//***************************************************************************
//***************************************************************************
//
//  CSliderButton
//
//***************************************************************************
//***************************************************************************

//***************************************************************************
//***************************************************************************
CSliderButton::CSliderButton()
{
	m_dwButtonStyle = SOB_BTN_FANCYFACE | SOB_BTN_ANIMATE;

	m_cFont.Attach(GetStockObject(DEFAULT_GUI_FONT));

};

//***************************************************************************
//
//***************************************************************************
CSliderButton::~CSliderButton()
{
	if( m_cFont.m_hObject != NULL )
	{
		m_cFont.DeleteObject();		
	}
}

//***************************************************************************
//***************************************************************************
CSliderButton* CWndSliderView::GetButton( const int nButton ) const
{
	ASSERT( nButton < GetViewCount() );

	return m_buttonArray.GetAt( nButton );
}

//***************************************************************************
//
//***************************************************************************
CWnd* CSliderButton::GetParentWnd( void )
{
	return m_pWndParent;
}

//***************************************************************************
//
//***************************************************************************
void CSliderButton::SetParentWnd( CWnd* pWnd )
{
	m_pWndParent = pWnd;
}
	  

//***************************************************************************
//
//***************************************************************************
DWORD CSliderButton::GetButtonStyle( void ) const 
{
	return m_dwButtonStyle;
}

//***************************************************************************
//
//***************************************************************************
void CSliderButton::ModifyButtonStyle( const DWORD dwRemove,
									   const DWORD dwAdd )
{
	m_dwButtonStyle = (m_dwButtonStyle & ~dwRemove);
	m_dwButtonStyle |= dwAdd;
}

//***************************************************************************
//
//***************************************************************************
void CSliderButton::SetButtonTitle( LPCSTR szcTitle )
{
	m_strButtonTitle = szcTitle;
}

//***************************************************************************
//
//***************************************************************************
LPCSTR CSliderButton::GetButtonTitle( void ) const
{
	return m_strButtonTitle;
}

void CSliderButton::SetBitmap(  CDibBitmap* pLeftNormal, CDibBitmap* pLeftOver, CDibBitmap* pLeftDown,
					CDibBitmap* pMidNormal, CDibBitmap* pMidOver,CDibBitmap* pMidDown,
					CDibBitmap* pRightNormal, CDibBitmap* pRightOver, CDibBitmap* pRightDown,
					CDibBitmap* pIcon, CDibBitmap* pIconRight)
{
	m_pBitmapLeftNormal	= pLeftNormal;
	m_pBitmapLeftOver	= pLeftOver;
	m_pBitmapLeftDown	= pLeftDown;

	m_pBitmapMidNormal	= pMidNormal;
	m_pBitmapMidOver	= pMidOver;
	m_pBitmapMidDown	= pMidDown;

	m_pBitmapRightNormal= pRightNormal;
	m_pBitmapRightOver	= pRightOver;
	m_pBitmapRightDown	= pRightDown;

	m_pBitmapIcon		= pIcon;
	m_pBitmapIconRight  = pIconRight;
}


void CSliderButton::SetButtonFont( CFont* pfont,COLORREF color)
{
	if( pfont != NULL )
	{
		if( m_cFont.m_hObject != NULL )
		{
			m_cFont.DeleteObject();
		}
		LOGFONT logfont;
		pfont->GetLogFont(&logfont);
		m_cFont.CreateFontIndirect( &logfont );
	}

	m_cFontColor	= color;
}
//***************************************************************************
//
//***************************************************************************
void CSliderButton::Draw( CDC* pDC, 
						  CRect& rectScreen, 
						  const DWORD dwFlags, int bDown )
{
	int nSaveDC = pDC->SaveDC();

	BOOL bIsPressed		= (dwFlags & SOB_BTN_HIT)>0;
	BOOL bMouseOnButton	= (dwFlags & SOB_BTN_OVER)>0;
	TRACE( "bIsPressed:%d bMouseOnButton:%d\n", bIsPressed, bMouseOnButton );

	CDC		 tmpDC;
	CBitmap* pOldBmp = NULL;

	CWndSliderView* parentView = (CWndSliderView*)m_pWndParent;

	bDown = false;

	CDibBitmap* pBarLeft = NULL;
	CDibBitmap* pBarMid  = NULL;
	CDibBitmap* pBarRight= NULL;
	if(bIsPressed)
	{
		pBarLeft = m_pBitmapLeftDown;
		pBarMid  = m_pBitmapMidDown;
		pBarRight= m_pBitmapRightDown;
	}
	else if(bMouseOnButton)
	{
		pBarLeft = m_pBitmapLeftOver;
		pBarMid  = m_pBitmapMidOver;
		pBarRight= m_pBitmapRightOver;
	}
	else
	{
		pBarLeft = m_pBitmapLeftNormal;
		pBarMid  = m_pBitmapMidNormal;
		pBarRight= m_pBitmapRightNormal;
	}
	
	//绘制左侧
	CRect r = CRect(0,0, pBarLeft->GetWidth() , pBarLeft->GetHeight() );
	r.OffsetRect(rectScreen.TopLeft());
	pBarLeft->SetCDibRect(r);
	pBarLeft->Draw(pDC, true);

	//绘制bar
	r = rectScreen;
	r.DeflateRect(2,0,0,0);
	pBarMid->SetCDibRect(r);
	pBarMid->Draw(pDC, true);

	//绘制图标
	if(m_pBitmapIcon != NULL)
	{
		int deltaH = (pBarMid->GetHeight() - m_pBitmapIcon->GetHeight()) / 2;
		CRect r = CRect(4, deltaH , m_pBitmapIcon->GetWidth() + 4, m_pBitmapIcon->GetHeight() + deltaH);
		r.OffsetRect(rectScreen.TopLeft());
		m_pBitmapIcon->SetCDibRect(r);
		m_pBitmapIcon->Draw(pDC, true);
	}
	//绘制右侧图标
	if(m_pBitmapIconRight != NULL)
	{
		int deltaH = (pBarMid->GetHeight() - m_pBitmapIconRight->GetHeight()) / 2;
		CRect r(rectScreen);
		r.top    = deltaH;
		r.bottom = r.top + m_pBitmapIconRight->GetHeight();
		r.right  = rectScreen.Width() - 8;
		r.left   = r.right - m_pBitmapIconRight->GetWidth();

		r.OffsetRect(rectScreen.TopLeft());
		m_pBitmapIconRight->SetCDibRect(r);
		m_pBitmapIconRight->Draw(pDC, true);
	}

	//绘制右侧
	r = CRect(rectScreen.right - pBarRight->GetWidth(), rectScreen.top, rectScreen.right, rectScreen.top + pBarRight->GetHeight());
	pBarRight->SetCDibRect(r);
	pBarRight->Draw(pDC, true);

	CFont* pOldFont = NULL;
	if( m_cFont.m_hObject != NULL )
	{
		CFont* pOldFont = pDC->SelectObject( &m_cFont );
	}
			
	pDC->SetBkMode( TRANSPARENT );
	rectScreen.DeflateRect(25, 1,0,0);

	CDibBitmap *pBmp = ::AfxGetUIManager()->UIGetSkinMgr()->GetDibBmp("QQPanelNew");
	if( m_bNew && pBmp != NULL )
	{
		//去掉new之外的地方画title
		CRect rcTitle = rectScreen;
		rcTitle.right -= pBmp->GetWidth();
		CRect rcRealTitle = rcTitle;

		pDC->SetTextColor( m_cFontColor );
		pDC->DrawText( GetButtonTitle(), -1, &rcRealTitle, DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_CALCRECT );
		pDC->DrawText( GetButtonTitle(), -1, &rcTitle, DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS);
		//剩余的地方画new
		CRect rcRealNewRect = rectScreen;
		rcRealNewRect.left += rcRealTitle.Width()+2;
		rcRealNewRect.right = rcRealNewRect.left + pBmp->GetWidth();
		int iGap = (rcRealNewRect.Height() - pBmp->GetHeight())/2;
		rcRealNewRect.top += iGap;
		rcRealNewRect.bottom -= iGap;		

		pBmp->SetCDibRect(rcRealNewRect);
		pBmp->Draw(pDC, true);
	}else
	{
		pDC->SetTextColor( m_cFontColor );
		pDC->DrawText( GetButtonTitle(), -1, &rectScreen, DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS );
	}

	if( pOldFont != NULL )
	{
		pDC->SelectObject(pOldFont);
	}
	pDC->RestoreDC( nSaveDC );
	return;
}

//***************************************************************************
//
//***************************************************************************
void CSliderButton::Size( const CRect& rectNewSize, const DWORD dwStyle )
{
	//
	// Only update if bitmaps have changed from the previous size.
	//
	if( dwStyle & SOB_VIEW_HORZ )
		if( m_rectButton.Width() == rectNewSize.Width() )
			return;

	if( dwStyle & SOB_VIEW_VERT )
		if( m_rectButton.Height() == rectNewSize.Height() )
			return;

	//
	// Save new rect size and set updatebitmaps to TRUE;
	//
	m_rectButton = rectNewSize;
}

LRESULT CWndSliderView::OnShowContentWnd(WPARAM wParam,LPARAM lParam )
{
	::SendMessage( GetParent()->m_hWnd,WM_SHOW_CONTENTWND,wParam,lParam );
	return 0;
}


