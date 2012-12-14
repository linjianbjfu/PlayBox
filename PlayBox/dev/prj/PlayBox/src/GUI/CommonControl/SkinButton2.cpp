#include "stdafx.h"
#include "SkinButton2.h"
#include "ListCtrol/GdiEx.h"
extern CGdiEx gGdiExOperate;

IMPLEMENT_DYNAMIC(CSkinButton2, CButton)

CSkinButton2::CSkinButton2():
m_pPngImage(NULL),
m_pToolTip(NULL),
m_bMouseOnButton(FALSE),
m_bSelected(FALSE),
m_bMouseDown(FALSE),
m_enmuDrawType(NO_FRAME),
m_bDisable(FALSE),
m_bHandMouse(TRUE),
m_nWidth(12),
m_bUseResponseRect(FALSE)
{
	ColorMatrix  ma = {
		1.00f, 0.00f, 0.00f, 0.00f, 0.00f,

			0.00f, 1.00f, 0.00f, 0.00f, 0.00f,

			0.00f, 0.00f, 1.00f, 0.00f, 0.00f,

			0.00f, 0.00f, 0.00f, 1.00f, 0.00f,

			0.15f, 0.15f, 0.15f, 0.00f, 1.00f	};
	memcpy( &m_LightMat, &ma,  sizeof( ColorMatrix ) );

	ColorMatrix ma2 = {
			0.40f, 0.40f, 0.40f, 0.00f, 0.00f,

				0.50f, 0.50f, 0.50f, 0.00f, 0.00f,

				0.10f, 0.10f, 0.10f, 0.00f, 0.00f,

				0.00f, 0.00f, 0.00f, 1.00f, 0.00f,

				0.00f, 0.00f, 0.00f, 0.00f, 1.00f	};

	::memcpy( &m_GrayMat, &ma2, sizeof( ColorMatrix ) );

	m_colFrame1.SetValue( Gdiplus::Color::MakeARGB(50,0,0,0) );
	m_colFrame2.SetValue( Gdiplus::Color::MakeARGB(200,255,255,255)  );
	m_colBrush2.SetValue( Gdiplus::Color::MakeARGB(80,255,255,255)  );
	m_colBrush1.SetValue( Gdiplus::Color::MakeARGB(100,255,255,255)  );
	m_colBK.SetValue( Gdiplus::Color::MakeARGB(255,255,255,255) );
	FontFamily fontFamily(L"宋体");
	mp_font = new Gdiplus::Font( &fontFamily,m_nWidth, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	m_pToolTip = NULL;
	m_enmuDrawType = TOP_ARC;
}

CSkinButton2::~CSkinButton2()
{
	if(m_pToolTip)	delete m_pToolTip;
	if(m_pPngImage) delete m_pPngImage;
	delete mp_font;
}

BEGIN_MESSAGE_MAP(CSkinButton2, CButton)
	ON_WM_CREATE()
	//	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,   OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER,   OnMouseHover)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//	ON_WM_PAINT()
END_MESSAGE_MAP()

// CSkinButton2 消息处理程序
int CSkinButton2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}
void CSkinButton2::SetCheck(int nCheck)
{
	m_bSelected = BOOL(nCheck);
	if( !m_bSelected )
	{
		m_bMouseDown = false;
	}
	this->RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);

	CRect rect;
	gGdiExOperate.GetWindowRectInParentCoordinates( this->m_hWnd, &rect );
	this->GetParent()->InvalidateRect( &rect, TRUE );
}
int CSkinButton2::GetCheck()const
{
	return (int)m_bSelected;
}
BOOL CSkinButton2::SetSkin(CString str,BOOL bDisable)
{
	if( m_pPngImage != NULL )
		delete m_pPngImage;
	m_pPngImage = new CGdiPlusBitmap;

	WCHAR   wFile[MAX_PATH];   
#ifndef   UNICODE   
	MultiByteToWideChar(CP_ACP,   0,   LPCTSTR(str),   -1,   wFile,   MAX_PATH);   
#else   
	lstrcpy(wFile,   szFile);   
#endif   
	bool bReturn = m_pPngImage->Load( wFile );
	if( bReturn ) this->Invalidate();

	m_bDisable = bDisable;
	return bReturn;
}
void CSkinButton2::SetBkGround(CDC* pDC)
{
	CRect rect, rectS;
	CBitmap bmp, *pOldBitmap;

	GetClientRect(rect);
	GetWindowRect(rectS);
	GetParent()->ScreenToClient(rectS);

	m_dcBk.DeleteDC();

	m_dcBk.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	pOldBitmap = m_dcBk.SelectObject(&bmp);
	m_dcBk.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, rectS.left, rectS.top, SRCCOPY);
	bmp.DeleteObject();
	Invalidate(true);
}
BOOL CSkinButton2::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;//CButton::OnEraseBkgnd(pDC);
}

