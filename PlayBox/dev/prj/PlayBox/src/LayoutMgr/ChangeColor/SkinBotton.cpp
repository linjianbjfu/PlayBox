// SkinBotton.cpp : implementation file
//

#include "stdafx.h"
#include "SkinBotton.h"

#include "BitMapOperate.h"
#include "WndColorMgr.h"

#include "IEvent.h"

extern CBitMapOperate gBitMapOperate;
extern CWndColorMgr   gWndColor;


// CSkinButton

IMPLEMENT_DYNAMIC(CSkinButton, CButton)

CSkinButton::CSkinButton()
{
	m_BitMap = NULL;
	mbMouseOnButton = false;
	m_bottonType =    0;
	m_pbtnEventListerner = 0;
	//m_SkinBitMapIndex = "";
	this->m_strSkinPath = "";
	m_bshowTip = true;
	m_pPngImage = NULL;
	m_bSelected = false;
	m_bEraseBK = false;
}

CSkinButton::~CSkinButton()
{
	if(m_pPngImage) delete m_pPngImage;
}


BEGIN_MESSAGE_MAP(CSkinButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_MOUSELEAVE,   OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER,   OnMouseHover)
END_MESSAGE_MAP()



// CSkinButton message handlers



void CSkinButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	ASSERT(lpDrawItemStruct != NULL);
	CString lstrTitle = GetText();
	lstrTitle.Trim();
	m_ButtonDraw.DrawButton(this, lpDrawItemStruct,m_bottonType, mbMouseOnButton,lstrTitle, m_colBegain, m_colEnd,m_BitMap);

	if( m_pPngImage && m_pPngImage->m_pBitmap && m_bSelected ) 
	{
		Gdiplus::Graphics graphics(lpDrawItemStruct->hDC);
		DrawImage(graphics);
	}
}
LRESULT   CSkinButton::OnMouseHover(WPARAM   wParam,   LPARAM   lParam) 
{ 	
	mbMouseOnButton = true;
	this->Invalidate();

	TRACE("OnBtn\n");
	return 1;
} 
LRESULT   CSkinButton::OnMouseLeave(WPARAM   wParam,   LPARAM   lParam) 
{ 	
	mbMouseOnButton = false;
	this->Invalidate();
	TRACE("LeaveBtn\n");
	return 1;
}
void CSkinButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bshowTip)
	{
		RelayEvent(WM_MOUSEMOVE,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));
	}
	CButton::OnMouseMove(nFlags, point);
	if   (!mbMouseOnButton) 
	{ 
		TRACKMOUSEEVENT   tme; 
		tme.cbSize   =   sizeof(tme); 
		tme.hwndTrack   =   m_hWnd; 
		tme.dwFlags   =   TME_LEAVE   |   TME_HOVER; 
		tme.dwHoverTime   =   1; 
		mbMouseOnButton   =   _TrackMouseEvent(&tme); 

		TRACE("MouseMove\n");
	} 
	else
	{
#ifndef IDC_HAND
#define IDC_HAND            MAKEINTRESOURCE(32649)
#endif
		::SetCursor(::LoadCursor(NULL, IDC_HAND));
	}
}

void CSkinButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	RelayEvent(WM_LBUTTONDOWN,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));
	CButton::OnLButtonDown(nFlags, point);
	if( NULL != m_pbtnEventListerner )
	{
		this->m_pbtnEventListerner->OnCliekButton(this);
	}
	//	::PostMessage(GetParent()->GetSafeHwnd(), MSG_BTN_MOUSEDOWN,::GetWindowLong(this->m_hWnd,GWL_ID ),0 );
}

void CSkinButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	RelayEvent(WM_LBUTTONUP,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));
	CButton::OnLButtonUp(nFlags, point);
}

BOOL CSkinButton::OnEraseBkgnd(CDC* pDC)
{

	if( m_bEraseBK )
	{
		CRect rc;
		this->GetClientRect( &rc );
		pDC->FillRect( &rc, &CBrush( RGB(255,255,255) ) );
		
	}
	return true;
}


