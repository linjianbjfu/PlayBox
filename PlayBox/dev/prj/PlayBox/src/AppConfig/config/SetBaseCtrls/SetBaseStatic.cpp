#include "stdafx.h"
#include "SetBaseStatic.h"

IMPLEMENT_DYNAMIC(SetBaseStatic, CStatic)
SetBaseStatic::SetBaseStatic()
{
	m_bTrans = false;
	m_FontSize = 12;
	m_TextColor = RGB(0,0,0);
	m_bBold = false;
}

SetBaseStatic::~SetBaseStatic()
{
}

BEGIN_MESSAGE_MAP(SetBaseStatic, CStatic)
	//ON_WM_CTLCOLOR_REFLECT()  
	//ON_WM_PAINT()
	//ON_MESSAGE(WM_SETTEXT,OnSetText)
END_MESSAGE_MAP()

void SetBaseStatic::OnPaint()
{
	if(m_bTrans)
	{
		CPaintDC dc(this);
		RECT rect;
		GetClientRect(&rect);			// get clip region
		dc.SetBkMode(TRANSPARENT);		// preserve background
		dc.SetTextColor(m_TextColor);

		CString strContent ;
		GetWindowText(strContent);

		LOGFONT  lf;
		ZeroMemory(&lf,sizeof(lf));

		lf.lfHeight = m_FontSize; /*-MulDiv(m_FontSize,GetDeviceCaps(dc.m_hDC,LOGPIXELSY),72);*/
		lf.lfWeight = (m_bBold)?FW_BOLD:FW_NORMAL;
		lf.lfWidth = 0;
		lf.lfCharSet = GB2312_CHARSET;
		lf.lfStrikeOut = FALSE;
		//	lf.lfCharSet= 0;
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfQuality = DEFAULT_QUALITY;
		lf.lfPitchAndFamily = FF_DONTCARE | DEFAULT_PITCH;
		lf.lfUnderline = false;
		strcpy(lf.lfFaceName, "ËÎÌו");

		CFont font;
		font.CreateFontIndirect(&lf);
		CFont* oldFont = (CFont*)dc.SelectObject(&font);


		::DrawText(
			dc.m_hDC,
			strContent,
			strContent.GetLength(),
			&rect,
			DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX/*|DT_EXTERNALLEADING|DT_WORDBREAK|DT_END_ELLIPSIS|DT_EDITCONTROL*/
			);

		//dc.SetTextColor(ltempColor);
		dc.SelectObject(oldFont);
		font.DeleteObject();
	}
	else
	{
		CStatic::OnPaint();
	}

}

LRESULT SetBaseStatic::OnSetText(WPARAM wParam,LPARAM lParam)
{
	LRESULT Result = Default();
	if(m_bTrans)
	{
		CRect Rect;
		GetWindowRect(&Rect);
		GetParent()->ScreenToClient(&Rect);
		GetParent()->InvalidateRect(&Rect);
		GetParent()->UpdateWindow();
	}
	return Result;
}

HBRUSH SetBaseStatic::CtlColor( CDC* pDC, UINT nCtlColor ) 
{
	if(m_bTrans)
	{
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	else
	{
		pDC->SetBkMode( TRANSPARENT );
		pDC->SetBkColor( RGB(255,255,255) );
		return ::CreateSolidBrush( RGB(255,255,255) );
	}
}

BOOL SetBaseStatic::OnEraseBkgnd( CDC* pDC )
{
	return TRUE;
}