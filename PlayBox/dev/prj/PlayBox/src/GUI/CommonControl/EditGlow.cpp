#include "stdafx.h"
#include "EditGlow.h"


IMPLEMENT_DYNAMIC(CEditGlow, CEdit)
CEditGlow::CEditGlow():
m_textSize(9),
 m_bMouseOn(FALSE)
{
	m_bDrawBorderSelf = false;
	m_pImgBorder = NULL;
	m_bFocus = false;
	m_pImgBorderGlow = NULL;

	m_colText = RGB( 238,238,238 );
	m_colBk	  = RGB( 59,59,59 );
	m_hBrush.CreateSolidBrush( m_colBk );
}

CEditGlow::~CEditGlow()
{
	m_hBrush.DeleteObject();
	delete m_pImgBorder;
	delete m_pImgBorderGlow;
}

void CEditGlow::SetPngBorder(string normalpath, string glowpath)
{
	m_pImgBorder = CloneImage(normalpath);
	m_pImgBorderGlow = CloneImage(glowpath);
}

Image * CEditGlow::CloneImage(string path)
{
	WCHAR szWidePath[2048];   
	long lLen = MultiByteToWideChar(CP_ACP,0,path.c_str(),path.length(),szWidePath,sizeof(szWidePath)); 
	szWidePath[lLen] = '\0';     

	Image image(szWidePath);
	if(image.GetLastStatus() != S_OK)
	{
		return NULL;
	}
	int w = image.GetWidth();
	int h = image.GetHeight(); 
	TRACE("w: %d ,h: %d, %s\r\n", w,h,path.c_str());
	return image.Clone();
}


BEGIN_MESSAGE_MAP(CEditGlow, CEdit)
	ON_WM_CREATE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER,OnMouseHover)
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//ON_WM_CTLCOLOR()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

void CEditGlow::SetTextSize( UINT textSize)
{
	m_textSize = textSize;
}

int CEditGlow::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	lpDC->SetBkMode( TRANSPARENT );
	
	ReleaseDC( lpDC );

	SetMargins(4,0);

	return 0;
}

LRESULT   CEditGlow::OnMouseHover(WPARAM   wParam,   LPARAM   lParam) 
{ 	
	m_bMouseOn = TRUE;
	EraseParentBK();
	return 1;
} 
LRESULT   CEditGlow::OnMouseLeave(WPARAM   wParam,   LPARAM   lParam) 
{ 	
	m_bMouseOn = FALSE;
	EraseParentBK();
	return 1; 
}

void CEditGlow::OnMouseMove(UINT nFlags, CPoint point)
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
		EraseParentBK();
	} 

	CEdit::OnMouseMove(nFlags, point);
}


HBRUSH CEditGlow::CtlColor(CDC* pDC, UINT nCtlColor) 
{


	// TODO: Return a non-NULL brush if the parent's handler should not be called

	//set text color
	pDC->SetTextColor( m_colText );
	//set the text's background color
	pDC->SetBkColor( m_colBk );

	//return the brush used for background this sets control background
	return m_hBrush;
}


void CEditGlow::OnPaint()
{
	CEdit::OnPaint();
}
//
//BOOL CEditGlow::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
//{
//	if (1)//message != WM_CTLCOLOREDIT) 
//	{
//		return CEdit::OnChildNotify(message, wParam, lParam, pLResult);
//	}
//
//	HDC hdcChild = (HDC)wParam;
//	COLORREF clrText , clrBk;
//	clrText	= ::AfxGetUIManager()->UIGetSkinMgr()->GetColor("SearchInputEditText");
//	clrBk = ::AfxGetUIManager()->UIGetSkinMgr()->GetColor("SearchInputEditBK");
//
//	SetTextColor(hdcChild, clrText);
//	SetBkColor(hdcChild, clrBk);
//	return TRUE;
//}