CButtonDraw::CButtonDraw()
{
	m_colShadow.SetValue( Gdiplus::Color::MakeARGB(60,100,100,100) );
	m_colShadow2.SetValue( Gdiplus::Color::MakeARGB(20,100,100,100) );
	m_colShadow3.SetValue( Gdiplus::Color::MakeARGB(50,100,100,100) );
	m_colBK.SetValue( Gdiplus::Color::MakeARGB(255,255,255,255) );
}
CButtonDraw:: ~CButtonDraw()
{
}
void CButtonDraw::DrawButton(CWnd* apWnd, LPDRAWITEMSTRUCT lpDrawItemStruct,BOOL m_bottonType,BOOL abMouseOnButton, 
							 CString &astrTitle,COLORREF acolBegain,COLORREF acolEnd,CBitmap*  aBitMap )
{
	RECT lrect = lpDrawItemStruct->rcItem;

	UINT liAlign = GetTextAlign(lpDrawItemStruct->hDC);


	CDC* lpDC = NULL;
	lpDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	//lpDC->FillSolidRect(&lrect, RGB(255,255,255));

	lpDC->SetTextColor(gWndColor.getColor(COLOL_CTROL)->structColBase->colFont);

	UINT lState = lpDrawItemStruct->itemState;	
	bool bSelect= ((CSkinButton*)apWnd)->m_bSelected;

	if( !(lState & ODS_DISABLED))
	{
		if (lState & ODS_SELECTED || bSelect )
		{
			//没要求支持，用 NORMAL 代替
			if( CSkinButton::COLLOR_BOTTON == m_bottonType)
			{
				DrawColorNormal( lpDrawItemStruct->hDC,lrect,astrTitle,acolBegain,acolEnd);
			}
			else if( CSkinButton::SKIN_BOTTON == m_bottonType)
			{
				DrawSkinEnter( lpDrawItemStruct->hDC,lrect,astrTitle,aBitMap);
			}
			else if( CSkinButton::DEFAULT_BOTTON == m_bottonType)
			{
				DrawDefaultButtonNormal( lpDrawItemStruct->hDC,lrect,astrTitle);
			}
			else if( CSkinButton::CLOSE_BOTTON == m_bottonType )
			{
				DrawExitButtonNormal( lpDrawItemStruct->hDC,lrect );
			}
			else if( CSkinButton::COLOR_NOTEXT_BOTTON == m_bottonType )
			{
				DrawColorNoTextNormal( lpDrawItemStruct->hDC,lrect,astrTitle,acolBegain,acolEnd);
			}
		}
		else if (abMouseOnButton)
		{
			if( CSkinButton::COLLOR_BOTTON == m_bottonType)
			{
				DrawColorNormal( lpDrawItemStruct->hDC,lrect,astrTitle,acolBegain,acolEnd);
				DrawColorEnter( lpDrawItemStruct->hDC,lrect,astrTitle,acolBegain,acolEnd);
			}
			else if( CSkinButton::SKIN_BOTTON == m_bottonType)
			{
				//DrawSkinNormal( lpDrawItemStruct->hDC,lrect,astrTitle,aBitMap);
				DrawSkinEnter( lpDrawItemStruct->hDC,lrect,astrTitle,aBitMap);
			}
			else if( CSkinButton::DEFAULT_BOTTON == m_bottonType)
			{
				DrawDefaultButtonEnter( lpDrawItemStruct->hDC,lrect,astrTitle);
			}
			else if( CSkinButton::CLOSE_BOTTON == m_bottonType )
			{
				DrawExitButtonEnter( lpDrawItemStruct->hDC,lrect);
			}
			else if( CSkinButton::COLOR_NOTEXT_BOTTON == m_bottonType )
			{
				DrawColorNoTextEnter( lpDrawItemStruct->hDC,lrect,astrTitle,acolBegain,acolEnd );
			}
		}
		else if (lState & ODS_FOCUS)
		{
			//Focused
			if( CSkinButton::COLLOR_BOTTON == m_bottonType)
			{
				DrawColorNormal( lpDrawItemStruct->hDC,lrect,astrTitle,acolBegain,acolEnd);
			}
			else if( CSkinButton::SKIN_BOTTON == m_bottonType)
			{
				DrawSkinNormal( lpDrawItemStruct->hDC,lrect,astrTitle,aBitMap);
			}
			else if( CSkinButton::DEFAULT_BOTTON == m_bottonType)
			{
				DrawDefaultButtonNormal( lpDrawItemStruct->hDC,lrect,astrTitle);
			}
			else if( CSkinButton::CLOSE_BOTTON == m_bottonType )
			{
				DrawExitButtonNormal( lpDrawItemStruct->hDC,lrect );
			}
			else if( CSkinButton::COLOR_NOTEXT_BOTTON == m_bottonType )
			{
				DrawColorNoTextNormal( lpDrawItemStruct->hDC,lrect,astrTitle,acolBegain,acolEnd );
			}
		}
		else
		{
			if( CSkinButton::COLLOR_BOTTON == m_bottonType)
			{
				DrawColorNormal( lpDrawItemStruct->hDC,lrect,astrTitle,acolBegain,acolEnd);
			}
			else if( CSkinButton::SKIN_BOTTON == m_bottonType)
			{
				DrawSkinNormal( lpDrawItemStruct->hDC,lrect,astrTitle,aBitMap);
			}
			else if( CSkinButton::DEFAULT_BOTTON == m_bottonType)
			{
				DrawDefaultButtonNormal( lpDrawItemStruct->hDC,lrect,astrTitle);
			}
			else if( CSkinButton::CLOSE_BOTTON == m_bottonType )
			{
				DrawExitButtonNormal( lpDrawItemStruct->hDC,lrect );
			}
			else if( CSkinButton::COLOR_NOTEXT_BOTTON == m_bottonType )
			{
				DrawColorNoTextNormal( lpDrawItemStruct->hDC,lrect,astrTitle,acolBegain,acolEnd );
			}
		}
	}
	else
	{
		if( CSkinButton::SKIN_BOTTON == m_bottonType )
		{
			DrawSkinNormal( lpDrawItemStruct->hDC,lrect,astrTitle,aBitMap);
		}
	}


	// draw text
	SetBkMode(lpDrawItemStruct->hDC, TRANSPARENT);
	LOGFONT  lf;
	ZeroMemory(&lf,sizeof(lf));
	lf.lfHeight=-12;//-MulDiv(9,GetDeviceCaps(lpDC->m_hDC,LOGPIXELSY),72);
	lf.lfWeight=FW_NORMAL;
	lf.lfCharSet=GB2312_CHARSET;
	lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
	lf.lfQuality=DEFAULT_QUALITY;
	lf.lfPitchAndFamily=FF_DONTCARE | DEFAULT_PITCH;
	CFont font;
	font.CreateFontIndirect(&lf);

	CFont * oldFont = lpDC->SelectObject(&font);
	if( CSkinButton::COLLOR_BOTTON == m_bottonType)
	{
		DrawText(
			lpDrawItemStruct->hDC,
			astrTitle,
			astrTitle.GetLength(),
			CRect(
			lrect.left, 
			lrect.top+LEFT_Y_TEXT_SPACE, 
			lrect.right,
			lrect.top+LEFT_Y_TEXT_SPACE+TEXT_WIDTH
			),
			DT_CENTER | DT_VCENTER | DT_SINGLELINE /*| DT_END_ELLIPSIS*/
			);
	}
	else if( CSkinButton::SKIN_BOTTON == m_bottonType)
	{
		/*DrawText(
		lpDrawItemStruct->hDC,
		astrTitle,
		astrTitle.GetLength(),
		CRect(
		lrect.left, 
		lrect.bottom - TEXT_WIDTH,
		lrect.right,
		lrect.bottom
		),
		DT_CENTER | DT_VCENTER | DT_SINGLELINE
		);*/
	}
	else if( CSkinButton::DEFAULT_BOTTON == m_bottonType)
	{
		lpDC->SetTextColor(gWndColor.getColor(TABLE_CTROL)->structColBase->colFrame1);
		DrawText(
			lpDrawItemStruct->hDC,
			astrTitle,
			astrTitle.GetLength(),
			CRect(
			lrect.left, 
			lrect.top,
			lrect.right,
			lrect.bottom
			),
			DT_CENTER | DT_VCENTER | DT_SINGLELINE  
			);
	}
	lpDC->SelectObject(oldFont);
}
//void CButtonDraw::DrawSkinBorderEnter()
//{
//}
//void CButtonDraw::DrawSkinBorderNormal()
//{
//}
//void CButtonDraw::DrawColorBorderEnter()
//{
//}
//void CButtonDraw::DrawColorBorderNormal()
//{
//}
void CButtonDraw::DrawSkinEnter( HDC &aHDC, RECT &aRect,CString &astr,CBitmap* aBitMap)
{
	if( NULL == aBitMap) return ;
	CSize lsize = aBitMap->GetBitmapDimension();

	int lisrcHight = aRect.bottom - aRect.top;
	int lisrcWidth = aRect.right  - aRect.left; 

	int nX = (lisrcWidth-lsize.cx)>4? (lisrcWidth-lsize.cx)/2:2;
	int nY = (lisrcHight-lsize.cy)>4? (lisrcHight-lsize.cy)/2:2;
	CImageList  m_Image;
	m_Image.Create(lsize.cx,lsize.cy,ILC_COLOR24|ILC_MASK,1,1);
	m_Image.Add( aBitMap ,0xff00ff);
	m_Image.Draw( CDC::FromHandle(aHDC),0,CPoint(nX,nY),ILD_NORMAL);
	m_Image.DeleteImageList();

	CRect lrcFrame(nX,nY,nX+lsize.cx, nY+lsize.cy);
	HBRUSH hHandle = 0;
	hHandle = CreateSolidBrush(gWndColor.getColor(COLOL_CTROL)->structColBase->colFrame2);
	//::InflateRect(&lrcFrame,1,1);
	FrameRect(aHDC, &lrcFrame,(HBRUSH)hHandle );
	::InflateRect(&lrcFrame,-1,-1);
	FrameRect(aHDC, &lrcFrame,(HBRUSH)hHandle );
	::InflateRect(&lrcFrame,-1,-1);
	HBRUSH hbru = CreateSolidBrush(RGB(255,255,255));
	FrameRect(aHDC, &lrcFrame,(HBRUSH)hbru );
	DeleteObject((HGDIOBJ)hHandle);
	DeleteObject(hbru);

	CRect rect( nX, nY, nX+lsize.cx, nY+ lsize.cy );
	DrawBmpShade( CDC::FromHandle(aHDC), rect );
}
void CButtonDraw::DrawSkinNormal( HDC &aHDC, RECT &aRect,CString &astr,CBitmap* aBitMap )
{
	if( NULL == aBitMap) return ;
	CSize lsize = aBitMap->GetBitmapDimension();

	int lisrcHight = aRect.bottom - aRect.top;
	int lisrcWidth = aRect.right  - aRect.left; 

	int nX = (lisrcWidth-lsize.cx)>4? (lisrcWidth-lsize.cx)/2:2;
	int nY = (lisrcHight-lsize.cy)>4? (lisrcHight-lsize.cy)/2:2;

	CImageList  m_Image;
	m_Image.Create(lsize.cx,lsize.cy,ILC_COLOR24|ILC_MASK,1,1);
	m_Image.Add( aBitMap ,0xff00ff);
	m_Image.Draw( CDC::FromHandle(aHDC),0,CPoint(nX,nY),ILD_NORMAL);
	m_Image.DeleteImageList();



	//CRect lrcFrame(nX,nY,nX+lsize.cx, nY+lsize.cy);
	//
	//HBRUSH hHandle = 0;
	//hHandle = CreateSolidBrush(gWndColor.getColor(COLOL_CTROL)->structColBase->colFrame1);
	////::InflateRect(&lrcFrame,1,1);
	//FrameRect(aHDC, &lrcFrame,(HBRUSH)hHandle );
	//::InflateRect(&lrcFrame,-1,-1);
	//FrameRect(aHDC, &lrcFrame,(HBRUSH)hHandle );
	//DeleteObject((HGDIOBJ)hHandle);

	CRect rect( nX, nY, nX+lsize.cx, nY+ lsize.cy );
	DrawBmpShade( CDC::FromHandle(aHDC), rect );
}
void CButtonDraw::DrawColorEnter( HDC &aHDC, RECT &aRect,CString &astr, COLORREF &acolBegain, COLORREF &acolEnd )
{
	//HBRUSH hHandle = 0;
	//hHandle = CreateSolidBrush(acolBegain);

	//RECT lrect = CRect(aRect.left+LEFT_X_COL_SPACE, aRect.top+LEFT_Y_COL_SPACE, aRect.left+LEFT_X_COL_SPACE+COL_HIGHT,aRect.top+LEFT_Y_COL_SPACE+COL_HIGHT);

	//FrameRect(aHDC,&lrect,(HBRUSH)hHandle );
	//DeleteObject((HGDIOBJ)hHandle);

	////InflateRect(&lrect,1,1);
	//hHandle = CreateSolidBrush(gWndColor.getColor(COLOL_CTROL)->structColBase->colFrame2 );
	//FrameRect(aHDC, &lrect,(HBRUSH)hHandle );
	//InflateRect(&lrect,-1,-1);
	//FrameRect(aHDC, &lrect,(HBRUSH)hHandle );
	//DeleteObject((HGDIOBJ)hHandle);
}
void CButtonDraw::DrawColorNormal( HDC &aHDC, RECT &aRect,CString &astr, COLORREF &acolBegain, COLORREF &acolEnd )
{
	//HBRUSH hHandle = 0;
	//hHandle = CreateSolidBrush(acolBegain);

	//RECT lrect = CRect(aRect.left+LEFT_X_COL_SPACE, aRect.top+LEFT_Y_COL_SPACE, aRect.left+LEFT_X_COL_SPACE+COL_HIGHT,aRect.top+LEFT_Y_COL_SPACE+COL_HIGHT);
	//gBitMapOperate.FillRectangle(aHDC, lrect,acolBegain,acolEnd, gBitMapOperate.edoVertical,true,COL_HIGHT );

	//FrameRect(aHDC,&lrect,(HBRUSH)hHandle );
	//DeleteObject((HGDIOBJ)hHandle);

	////InflateRect(&lrect,1,1);
	//hHandle = CreateSolidBrush(gWndColor.getColor(COLOL_CTROL)->structColBase->colFrame1 );
	//FrameRect(aHDC, &lrect,(HBRUSH)hHandle );
	//InflateRect(&lrect,-1,-1);
	//FrameRect(aHDC, &lrect,(HBRUSH)hHandle );
	//DeleteObject((HGDIOBJ)hHandle);
}
void CButtonDraw::DrawColorNoTextEnter( HDC &aHDC, RECT &aRect,CString &astr, COLORREF &acolBegain, COLORREF &acolEnd )
{
	//HBRUSH hHandle = 0;
	//hHandle = CreateSolidBrush(acolBegain);

	//RECT lrect = CRect(aRect.left+LEFT_X_COL_NOTEXT_SPACE, aRect.top+LEFT_Y_COL_NOTEXT_SPACE, aRect.left+LEFT_X_COL_NOTEXT_SPACE+COL_HIGHT,aRect.top+LEFT_Y_COL_NOTEXT_SPACE+COL_HIGHT);

	//FrameRect(aHDC,&lrect,(HBRUSH)hHandle );
	//DeleteObject((HGDIOBJ)hHandle);

	////InflateRect(&lrect,1,1);
	//hHandle = CreateSolidBrush(gWndColor.getColor(COLOL_CTROL)->structColBase->colFrame2 );
	//FrameRect(aHDC, &lrect,(HBRUSH)hHandle );
	//InflateRect(&lrect,-1,-1);
	//FrameRect(aHDC, &lrect,(HBRUSH)hHandle );
	//DeleteObject((HGDIOBJ)hHandle);
}
void CButtonDraw::DrawColorNoTextNormal( HDC &aHDC, RECT &aRect,CString &astr, COLORREF &acolBegain, COLORREF &acolEnd )
{
	//HBRUSH hHandle = 0;
	//hHandle = CreateSolidBrush(acolBegain);

	//RECT lrect = CRect(aRect.left+LEFT_X_COL_NOTEXT_SPACE, aRect.top+LEFT_Y_COL_NOTEXT_SPACE, aRect.left+LEFT_X_COL_NOTEXT_SPACE+COL_HIGHT,aRect.top+LEFT_Y_COL_NOTEXT_SPACE+COL_HIGHT);
	//gBitMapOperate.FillRectangle(aHDC, lrect,acolBegain,acolEnd, gBitMapOperate.edoVertical,true,COL_HIGHT );

	//FrameRect(aHDC,&lrect,(HBRUSH)hHandle );
	//DeleteObject((HGDIOBJ)hHandle);

	////InflateRect(&lrect,1,1);
	//hHandle = CreateSolidBrush(gWndColor.getColor(COLOL_CTROL)->structColBase->colFrame1 );
	//FrameRect(aHDC, &lrect,(HBRUSH)hHandle );
	//InflateRect(&lrect,-1,-1);
	//FrameRect(aHDC, &lrect,(HBRUSH)hHandle );
	//DeleteObject((HGDIOBJ)hHandle);
}
void CButtonDraw::DrawDefaultButtonNormal( HDC &aHDC, RECT &aRect,CString &astr )
{
	HBRUSH hHandle = 0;
	hHandle = CreateSolidBrush( gWndColor.getColor(COLOL_CTROL)->structColBase->colFrame1 );
	FrameRect(aHDC,&aRect,(HBRUSH)hHandle );
	DeleteObject((HGDIOBJ)hHandle);
}
void CButtonDraw::DrawDefaultButtonEnter ( HDC &aHDC, RECT &aRect,CString &astr )
{
	HBRUSH hHandle = 0;
	hHandle = CreateSolidBrush( gWndColor.getColor(COLOL_CTROL)->structColBase->colFrame2 );
	FrameRect(aHDC,&aRect,(HBRUSH)hHandle );
	DeleteObject((HGDIOBJ)hHandle);
}
//
void CButtonDraw::DrawExitButtonNormal( HDC &aHDC, RECT &aRect )
{	
	HBRUSH hHandle = 0;
	hHandle = CreateSolidBrush( gWndColor.getColor(TABLE_CTROL)->structColBase->colFrame1 );
	FrameRect(aHDC,&aRect,(HBRUSH)hHandle );
	gBitMapOperate.DrawLine( aHDC, RGB(152, 152, 152), 2, aRect.left+8,aRect.top+8,aRect.left+15,aRect.top+15, true);
	gBitMapOperate.DrawLine( aHDC, RGB(152, 152, 152), 2, aRect.left+8,aRect.top+15,aRect.left+15,aRect.top+8, true);
	DeleteObject((HGDIOBJ)hHandle);
}
//
void CButtonDraw::DrawExitButtonEnter(  HDC &aHDC, RECT &aRect )
{
	HBRUSH hHandle = 0;
	hHandle = CreateSolidBrush( gWndColor.getColor(TABLE_CTROL)->structColBase->colFrame1 );
	FrameRect(aHDC,&aRect,(HBRUSH)hHandle );
	gBitMapOperate.DrawLine( aHDC, RGB(125, 125, 125), 2, aRect.left+8,aRect.top+8,aRect.left+15,aRect.top+15, true);
	gBitMapOperate.DrawLine( aHDC, RGB(125, 125, 125), 2, aRect.left+8,aRect.top+15,aRect.left+15,aRect.top+8, true);
	DeleteObject((HGDIOBJ)hHandle);
}