void CSkinButton2::SendMouseMessageToParent(UINT msg, UINT nFlags, CPoint point)
{
	WPARAM wParam;
	LPARAM lParam;

	ClientToScreen( &point);
	::ScreenToClient( GetParent()->m_hWnd,&point );

	wParam = nFlags;
	lParam = (WORD)point.x;
	lParam += ((WORD)point.y) << 16;		
	::SendMessage(this->GetParent()->m_hWnd,msg, wParam,lParam );	
}

void CSkinButton2::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW);

	CButton::PreSubclassWindow();
}
void CSkinButton2::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bUseResponseRect && !m_rcResponse.PtInRect(point))
	{
		//CButton::OnMouseMove(nFlags, point);
		SendMouseMessageToParent(WM_MOUSEMOVE, nFlags, point);
		return;
	}
	if( !m_bSelected && IsWindowEnabled()&&m_bHandMouse )
	{
#ifndef IDC_HAND
#define IDC_HAND            MAKEINTRESOURCE(32649)
#endif
		::SetCursor(::LoadCursor(NULL, IDC_HAND));
	}

	if (!m_bMouseOnButton) 
	{ 
		TRACKMOUSEEVENT   tme; 
		tme.cbSize   =   sizeof(tme); 
		tme.hwndTrack   =   m_hWnd; 
		tme.dwFlags   =   TME_LEAVE   |   TME_HOVER; 
		tme.dwHoverTime   =   1; 
		m_bMouseOnButton   =   _TrackMouseEvent(&tme); 
	} 
	CButton::OnMouseMove(nFlags, point);
}
LRESULT   CSkinButton2::OnMouseHover(WPARAM   wParam,   LPARAM   lParam) 
{ 	
	m_bMouseOnButton = TRUE;
	this->Invalidate();

	DeleteToolTip();

	SetToolTipText(m_tooltext);

	if (m_pToolTip != NULL)
	{
		if (::IsWindow(m_pToolTip->m_hWnd))
		{
		m_pToolTip->Update();
		}
	}
	return 1;
} 
LRESULT  CSkinButton2::OnMouseLeave(WPARAM   wParam,   LPARAM   lParam) 
{ 	
	m_bMouseOnButton = FALSE;
	this->Invalidate();
	return 1;
}
void CSkinButton2::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_bUseResponseRect && !m_rcResponse.PtInRect(point))
	{
		//CButton::OnMouseMove(nFlags, point);
		SendMouseMessageToParent(WM_LBUTTONDOWN, nFlags, point);
		return;
	}

	if( m_bSelected )
	{
		this->SetCapture();
		return ;
	}
	m_bMouseDown = TRUE;
	CButton::OnLButtonDown(nFlags, point);
}

void CSkinButton2::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_bUseResponseRect && !m_rcResponse.PtInRect(point))
	{
		//CButton::OnMouseMove(nFlags, point);
		::ReleaseCapture();
		SendMouseMessageToParent(WM_LBUTTONUP, nFlags, point);
		return;
	}

	if( m_bSelected )
	{
		::ReleaseCapture();
		return ;
	}
	m_bMouseDown = FALSE;
	CButton::OnLButtonUp(nFlags, point);
}