void CEditGlow::ShowSearchEditFrame(CDC *pdc, bool bUse)
{
	CRect rectEdit;  // ËÑË÷¿òÎ»ÖÃ
	CWnd *pWnd = GetParent();
	this->GetWindowRect(rectEdit);
	pWnd->ScreenToClient(&rectEdit);

	//CDC* pdc = pWnd->GetDC();
	Gdiplus::Graphics graph(pdc->m_hDC);
	graph.SetSmoothingMode(Gdiplus::SmoothingModeNone);

	if(m_pImgBorder && m_pImgBorderGlow)
	{
		if(m_bMouseOn || m_bFocus)
		{
			Rect rr(rectEdit.left-2 , rectEdit.top-2 , m_pImgBorderGlow->GetWidth(), m_pImgBorderGlow->GetHeight());

			graph.DrawImage(m_pImgBorderGlow,
				rr,
				(int)0, 
				(int)0,
				(int)m_pImgBorderGlow->GetWidth(), 
				(int)m_pImgBorderGlow->GetHeight(),
				UnitPixel );
		}
		else
		{
			Rect rr(rectEdit.left-2 , rectEdit.top-2 , m_pImgBorder->GetWidth(), m_pImgBorder->GetHeight());

			graph.DrawImage(m_pImgBorder,
				rr,
				(int)0, 
				(int)0,
				(int)m_pImgBorder->GetWidth(), 
				(int)m_pImgBorder->GetHeight(),
				UnitPixel );

		}

	} 

	return;




	ISkinMgr*	pSkinMgr = AfxGetUIManager()->UIGetSkinMgr();

	CDibBitmap*	m_bmpEditLeft = pSkinMgr->GetDibBmp("LoginEditLeftBorder");
	CDibBitmap*	m_bmpEditTop = pSkinMgr->GetDibBmp("LoginEditTopBorder");
	CDibBitmap*	m_bmpEditRight = pSkinMgr->GetDibBmp("LoginEditRightBorder");
	CDibBitmap*	m_bmpEditBottom = pSkinMgr->GetDibBmp("LoginEditBottomBorder");

	CDibBitmap*	m_bmpEditLiftGray = pSkinMgr->GetDibBmp("AuthorListPanelSearchLeftGray");
	CDibBitmap*	m_bmpEditCenterGray = pSkinMgr->GetDibBmp("AuthorListPanelSearchCenterGray");
	CDibBitmap*	m_bmpEditRightGray = pSkinMgr->GetDibBmp("AuthorListPanelSearchRightGray");



	int nTop = rectEdit.top - m_bmpEditTop->GetHeight();
	int nLeft = rectEdit.left-m_bmpEditLeft->GetWidth();

	if( bUse )
	{
		CDC* pdc = pWnd->GetDC();

		m_bmpEditLeft->SetCDibRect(  CRect(nLeft,nTop, nLeft+m_bmpEditLeft->GetWidth(), nTop+m_bmpEditLeft->GetHeight()) );
		m_bmpEditLeft->Draw( pdc, FALSE );

		m_bmpEditRight->SetCDibRect( CRect(rectEdit.right,nTop, rectEdit.right+m_bmpEditRight->GetWidth(), nTop+m_bmpEditRight->GetHeight()) );
		m_bmpEditRight->Draw( pdc, FALSE );

		m_bmpEditTop->SetCDibRect( CRect(rectEdit.left,nTop, rectEdit.left+rectEdit.Width(), nTop+m_bmpEditTop->GetHeight()) );
		m_bmpEditTop->Draw( pdc, FALSE );

		ReleaseDC(pdc);
	}
	else
	{
		CDC* pdc = pWnd->GetDC();

		m_bmpEditLiftGray->SetCDibRect(  CRect(nLeft,nTop, nLeft+m_bmpEditLiftGray->GetWidth(), nTop+m_bmpEditLiftGray->GetHeight()) );
		m_bmpEditLiftGray->Draw( pdc, FALSE );

		m_bmpEditRightGray->SetCDibRect( CRect(rectEdit.right,nTop, rectEdit.right+m_bmpEditRightGray->GetWidth(), nTop+m_bmpEditRightGray->GetHeight()) );
		m_bmpEditRightGray->Draw( pdc, FALSE );

		//m_bmpEditCenterGray->SetCDibRect( CRect(rectEdit.left,nTop, rectEdit.left+rectEdit.Width(), nTop+m_bmpEditCenter->GetHeight()) );
		//m_bmpEditCenter->Draw( pdc, FALSE );

		ReleaseDC(pdc);
	}
}

void CEditGlow::EraseParentBK()
{
	CRect Rect;
	GetWindowRect(&Rect);
	GetParent()->ScreenToClient(&Rect);
	Rect.InflateRect(6,6,6,6);
	GetParent()->InvalidateRect(&Rect);
	//GetParent()->Invalidate();
	//GetParent()->UpdateWindow();
}

void CEditGlow::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);

	m_bFocus = true;
	EraseParentBK();

	// TODO: Add your message handler code here
}

void CEditGlow::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	m_bFocus = false;
	EraseParentBK();

	// TODO: Add your message handler code here
}
//
//HBRUSH CEditGlow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
//{
//	HBRUSH hbr = CEdit::OnCtlColor(pDC, pWnd, nCtlColor);
//
//
//	// TODO: Return a non-NULL brush if the parent's 
//
//	//handler should not be called
//
//
//	//set text color
//
//	pDC->SetTextColor(RGB(0,0,0));
//
//	//set the text's background color
//
//	pDC->SetBkColor(RGB(255,255,255));
//
//	//return the brush used for background this sets control background
//	return m_hBrush;
//}