void CSkinButton::OnDestroy()
{
	CButton::OnDestroy();

	//if(NULL != m_BitMap)
	//{
	//	m_BitMap->Detach();
	//	delete m_BitMap;
	//	m_BitMap = NULL;
	//}
}
void CSkinButton::SetToolTipText(CString s)
{
	m_strToolTip = s;
	if(m_tooltip.m_hWnd==NULL)
	{
		if(m_tooltip.Create(this,TTS_ALWAYSTIP|WS_POPUP|WS_EX_TOOLWINDOW ))	//first assignment
		{
			if(m_tooltip.AddTool(this, (LPCTSTR)s))
				m_tooltip.Activate(1);
		}
	} 
	m_tooltip.SetMaxTipWidth(150);
	m_tooltip.UpdateTipText((LPCTSTR)s,this);
}
void CSkinButton::RelayEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	// This function will create a MSG structure, fill it in a pass it to
	// the ToolTip control, m_ttip.  Note that we ensure the point is in window
	// coordinates (relative to the control's window).
	if(NULL != m_tooltip.m_hWnd){
		MSG msg;
		msg.hwnd = m_hWnd;
		msg.message = message;
		msg.wParam = wParam;
		msg.lParam = lParam;
		msg.time = 0;
		msg.pt.x = LOWORD(lParam);
		msg.pt.y = HIWORD(lParam);

		m_tooltip.RelayEvent(&msg);
	}
}
void CSkinButton::SetUseTip(bool bshowTip)
{
	m_bshowTip=bshowTip ;
	if(m_bshowTip)
	{
		CRect rect;
		this->GetWindowRect(&rect);
		POINT point;
		::GetCursorPos(&point);
		if (::PtInRect( &rect, point ) )
		{
			this->UpdateWindow();
		}
	}
}
void CButtonDraw::DrawBmpShade(CDC* pDc, CRect& rcBmp)
{
	Gdiplus::Graphics graphics(pDc->m_hDC);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);
	Gdiplus::RectF rc1( rcBmp.right-1, rcBmp.top, 5, rcBmp.Height()+4 );
	Gdiplus::RectF rc2( rcBmp.left, rcBmp.bottom-1, rcBmp.Width()+4, 5);

	PointF point1(rcBmp.right,  rcBmp.top+1 );
	PointF point2(rcBmp.right+4,  rcBmp.top+4);
	PointF point3(rcBmp.right+4,  rcBmp.bottom);
	PointF point4(rcBmp.right,  rcBmp.bottom);

	PointF points[4] = {point1, point2, point3, point4};
	PointF* pPoints = points;

	LinearGradientBrush brushBK( rc1, m_colBK, m_colBK,LinearGradientModeHorizontal );
	graphics.FillPolygon(&brushBK, pPoints,4,FillModeWinding );

	LinearGradientBrush brush( rc1, m_colShadow, m_colShadow2,LinearGradientModeHorizontal );
	graphics.FillPolygon(&brush, pPoints,4,FillModeWinding );

	PointF point5(rcBmp.left+1,   rcBmp.bottom );
	PointF point6(rcBmp.right+4,  rcBmp.bottom);
	PointF point7(rcBmp.right+4,  rcBmp.bottom+3);
	PointF point8(rcBmp.left+4,    rcBmp.bottom+3);





	PointF points2[4] = {point5, point6, point7, point8};
	pPoints = points2;

	LinearGradientBrush brushBK2( rc2, m_colBK, m_colBK,LinearGradientModeVertical );
	graphics.FillPolygon(&brushBK2, pPoints,4,FillModeWinding );

	LinearGradientBrush   brush2( rc2, m_colShadow3, m_colShadow2,LinearGradientModeVertical );
	graphics.FillPolygon(&brush2, points2,4,FillModeWinding );
}

BOOL CSkinButton::SetPngSkin(CString str)
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

	return bReturn;
}
void CSkinButton::DrawImage(Gdiplus::Graphics& gdi)
{
	if( m_pPngImage && m_pPngImage->m_pBitmap )
	{
		CRect rect;
		this->GetClientRect(&rect);

		 
		
		RectF grect; grect.X=rect.right - m_pPngImage->m_pBitmap->GetWidth()-5, 
		grect.Y=2; grect.Width = m_pPngImage->m_pBitmap->GetWidth(); grect.Height = m_pPngImage->m_pBitmap->GetHeight();
		gdi.DrawImage(*m_pPngImage,grect );
	}
}