void CSkinButton2::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{	
	UINT lState = lpDrawItemStruct->itemState;	

	CRect rect;
	GetClientRect(rect);

	CMemDC pDC( CDC::FromHandle(lpDrawItemStruct->hDC), rect);
	Gdiplus::Graphics graphics(pDC->m_hDC);

	// get background
	CBitmap bmp, *pOldBitmap;
	if (m_dcBk.m_hDC == NULL)
	{
		CRect rect1;
		CClientDC clDC(GetParent());
		GetWindowRect(rect1);
		GetParent()->ScreenToClient(rect1);

		m_dcBk.CreateCompatibleDC(&clDC);
		bmp.CreateCompatibleBitmap(&clDC, rect.Width(), rect.Height());
		pOldBitmap = m_dcBk.SelectObject(&bmp);
		m_dcBk.BitBlt(0, 0, rect.Width(), rect.Height(), &clDC, rect1.left, rect1.top, SRCCOPY);
		bmp.DeleteObject();
	}
	// paint backGround

	pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &m_dcBk, 0, 0, SRCCOPY);

	if( m_bDisable )
	{
		DrawImage(graphics);
	}
	else
	{
		if (lState & ODS_DISABLED)
		{
			DrawImage(graphics, DRAW_GRAY);
		}
		else
		{
			if (m_bSelected )
			{
				DrawFrame(graphics,rect );
				DrawImage(graphics, DRAW_LIGHT);
			}
			else
			{
				if (lState & ODS_SELECTED)
				{
					DrawFrame(graphics,rect );
					if( m_enmuDrawType != NO_FRAME_SELECT )
					{
						DrawImage(graphics, DRAW_LIGHT);
					}
				}
				else if (m_bMouseOnButton)
				{
					DrawFrame(graphics,rect );
					if( m_enmuDrawType != NO_FRAME_SELECT )
					{
						DrawImage(graphics,DRAW_LIGHT);
					}	
				}
				else if (lState & ODS_FOCUS)
				{				
					if( m_enmuDrawType == NO_FRAME_SELECT )
					{
						DrawFrame(graphics,rect );
					}
					if( m_enmuDrawType != NO_FRAME_SELECT )
					{
						DrawImage(graphics);
					}
				}
				else
				{
					if( m_enmuDrawType == NO_FRAME_SELECT )
					{
						DrawFrame(graphics,rect );
					}
					if( m_enmuDrawType != NO_FRAME_SELECT )
					{
						DrawImage(graphics);
					}
				}
			}
			//graphics
		}
	}

	if( m_strText.GetLength() > 0 && m_pPngImage == NULL )
	{
		Gdiplus::Bitmap bmpText( rect.Width(), rect.Height(), &graphics );
		Graphics* Ptext=Graphics::FromImage(&bmpText);
		//	Ptext->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

		//Matrix mx(0.25f,0,0,0.25f,1,1);
		//Ptext->SetTransform(&mx);

		Gdiplus::RectF rc( rect.left, rect.top, rect.Width(), rect.Height() );

		WCHAR   wFile[1024];   
#ifndef   UNICODE   
		MultiByteToWideChar(CP_ACP,   0,   LPCTSTR(m_strText),   -1,   wFile,  1024 );   
#else   
		lstrcpy(wFile,   szFile);   
#endif 

		StringFormat format;
		format.SetAlignment( StringAlignmentCenter );

		Gdiplus::RectF rectText, rectBack;
		rectText.X = rc.X;
		rectText.Y = rc.Y+ ( rc.Height-m_nWidth)/2;
		rectText.Width = rc.Width;
		rectText.Height = m_nWidth + rc.Height/2;

		rectBack.X = rc.X+2;
		rectBack.Y = rc.Y+1+ ( rc.Height-m_nWidth)/2;
		rectBack.Width = rc.Width-2;
		rectBack.Height = m_nWidth + rc.Height/2-1;

		Ptext->DrawString( wFile,-1 ,mp_font,rectBack,&format,&SolidBrush(Color(100,0,0,0)) );
		//graphics.SetInterpolationMode( Gdiplus::InterpolationModeHighQualityBicubic);
		//graphics.SetTextRenderingHint(  Gdiplus::TextRenderingHintAntiAlias);
		graphics.DrawImage(&bmpText,rc,0,0,bmpText.GetWidth(),bmpText.GetHeight(),UnitPixel);
		graphics.DrawString(wFile,-1,mp_font,rectText,&format,&SolidBrush(Color(255,255,255,255)));
	}
}

