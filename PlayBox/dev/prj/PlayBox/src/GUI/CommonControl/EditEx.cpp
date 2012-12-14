// EditEx.cpp : implementation file
//

#include "stdafx.h"
#include "EditEx.h"

// CEditEx

IMPLEMENT_DYNAMIC(CEditEx, CEdit)
CEditEx::CEditEx():
m_textSize(9),
 m_bMouseOn(FALSE)
{
}

CEditEx::~CEditEx()
{
}


BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	ON_WM_CREATE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER,OnMouseHover)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CEditEx::SetTextSize( UINT textSize)
{
	m_textSize = textSize;
}

int CEditEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	CDC* lpDC;
	lpDC = GetDC();
	
	LOGFONT  lf;
	ZeroMemory(&lf,sizeof(lf));
	lf.lfHeight= -MulDiv(m_textSize,GetDeviceCaps(lpDC->m_hDC,LOGPIXELSY),72);
	lf.lfWeight=FW_NORMAL;
	lf.lfCharSet=ANSI_CHARSET;
	lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
	lf.lfQuality=DEFAULT_QUALITY;
	lf.lfPitchAndFamily=FF_DONTCARE | DEFAULT_PITCH;

	m_font.CreateFontIndirect(&lf);
	SetFont( &m_font );

	lpDC->SetTextAlign( TA_BOTTOM  );
	
	ReleaseDC( lpDC );

	SetMargins(4,0);
	return 0;
}

LRESULT   CEditEx::OnMouseHover(WPARAM   wParam,   LPARAM   lParam) 
{ 	
	m_bMouseOn = TRUE;
	::SendMessage( this->GetParent()->m_hWnd, MSG_MOUSEHOVER_EDIT, GetWindowLong(this->m_hWnd, GWL_ID),0 );
	return 1;
} 
LRESULT   CEditEx::OnMouseLeave(WPARAM   wParam,   LPARAM   lParam) 
{ 	
	m_bMouseOn = FALSE;
	::SendMessage( this->GetParent()->m_hWnd, MSG_MOUSELEAVE_EDIT, GetWindowLong(this->m_hWnd, GWL_ID),0 );
	return 1;
}
void CEditEx::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if   (!m_bMouseOn) 
	{ 
		TRACKMOUSEEVENT   tme;
		tme.cbSize   =   sizeof(tme); 
		tme.hwndTrack   =   m_hWnd; 
		tme.dwFlags   =   TME_LEAVE   |   TME_HOVER; 
		tme.dwHoverTime   =   1; 
		m_bMouseOn   =   _TrackMouseEvent(&tme); 
	} 

	CEdit::OnMouseMove(nFlags, point);
}

void CEditEx::OnPaint()
{
	bool bDrawTip = false;
	CString cText;
	if( GetFocus()!= this )
	{
		GetWindowText(cText);
		if( cText.Trim().GetLength() == 0 )
		{
			cText = SEARCH_HELP;
			bDrawTip = true;
		}
	}

	CPaintDC dc(this); 
	CRect rectText;
	GetClientRect(&rectText); 

	COLORREF clrText;
	if( bDrawTip )
	{
		clrText = ::AfxGetUIManager()->UIGetSkinMgr()->GetColor("SearchInputEditTip");
	}else
	{
		clrText	= ::AfxGetUIManager()->UIGetSkinMgr()->GetColor("SearchInputEditText");
	}
	dc.SetTextColor( clrText );

	CFont   *oldfont;   
	oldfont = dc.SelectObject( &m_font);
	
	COLORREF clrBk = ::AfxGetUIManager()->UIGetSkinMgr()->GetColor("SearchInputEditBK");
	dc.FillSolidRect(&rectText,clrBk);

	rectText.left += 4;
	dc.DrawText( cText,rectText, DT_VCENTER|DT_LEFT|DT_SINGLELINE );
	dc.SelectObject( oldfont );
	SetSel(-1, -1);
}

BOOL CEditEx::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	if (message != WM_CTLCOLOREDIT) 
	{
		return CEdit::OnChildNotify(message, wParam, lParam, pLResult);
	}

	HDC hdcChild = (HDC)wParam;
	COLORREF clrText , clrBk;
	clrText	= ::AfxGetUIManager()->UIGetSkinMgr()->GetColor("SearchInputEditText");
	clrBk = ::AfxGetUIManager()->UIGetSkinMgr()->GetColor("SearchInputEditBK");

	SetTextColor(hdcChild, clrText);
	SetBkColor(hdcChild, clrBk);
	return TRUE;
}