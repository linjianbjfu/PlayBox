// xStaticText.cpp : implementation file
/* 15/03/2001
 * ing.davide.pizzolato@libero.it
 */
#include "stdafx.h"
#include "xStaticText.h"
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CxStaticText
CxStaticText::CxStaticText():m_strWindowText(""), m_EnableToolTip(false)
{
	m_ActualColor=m_TextColor=m_BackgroundColor=GetSysColor(COLOR_BTNTEXT); // default text color
	m_BlinkColor=GetSysColor(COLOR_3DLIGHT);		// default blink text color
	m_TimerID=0;
	m_BlinkCounter=-1;
	//m_Style=SST_NORMAL;
	mpBmpNormal = NULL;
	m_SingleLine = true;
	m_UnderLine = false;
	m_EnableClick = false;

	m_FontFace = "Arial";
	m_FontSize = 12;
	m_hCurFinger = ::LoadCursor(NULL, IDC_HAND);
}
/////////////////////////////////////////////////////////////////////////////
CxStaticText::~CxStaticText()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CxStaticText, CStatic)
	//{{AFX_MSG_MAP(CxStaticText)
	ON_WM_PAINT()
	ON_WM_ENABLE()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT(STN_CLICKED, OnStnClicked)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CxStaticText::OnPaint() 
{
	CPaintDC dc(this);				// device context for painting

	RECT rect;
	GetClientRect(&rect);			// get clip region
	dc.SetBkMode(TRANSPARENT);		// preserve background

	dc.FillSolidRect(&rect,m_BackgroundColor);	// update background color

	if (mpBmpNormal)				// draw background
	{
		DrawBitmap(&dc,mpBmpNormal,rect,m_DrawMode);
	}

	int nBufLen = MultiByteToWideChar(CP_ACP, 0, m_strWindowText, -1, NULL, 0);
	if(nBufLen <= 0)
		return;
	wchar_t * wszWndText = new wchar_t[nBufLen];
	if(wszWndText == NULL)
		return;
	if(MultiByteToWideChar(CP_ACP, 0, m_strWindowText, -1, wszWndText, nBufLen) == 0)
		return;

	dc.SetTextColor(m_TextColor);

	//COLORREF ltempColor = dc.GetTextColor();

	LOGFONT  lf;
	ZeroMemory(&lf,sizeof(lf));

	lf.lfHeight = m_FontSize; /*-MulDiv(m_FontSize,GetDeviceCaps(dc.m_hDC,LOGPIXELSY),72);*/
	lf.lfWeight = FW_NORMAL;
	lf.lfWidth = 0;
	lf.lfCharSet = GB2312_CHARSET;
	lf.lfStrikeOut = FALSE;
	//	lf.lfCharSet= 0;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = FF_DONTCARE | DEFAULT_PITCH;
	lf.lfUnderline = m_UnderLine;

	//TEXTMETRIC metric;

	//GetTextMetrics(dc,&metric);
	//
	//SetTextMetrics(dc,&metric);


	if( m_FontFace.GetLength() != 0)
	{
		strcpy(lf.lfFaceName, m_FontFace);
	}

	CFont font;
	font.CreateFontIndirect(&lf);
	CFont* oldFont = (CFont*)dc.SelectObject(&font);

	//CString sCaption;
	//GetWindowText(sCaption);		// get label text
	wstring wszPunctuation = L",.\'\"?!.，。“”？！)）】];；、：:";
	if ( m_SingleLine )
	{
		::DrawTextW(
			dc.m_hDC,
			wszWndText,
			wcslen(wszWndText),
			&rect,
			DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX/*|DT_EXTERNALLEADING|DT_WORDBREAK|DT_END_ELLIPSIS|DT_EDITCONTROL*/
			);
	}
	else
	{
		wistringstream iss(wszWndText);
		wstring strline;
		int out_line = 0;
		int line_spacing = 5;
		int wnd_width = rect.right - rect.left;
		int wnd_height = rect.bottom - rect.top;
		SIZE size;
		::GetTextExtentPointW(dc.m_hDC, wszWndText, wcslen(wszWndText), &size);
		int text_width = size.cx;
		int line_num = wnd_height / (size.cy + line_spacing);
		if(text_width > wnd_width)
		{
			while(getline(iss, strline, L'\n') && out_line <= line_num)
			{
				if(strline.size() == 0)
					continue;
				if(*(--strline.end()) == L'\r')
					strline.erase(--strline.end());
				while(strline.size())
				{
					int line_len = (int)(wcslen(wszWndText) * wnd_width / (float)text_width);//大概估算当前窗口的长度能容纳多少个字符
					if(line_len == 0)
						continue;
					wstring strOutput;
					int i = 0;
					if(line_len < strline.size())
					{
						strOutput = strline.substr(0, line_len);
					}
					else
					{
						strOutput = strline;
						line_len = strline.size();
					}
						::GetTextExtentPointW(dc.m_hDC, strOutput.c_str(), strOutput.size(), &size);
						if(size.cx > wnd_width)
						{
							line_len --;
							while(line_len > 0)
							{
								strOutput = strline.substr(0, line_len);
								::GetTextExtentPointW(dc.m_hDC, strOutput.c_str(), strOutput.size(), &size);
								if(size.cx > wnd_width)
									line_len --;
								else
									break;
							}
						}
						else if(size.cx < wnd_width)
						{
							line_len ++;
							while(line_len <= strline.size()) 
							{
								strOutput = strline.substr(0, line_len);
								::GetTextExtentPointW(dc.m_hDC, strOutput.c_str(), strOutput.size(), &size);								
								if(size.cx < wnd_width)
									line_len ++;
								else
									break;
							}
							line_len --;
						}
						//如果下一行的开始是标点符号，则减一个
						int temp = line_len;
						while(line_len < strline.size() && line_len >= 0 && wszPunctuation.find(strline[line_len]) != wstring::npos)
							line_len --;
						if(line_len == 0)
							line_len = temp;
						strOutput = strline.substr(0, line_len);
						strline = strline.substr(line_len);
						//如果是最后一行，则把最后三个字符替换为...
						if(out_line == line_num - 1 && !strline.empty())
						{
							strOutput.replace(strOutput.end() - 2, strOutput.end(), L"...");
						}
//					}
					//else
					//{
					//	strOutput = strline;
					//	strline = L"";
					//}
					RECT output_rect = {rect.left, rect.top + (size.cy + line_spacing) * out_line, rect.right,
						rect.top + (size.cy + line_spacing) * (out_line + 1)};
					::DrawTextW(
						dc.m_hDC,
						strOutput.c_str(),
						strOutput.size(),
						&output_rect,
						DT_WORDBREAK|DT_END_ELLIPSIS|DT_EDITCONTROL|DT_EXTERNALLEADING|DT_NOPREFIX
						);
					if(++out_line >= line_num)
						break;
				}
			}
		}
		else
		{
			::DrawTextW(
				dc.m_hDC,
				wszWndText,
				wcslen(wszWndText),
				&rect,
				DT_WORDBREAK|DT_END_ELLIPSIS|DT_EDITCONTROL|DT_EXTERNALLEADING|DT_NOPREFIX
				);
		}
	}

	//dc.SetTextColor(ltempColor);
	delete[] wszWndText;
	dc.SelectObject(oldFont);
	font.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
void CxStaticText::OnEnable(BOOL bEnable)
/* called by the framework when the object state changes */
{
	// the background must be drawn by the object owner
	InvalidateParent();
	Invalidate(0);	//update screen
}
/////////////////////////////////////////////////////////////////////////////
COLORREF CxStaticText::SetTextColor(COLORREF new_color)
/* Set active text color; ex: m_static1.SetTextColor(RGB(0,0,255)); */
{
	COLORREF tmp_color=m_TextColor;
	m_ActualColor=m_TextColor=new_color;
	Invalidate(0);				//update screen
	return tmp_color;			//returns the previous color
}
/////////////////////////////////////////////////////////////////////////////
COLORREF CxStaticText::SetBlinkColor(COLORREF new_color)
/* Set blink text color; ex: m_static1.SetBlinkColor(RGB(0,255,0)); */
{
	COLORREF tmp_color=m_BlinkColor;
	m_BlinkColor=new_color;
	return tmp_color;			//returns the previous color
}

COLORREF CxStaticText::SetBackgroundColor(COLORREF new_color)
{
	COLORREF tmp_color=m_BackgroundColor;
	m_BackgroundColor=new_color;
	return tmp_color;			//returns the previous color
}

/////////////////////////////////////////////////////////////////////////////
void CxStaticText::SetBlink(BOOL bBlink, int blink_time, int blink_count)
{
	if (blink_count<0){
		m_BlinkCounter=0;
	} else {
		m_BlinkCounter=2*blink_count;
	}

	if(bBlink){
		if (m_TimerID==0) m_TimerID=(UINT)(SetTimer(1, (UINT)blink_time,NULL));
	} else {
		if (m_TimerID!=0){
			KillTimer(m_TimerID);
			m_TimerID=0;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CxStaticText::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent==m_TimerID){
		if (m_BlinkCounter>0){
			m_BlinkCounter--;
			if (m_BlinkCounter==0){
				if (m_TimerID!=0){
					KillTimer(m_TimerID);
					m_TimerID=0;
				}
			}
		}
		if (m_ActualColor==m_TextColor)	m_ActualColor=m_BlinkColor;
		else	m_ActualColor=m_TextColor;
		Invalidate(0);
	}
	CStatic::OnTimer(nIDEvent);
}

//----------------------------------------------------------------
void CxStaticText::SetSingleLine(bool single)
{
	m_SingleLine = single;
}

//----------------------------------------------------------------

void CxStaticText::SetUnderLine(bool under)
{
	m_UnderLine = under;
}

//----------------------------------------------------------------
void CxStaticText::SetFontFace(const CString& face)
{
	m_FontFace = face;
}

//----------------------------------------------------------------
void CxStaticText::SetFontSize(unsigned int size)
{
	m_FontSize = size;
}

void CxStaticText::EnableClick(bool enable)
{
	m_EnableClick = enable;
}
//----------------------------------------------------------------

void CxStaticText::InvalidateParent()
{
	CWnd* pParent=GetParent();
	if (pParent){
		RECT rect;
		GetWindowRect(&rect);
		pParent->ScreenToClient(&rect);
		pParent->InvalidateRect(&rect,0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CxStaticText::SetWindowText(LPCTSTR lpszString)
{
	//CStatic::SetWindowText(lpszString);
	m_strWindowText = lpszString;
	Invalidate(0);

	if (m_EnableToolTip)
	{
		SetToolTipText(lpszString);
	}

}
/////////////////////////////////////////////////////////////////////////////
void CxStaticText::SetSkin(CDibBitmap* abmpNormal ,CDibBitmap* abmpMask, short aiDrawmode)
{
	mpBmpNormal = abmpNormal;
	mpBmpMask = abmpMask;
	m_DrawMode=max(0,min(aiDrawmode,2)); 
	if (abmpMask!=0){		
		if (hClipRgn) DeleteObject(hClipRgn);
		hClipRgn = CreateRgnFromBitmap(abmpMask,RGB(255,255,255));
		if (hClipRgn){
			SetWindowRgn(hClipRgn, TRUE);
			SelectClipRgn((HDC)GetDC(),hClipRgn);
		}
		if (m_DrawMode==0){
			SetWindowPos(NULL,0,0,GetBitmapWidth(abmpMask),
						GetBitmapHeight(abmpMask),SWP_NOZORDER|SWP_NOMOVE);
		}
	}
}
HRGN CxStaticText::CreateRgnFromBitmap(CDibBitmap* hBmp, COLORREF color)
{
	int cx = hBmp->m_lpBMIH->biWidth;
	int cy = hBmp->m_lpBMIH->biHeight;
	CDC hdcMem;
	hdcMem.CreateCompatibleDC(NULL);
	CBitmap hBitmap;
	hBitmap.CreateCompatibleBitmap(&hdcMem,cx,cy);//此处dc的设置可能有问题
	hdcMem.SelectObject(&hBitmap);
	CRect rect(0,0,cx,cy);	
	//this->Draw(&hdcMem,FALSE);
	//CDib::Draw(&hdcMem, CPoint(0,0), CSize(cx,cy),FALSE);	
	hBmp->Draw(&hdcMem,FALSE);

	const DWORD RDHDR = sizeof(RGNDATAHEADER);
	const DWORD MAXBUF = 40;		// size of one block in RECTs
									// (i.e. MAXBUF*sizeof(RECT) in bytes)
	LPRECT	pRects;								
	DWORD	cBlocks = 0;			// number of allocated blocks

	INT		i, j;					// current position in mask image
	INT		first = 0;				// left position of current scan line
									// where mask was found
	bool	wasfirst = false;		// set when if mask was found in current scan line
	bool	ismask;					// set when current color is mask color

	// allocate memory for region data
	RGNDATAHEADER* pRgnData = (RGNDATAHEADER*)new BYTE[ RDHDR + ++cBlocks * MAXBUF * sizeof(RECT) ];
	memset( pRgnData, 0, RDHDR + cBlocks * MAXBUF * sizeof(RECT) );
	// fill it by default
	pRgnData->dwSize	= RDHDR;
	pRgnData->iType		= RDH_RECTANGLES;
	pRgnData->nCount	= 0;
	for ( i = 0; i < cy; i++ )
	for ( j = 0; j < cx; j++ ){
		// get color
		COLORREF c =  hdcMem.GetPixel(j,cy-i-1);
		ismask=(hdcMem.GetPixel(j,cy-i-1)!=color);
		/*if(c)
		{
			c=0;
		}*/
		// place part of scan line as RECT region if transparent color found after mask color or
		// mask color found at the end of mask image
		if (wasfirst && ((ismask && (j==(cx-1)))||(ismask ^ (j<cx)))){
			// get offset to RECT array if RGNDATA buffer
			pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
			// save current RECT
			pRects[ pRgnData->nCount++ ] = CRect( first, cy - i - 1, j+(j==(cx-1)), cy - i );
			// if buffer full reallocate it
			if ( pRgnData->nCount >= cBlocks * MAXBUF ){
				LPBYTE pRgnDataNew = new BYTE[ RDHDR + ++cBlocks * MAXBUF * sizeof(RECT) ];
				memcpy( pRgnDataNew, pRgnData, RDHDR + (cBlocks - 1) * MAXBUF * sizeof(RECT) );
				delete pRgnData;
				pRgnData = (RGNDATAHEADER*)pRgnDataNew;
			}
			wasfirst = false;
		} else if ( !wasfirst && ismask ){		// set wasfirst when mask is found
			first = j;
			wasfirst = true;
		}
	}

	hBitmap.DeleteObject();
	hdcMem.DeleteDC();	//release the bitmap
	// create region
/*  Under WinNT the ExtCreateRegion returns NULL (by Fable@aramszu.net) */
//	HRGN hRgn = ExtCreateRegion( NULL, RDHDR + pRgnData->nCount * sizeof(RECT), (LPRGNDATA)pRgnData );
/* ExtCreateRegion replacement { */
	HRGN hRgn=CreateRectRgn(0, 0, 0, 0);
	ASSERT( hRgn!=NULL );
	pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
	for(i=0;i<(int)pRgnData->nCount;i++)
	{
		HRGN hr=CreateRectRgn(pRects[i].left, pRects[i].top, pRects[i].right, pRects[i].bottom);
		VERIFY(CombineRgn(hRgn, hRgn, hr, RGN_OR)!=ERROR);
		if (hr) DeleteObject(hr);
	}
	ASSERT( hRgn!=NULL );
/* } ExtCreateRegion replacement */

	delete pRgnData;
	return hRgn;
}
void CxStaticText::DrawBitmap(CDC* dc, CDibBitmap* hbmp, RECT r, int DrawMode)
{
//	DrawMode: 0=Normal; 1=stretch; 2=tiled fill
	if(DrawMode==2){
		FillWithBitmap(dc,hbmp,r);
		return;
	}
	if(!hbmp) return;	//safe check

	int cx=r.right  - r.left;
	int cy=r.bottom - r.top;
	CDC dcBmp,dcMask;
	dcBmp.CreateCompatibleDC(dc);
	CBitmap hBitmapBmp;                     //需要删除
	hBitmapBmp.CreateCompatibleBitmap(dc,hbmp->m_lpBMIH->biWidth,hbmp->m_lpBMIH->biHeight);
	dcBmp.SelectObject(&hBitmapBmp);
	hbmp->SetCDibRect(CRect(0,0,hbmp->m_lpBMIH->biWidth,hbmp->m_lpBMIH->biHeight));
	hbmp->Draw(&dcBmp,FALSE);
	
	if (mpBmpMask!=NULL){

		dcMask.CreateCompatibleDC(dc);
		CBitmap hBitmapMask;//需要删除		 
		hBitmapMask.CreateCompatibleBitmap(dc,mpBmpMask->m_lpBMIH->biWidth,mpBmpMask->m_lpBMIH->biHeight);
		dcMask.SelectObject(&hBitmapMask);
		mpBmpMask->SetCDibRect(CRect(0,0,mpBmpMask->m_lpBMIH->biWidth,mpBmpMask->m_lpBMIH->biHeight));
		mpBmpMask->Draw(&dcMask,FALSE);

		CDC hdcMem;
		hdcMem.CreateCompatibleDC(dc);
	    CBitmap hBitmap;
		hBitmap.CreateCompatibleBitmap(dc,cx,cy);
	    hdcMem.SelectObject(hBitmap);		
        hdcMem.BitBlt(r.left,r.top,cx,cy,dc,0,0,SRCCOPY);
		if(!DrawMode){
			hdcMem.BitBlt(r.left,r.top,cx,cy,&dcBmp,0,0,SRCINVERT);
			hdcMem.BitBlt(r.left,r.top,cx,cy,&dcMask,0,0,SRCAND);
			hdcMem.BitBlt(r.left,r.top,cx,cy,&dcBmp,0,0,SRCINVERT);
		} else {
			int bx=GetBitmapWidth(hbmp);
			int by=GetBitmapHeight(hbmp);
			hdcMem.StretchBlt(r.left,r.top,cx,cy,&dcBmp,0,0,bx,by,SRCINVERT);
			hdcMem.StretchBlt(r.left,r.top,cx,cy,&dcMask,0,0,bx,by,SRCAND);
			hdcMem.StretchBlt(r.left,r.top,cx,cy,&dcBmp,0,0,bx,by,SRCINVERT);
		}
        dc->BitBlt(r.left,r.top,cx,cy,&hdcMem,0,0,SRCCOPY);


		hBitmap.DeleteObject();
		hBitmapMask.DeleteObject();
		hdcMem.DeleteDC();
		DeleteDC(dcMask);
	} else {
		if(!DrawMode){
			dc->BitBlt(r.left,r.top,cx,cy,&dcBmp,0,0,SRCCOPY);
		} else {
			int bx=GetBitmapWidth(hbmp);
			int by=GetBitmapHeight(hbmp);
			dc->StretchBlt(r.left,r.top,cx,cy,&dcBmp,0,0,bx,by,SRCCOPY);
		}
	}
	hBitmapBmp.DeleteObject();
	DeleteDC(dcBmp);
}
int CxStaticText::GetBitmapWidth (CDibBitmap* hBitmap)
{
	return hBitmap->m_lpBMIH->biWidth;
}
/////////////////////////////////////////////////////////////////////////////
int CxStaticText::GetBitmapHeight (CDibBitmap* hBitmap)
{
	return hBitmap->m_lpBMIH->biHeight;
}
void CxStaticText::FillWithBitmap(CDC* dc, CDibBitmap* hbmp, RECT r)
{
	if(!hbmp) return;
	CDC memdc;
	memdc.CreateCompatibleDC(dc);
	CBitmap hBitmapBmp;                     //需要删除
	hBitmapBmp.CreateCompatibleBitmap(dc,hbmp->m_lpBMIH->biWidth,hbmp->m_lpBMIH->biHeight);
	memdc.SelectObject(&hBitmapBmp);
	hbmp->SetCDibRect(CRect(0,0,hbmp->m_lpBMIH->biWidth,hbmp->m_lpBMIH->biHeight));
	hbmp->Draw(&memdc,FALSE);

	int w = r.right - r.left;
	int	h = r.bottom - r.top;
	int x,y,z;
	int	bx=GetBitmapWidth(hbmp);
	int	by=GetBitmapHeight(hbmp);
	for (y = r.top ; y < h ; y += by){
		if ((y+by)>h) by=h-y;
		z=bx;
		for (x = r.left ; x < w ; x += z){
			if ((x+z)>w) z=w-x;
			dc->BitBlt(x, y, z, by, &memdc, 0, 0, SRCCOPY);
		}
	}
	hBitmapBmp.DeleteObject();
	DeleteDC(memdc);
}

void CxStaticText::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if(m_EnableClick)
	{	
		::SetCursor(m_hCurFinger);
	}

	if(m_EnableToolTip)
	{
		RelayEvent(WM_MOUSEMOVE,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));
	}

	CStatic::OnLButtonDown(nFlags, point);
}

void CxStaticText::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	ModifyStyle(0,SS_NOTIFY);

	CStatic::PreSubclassWindow();
}


CRect CxStaticText::GetSingleLineTextRect()
{
	CPaintDC dc(this);

	CRect rc(0,0,0,0);

	this->GetWindowRect(&rc);
	LOGFONT  lf;
	ZeroMemory(&lf,sizeof(lf));
	lf.lfHeight = m_FontSize; /*-MulDiv(m_FontSize,GetDeviceCaps(dc.m_hDC,LOGPIXELSY),72);*/
	lf.lfWeight = FW_NORMAL;
	lf.lfWidth = 0;
	lf.lfCharSet = GB2312_CHARSET;
	lf.lfStrikeOut = FALSE;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = FF_DONTCARE | DEFAULT_PITCH;
	lf.lfUnderline = m_UnderLine;
	if( m_FontFace.GetLength() != 0)
	{
		strcpy(lf.lfFaceName, m_FontFace);
	}

	CFont font;
	font.CreateFontIndirect(&lf);
	CFont* oldFont = (CFont*)dc.SelectObject(&font);
	dc.DrawText(m_strWindowText + " ", &rc, DT_CALCRECT);
	dc.SelectObject(oldFont);
	font.DeleteObject();
	return rc;
}

void CxStaticText::SizeToTextRect()
{
	CRect rect = GetSingleLineTextRect();
	GetParent()->ScreenToClient(&rect);
	MoveWindow(&rect);

	Invalidate(TRUE);
}

void CxStaticText::OnStnClicked()
{
	if (m_EnableClick)
	{		
		long id = GetWindowLong(m_hWnd, GWL_ID);

		CWnd* pWnd = GetParent();

		pWnd->SendMessage(WM_STATICCLICKED, (WPARAM)id, NULL);
	}
}


/////////////////////////////////////////////////////////////////////////////
void CxStaticText::SetToolTipText(CString s)
{
	m_strToolTip = s;
	if(m_tooltip.m_hWnd==NULL){
		if(m_tooltip.Create(this))	//first assignment
			if(m_tooltip.AddTool(this, (LPCTSTR)s))
				m_tooltip.Activate(1);
	} else {
		m_tooltip.UpdateTipText((LPCTSTR)s,this);
	}
}

void CxStaticText::RelayEvent(UINT message, WPARAM wParam, LPARAM lParam)
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

void CxStaticText::EnableToolTip(bool enable)
{
	m_EnableToolTip = enable;
}


//void CxStaticText::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	//RelayEvent(WM_LBUTTONDOWN,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));
//
//	CStatic::OnLButtonDown(nFlags, point);
//}

//void CxStaticText::OnLButtonUp(UINT nFlags, CPoint point)
//{
//	//RelayEvent(WM_LBUTTONUP,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));
//
//	CStatic::OnLButtonUp(nFlags, point);
//}