void CSkinButton2::DrawImage(Gdiplus::Graphics& gdi,BTN_DRAW_HUE hue)
{
	if(hue == DRAW_NORMAL)
	{
		if( m_pPngImage && m_pPngImage->m_pBitmap )
		{
			RectF grect; grect.X=0, grect.Y=0; grect.Width = m_pPngImage->m_pBitmap->GetWidth(); grect.Height = m_pPngImage->m_pBitmap->GetHeight();
			gdi.DrawImage(*m_pPngImage,grect );
		}
	}
	else if(hue == DRAW_LIGHT)
	{
		if( m_pPngImage && m_pPngImage->m_pBitmap )
		{
			ImageAttributes ia;
			ia.SetColorMatrix(&m_LightMat);
			RectF grect; grect.X=0, grect.Y=0; grect.Width = m_pPngImage->m_pBitmap->GetWidth(); grect.Height = m_pPngImage->m_pBitmap->GetHeight();
			gdi.DrawImage(*m_pPngImage, grect, 0, 0, grect.Width, grect.Height, UnitPixel, &ia);
		}
	}
	else if(hue == DRAW_GRAY )
	{
		if( m_pPngImage && m_pPngImage->m_pBitmap )
		{
			ImageAttributes ia;
			ia.SetColorMatrix(&m_GrayMat);
			RectF grect; grect.X=0, grect.Y=0; grect.Width = m_pPngImage->m_pBitmap->GetWidth(); grect.Height = m_pPngImage->m_pBitmap->GetHeight();
			gdi.DrawImage(*m_pPngImage, grect, 0, 0, grect.Width, grect.Height, UnitPixel, &ia);
		}
	}
}
void CSkinButton2::DrawFrame(Gdiplus::Graphics& gdi,CRect& rect)
{
	if( m_enmuDrawType == NO_FRAME )
	{
		Gdiplus::Pen pen1( m_colFrame1 );
		gdi.DrawLine( &pen1, rect.left+1, rect.top, rect.right-2, rect.top );
		gdi.DrawLine( &pen1, rect.left+1, rect.bottom-1, rect.right-2, rect.bottom-1 );
		gdi.DrawLine( &pen1, rect.left, rect.top+1, rect.left, rect.bottom-2 );
		gdi.DrawLine( &pen1, rect.right-1, rect.top+1, rect.right-1, rect.bottom-2 );

		Gdiplus::Color colpix;
		pen1.GetColor( &colpix );
		colpix.SetValue( Gdiplus::Color::MakeARGB(colpix.GetA()/2,colpix.GetR(),colpix.GetG(),colpix.GetB() ) );
		Gdiplus::Pen penPix1( colpix );
		gdi.DrawLine( &penPix1, rect.left, rect.top, rect.left+1, rect.top );
		gdi.DrawLine( &penPix1, rect.right-1, rect.top, rect.right-1, rect.top+1 );
		gdi.DrawLine( &penPix1, rect.right-1, rect.bottom-1, rect.right-2, rect.bottom-1 );
		gdi.DrawLine( &penPix1, rect.left,  rect.bottom-1, rect.left+1,  rect.bottom-1 );

		CRect rect2 = rect;
		::InflateRect( &rect2, -1,-1 );
		if( !m_bMouseDown )
		{
			Gdiplus::Pen pen2(m_colFrame2);
			if( m_bMouseDown )
				pen2.SetColor(m_colFrame1);
			else
				pen2.SetColor(m_colFrame2);

			gdi.DrawLine( &pen2, rect2.left+1, rect2.top, rect2.right-2, rect2.top );
			gdi.DrawLine( &pen2, rect2.left+1, rect2.bottom-1, rect2.right-2, rect2.bottom-1 );
			gdi.DrawLine( &pen2, rect2.left, rect2.top+1, rect2.left, rect2.bottom-2 );
			gdi.DrawLine( &pen2, rect2.right-1, rect2.top+1, rect2.right-1, rect2.bottom-2 );

			Gdiplus::Color colpix2;
			pen2.GetColor( &colpix2 );
			colpix2.SetValue( Gdiplus::Color::MakeARGB(colpix2.GetA()/2,colpix2.GetR(),colpix2.GetG(),colpix2.GetB() ) );
			Gdiplus::Pen penPix2( colpix2 );
			gdi.DrawLine( &penPix2, rect2.left, rect2.top, rect2.left+1, rect2.top );
			gdi.DrawLine( &penPix2, rect2.right-1, rect2.top, rect2.right-1, rect2.top+1 );
			gdi.DrawLine( &penPix2, rect2.right-1, rect2.bottom-1, rect2.right-2, rect2.bottom-1 );
			gdi.DrawLine( &penPix2, rect2.left,  rect2.bottom-1, rect2.left+1,  rect2.bottom-1 );
		}

		if( m_bMouseDown )
		{
			Gdiplus::RectF rc( rect2.left, rect2.top, rect2.Width(), rect2.Height()/3 );
			Gdiplus::Color colBrush1,colBrush2;
			colBrush1.SetValue( Gdiplus::Color::MakeARGB(15,1,1,1)  );
			colBrush2.SetValue( Gdiplus::Color::MakeARGB(0,1,1,1)  );

			LinearGradientBrush brush( rc, colBrush1, colBrush2,LinearGradientModeVertical );
			gdi.FillRectangle( &brush, rc );
		}
		else
		{
			Gdiplus::RectF rc( rect2.left, rect2.top, rect2.Width(), rect2.Height()/2 );
			rc.Inflate(-1,-1);
			LinearGradientBrush brush( rc, m_colBrush1, m_colBrush2,LinearGradientModeVertical );
			gdi.FillRectangle( &brush, rc );
		}

		return;
	}
	else if( m_enmuDrawType == NO_FRAME_SELECT )
	{
		Gdiplus::Pen pen1( m_colFrame1 );
		gdi.DrawLine( &pen1, rect.left+1, rect.top, rect.right-2, rect.top );
		gdi.DrawLine( &pen1, rect.left+1, rect.bottom-1, rect.right-2, rect.bottom-1 );
		gdi.DrawLine( &pen1, rect.left, rect.top+1, rect.left, rect.bottom-2 );
		gdi.DrawLine( &pen1, rect.right-1, rect.top+1, rect.right-1, rect.bottom-2 );

		Gdiplus::Color colpix;
		pen1.GetColor( &colpix );
		colpix.SetValue( Gdiplus::Color::MakeARGB(colpix.GetA()/2,colpix.GetR(),colpix.GetG(),colpix.GetB() ) );
		Gdiplus::Pen penPix1( colpix );
		gdi.DrawLine( &penPix1, rect.left, rect.top, rect.left+1, rect.top );
		gdi.DrawLine( &penPix1, rect.right-1, rect.top, rect.right-1, rect.top+1 );
		gdi.DrawLine( &penPix1, rect.right-1, rect.bottom-1, rect.right-2, rect.bottom-1 );
		gdi.DrawLine( &penPix1, rect.left,  rect.bottom-1, rect.left+1,  rect.bottom-1 );

		CRect rect2 = rect;
		::InflateRect( &rect2, -1,-1 );
		if( !m_bMouseDown )
		{
			Gdiplus::Pen pen2(m_colFrame2);
			if( m_bMouseDown )
				pen2.SetColor(m_colFrame1);
			else
				pen2.SetColor(m_colFrame2);

			gdi.DrawLine( &pen2, rect2.left+1, rect2.top, rect2.right-2, rect2.top );
			gdi.DrawLine( &pen2, rect2.left+1, rect2.bottom-1, rect2.right-2, rect2.bottom-1 );
			gdi.DrawLine( &pen2, rect2.left, rect2.top+1, rect2.left, rect2.bottom-2 );
			gdi.DrawLine( &pen2, rect2.right-1, rect2.top+1, rect2.right-1, rect2.bottom-2 );

			Gdiplus::Color colpix2;
			pen2.GetColor( &colpix2 );
			colpix2.SetValue( Gdiplus::Color::MakeARGB(colpix2.GetA()/2,colpix2.GetR(),colpix2.GetG(),colpix2.GetB() ) );
			Gdiplus::Pen penPix2( colpix2 );
			gdi.DrawLine( &penPix2, rect2.left, rect2.top, rect2.left+1, rect2.top );
			gdi.DrawLine( &penPix2, rect2.right-1, rect2.top, rect2.right-1, rect2.top+1 );
			gdi.DrawLine( &penPix2, rect2.right-1, rect2.bottom-1, rect2.right-2, rect2.bottom-1 );
			gdi.DrawLine( &penPix2, rect2.left,  rect2.bottom-1, rect2.left+1,  rect2.bottom-1 );
		}

	/*	if( m_bMouseDown )
		{
			Gdiplus::RectF rc( rect2.left, rect2.top, rect2.Width(), rect2.Height()/3 );
			Gdiplus::Color colBrush1,colBrush2;
			colBrush1.SetValue( Gdiplus::Color::MakeARGB(15,1,1,1)  );
			colBrush2.SetValue( Gdiplus::Color::MakeARGB(0,1,1,1)  );

			LinearGradientBrush brush( rc, colBrush1, colBrush2,LinearGradientModeVertical );
			gdi.FillRectangle( &brush, rc );
		}
		else
		{
			Gdiplus::RectF rc( rect2.left, rect2.top, rect2.Width(), rect2.Height()/2 );
			rc.Inflate(-1,-1);
			LinearGradientBrush brush( rc, m_colBrush1, m_colBrush2,LinearGradientModeVertical );
			gdi.FillRectangle( &brush, rc );
		}*/

		CRect rect3 = rect2;
		::InflateRect( &rect3, -1,-1 );
		Gdiplus::RectF rc( rect3.left, rect3.top, rect3.Width(), rect3.Height() );
		LinearGradientBrush brush( rc, m_colBK, m_colBK,LinearGradientModeVertical );
		gdi.FillRectangle( &brush, rc );

		Gdiplus::Color colBlack1 = Gdiplus::Color::MakeARGB(30,50,50,50);
		Gdiplus::Color colBlack2 = Gdiplus::Color::MakeARGB(15,50,50,50);
	    Gdiplus::RectF rc1( rect3.left, rect3.top+rect3.Height()/2, rect3.Width(), rect3.Height()/2 );
		LinearGradientBrush brush2( rc1, colBlack1, colBlack2,LinearGradientModeVertical );
		gdi.FillRectangle( &brush2, rc1 );
		return;
	}
	if( m_enmuDrawType == TOP_ARC )
	{
		Gdiplus::Pen pen1( m_colFrame1 );
		gdi.DrawLine( &pen1, rect.left+5, rect.top, rect.right-7, rect.top );
		gdi.DrawLine( &pen1, rect.left, rect.bottom-1, rect.right, rect.bottom-1 );
		gdi.DrawLine( &pen1, rect.left, rect.top+5, rect.left, rect.bottom-2 );
		gdi.DrawLine( &pen1, rect.right-1, rect.top+5, rect.right-1, rect.bottom-2 );

		CRect rect2 = rect;
		::InflateRect( &rect2, -1,-1 );
		if( !m_bMouseDown )
		{
			Gdiplus::Pen pen2(m_colFrame2);
			gdi.DrawLine( &pen2, rect2.left+4, rect2.top, rect2.right-5, rect2.top );
			gdi.DrawLine( &pen2, rect2.left, rect2.bottom-1, rect2.right-1, rect2.bottom-1 );
			gdi.DrawLine( &pen2, rect2.left, rect2.top+4, rect2.left, rect2.bottom-2 );
			gdi.DrawLine( &pen2, rect2.right-1, rect2.top+4, rect2.right-1, rect2.bottom-2 );

			Gdiplus::RectF rectLeftTop2;
			rectLeftTop2.X = rect2.left;
			rectLeftTop2.Y  = rect2.top;
			rectLeftTop2.Width = 6;
			rectLeftTop2.Height  = 6;
			gdi.DrawArc(  &pen2, rectLeftTop2,180,90 );

			Gdiplus::RectF rectRightTop2;
			rectRightTop2.X = rect2.right-7;
			rectRightTop2.Y  = rect2.top;
			rectRightTop2.Width = 6;
			rectRightTop2.Height  = 6;
			gdi.DrawArc(  &pen2, rectRightTop2,270,90 );
		}

		Gdiplus::RectF rectLeftTop;
		rectLeftTop.X = rect.left;
		rectLeftTop.Y  = rect.top;
		rectLeftTop.Width = 9;
		rectLeftTop.Height  = 9;
		gdi.DrawArc(  &pen1, rectLeftTop,180,90 );

		Gdiplus::RectF rectRightTop;
		rectRightTop.X = rect.right-10;
		rectRightTop.Y  = rect.top;
		rectRightTop.Width = 9;
		rectRightTop.Height  = 9;
		gdi.DrawArc(  &pen1, rectRightTop,270,90 );

		/*Gdiplus::SolidBrush sb( m_colBrush );
		Gdiplus::Brush *pbrush = sb.Clone();*/
		if( m_bMouseDown )
		{
			Gdiplus::RectF rc( rect2.left, rect2.top, rect2.Width(), rect2.Height()/3 );
			Gdiplus::Color colBrush1,colBrush2;
			colBrush1.SetValue( Gdiplus::Color::MakeARGB(15,1,1,1)  );
			colBrush2.SetValue( Gdiplus::Color::MakeARGB(0,1,1,1)  );

			LinearGradientBrush brush( rc, colBrush1, colBrush2,LinearGradientModeVertical );
			gdi.FillRectangle( &brush, rc );
		}
		else
		{
			Gdiplus::RectF rc( rect2.left, rect2.top, rect2.Width(), rect2.Height()/2 );
			rc.Inflate(-1,-1);
			LinearGradientBrush brush( rc, m_colBrush1, m_colBrush2,LinearGradientModeVertical );
			gdi.FillRectangle( &brush, rc );
		}
	}
	else if( m_enmuDrawType == BOTTON_ARC )
	{
	}
	else if( m_enmuDrawType == RIGHT_ARC )
	{
	}
	else if( m_enmuDrawType == LEFT_ARC )
	{
	}
	else if( m_enmuDrawType & (TOP_ARC|BOTTON_ARC) || m_enmuDrawType & (RIGHT_ARC|LEFT_ARC) )
	{
		// 四角都有弧线
	}

	//gdi.DrawString(
}

void CSkinButton2::SetWindowsText(CString str)
{
	m_strText = str;
	if( this->m_hWnd )
		this->Invalidate();
}

void CSkinButton2::GetWindowsText(CString& str)
{
	if( !m_strText.IsEmpty() )
	str = m_strText;
}

void CSkinButton2::SetFrameColor1( BYTE a, BYTE r, BYTE g, BYTE b)
{
	m_colFrame1.SetValue( Gdiplus::Color::MakeARGB(a,r,g,b) );
}
void CSkinButton2::SetFrameColor2( BYTE a, BYTE r, BYTE g, BYTE b)
{
	m_colFrame2.SetValue( Gdiplus::Color::MakeARGB(a,r,g,b) );
}
void CSkinButton2::SetFrameBrush( BYTE a, BYTE r, BYTE g, BYTE b)
{
	m_colBrush1.SetValue( Gdiplus::Color::MakeARGB(a,r,g,b) );
}
void CSkinButton2::SetBKColor( BYTE a, BYTE r, BYTE g, BYTE b)
{
	m_colBK.SetValue( Gdiplus::Color::MakeARGB(a,r,g,b) );
	if( this->m_hWnd )
	this->Invalidate();
}
COLORREF CSkinButton2::GetBKColor()const
{
   return RGB( m_colBK.GetR(), m_colBK.GetG(), m_colBK.GetB() );
}
BOOL CSkinButton2::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	if (m_pToolTip != NULL)
	{
		if (::IsWindow(m_pToolTip->m_hWnd))
		{
			m_pToolTip->RelayEvent(pMsg);		
		}
	}

	return CButton::PreTranslateMessage(pMsg);
}
void CSkinButton2::SetToolTipText(CString sText, BOOL bActivate)
{
	if (sText.IsEmpty()) return;

	InitToolTip();
	m_tooltext = sText;

	if (m_pToolTip->GetToolCount() == 0)
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_pToolTip->AddTool(this, m_tooltext, rectBtn, 1);
	}

	m_pToolTip->UpdateTipText(m_tooltext, this, 1);
	m_pToolTip->SetDelayTime(2000);
	m_pToolTip->Activate(bActivate);
}

void CSkinButton2::InitToolTip()
{
	if (m_pToolTip == NULL)
	{
		m_pToolTip = new CToolTipCtrl;
		// Create ToolTip control
		m_pToolTip->Create(this);
		m_pToolTip->Activate(TRUE);
	}
} 

void CSkinButton2::DeleteToolTip()
{
	if (m_pToolTip != NULL)
	{
		delete m_pToolTip;
		m_pToolTip = NULL;
	}
}
CSize CSkinButton2::GetSkinSize()
{
	CSize size(0,0);
	if( NULL != m_pPngImage )
	{
		size.cx = m_pPngImage->m_pBitmap->GetWidth();
		size.cy = m_pPngImage->m_pBitmap->GetHeight();
	}
	return size;
}