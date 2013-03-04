// CxSkinButton.cpp : implementation file
/** 15/03/2001 v1.00
 * ing.davide.pizzolato@libero.it
 ** 29/03/2001 v1.10
 * Milan.Gardian@LEIBINGER.com
 * - mouse tracking
 ** 02/04/2001 v1.20
 * ing.davide.pizzolato@libero.it
 * - new CreateRgnFromBitmap
 ** 14/04/2001 v1.21
 * - OnMouseLeave cast fixed
 * - Over bitmap consistency check
 ** 25/04/2001 v1.30
 * Fable@aramszu.net
 * - ExtCreateRegion replacement
 ** 24/06/2001 v1.40
 * - check & radio button add on
 * - added "focus" bitmap
 * - fixed CreateRgnFromBitmap bug
 * - fixed shortcut bug
 ** 27/10/2001 v1.41
 * - fixed memory leakage in CreateRgnFromBitmap
 */

#include "stdafx.h"
#include "xSkinButton.h"
#include ".\xskinbutton.h"
#include "CDataManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HRGN CxSkinButton::CreateRegionFromDib(CDibBitmap* dib , COLORREF color)
{
	CDC *pDC = GetDC();
	HBITMAP hBmp = dib->CreateBitmap(pDC);
	::ReleaseDC(m_hWnd, pDC->m_hDC);
	// get image properties
	BITMAP bmp = { 0 };
	GetObject( hBmp, sizeof(BITMAP), &bmp );
	// allocate memory for extended image information
	LPBITMAPINFO bi = (LPBITMAPINFO) new BYTE[ sizeof(BITMAPINFO) + 8 ];
	memset( bi, 0, sizeof(BITMAPINFO) + 8 );
	bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	// set window size
	//m_dwWidth	= bmp.bmWidth;		// bitmap width
	//m_dwHeight	= bmp.bmHeight;		// bitmap height
	// create temporary dc
	HDC dc = CreateIC( "DISPLAY",NULL,NULL,NULL );
	// get extended information about image (length, compression, length of color table if exist, ...)
	DWORD res = GetDIBits( dc, hBmp, 0, bmp.bmHeight, 0, bi, DIB_RGB_COLORS );
	// allocate memory for image data (colors)
	LPBYTE pBits = new BYTE[ bi->bmiHeader.biSizeImage + 4 ];
	// allocate memory for color table
	if ( bi->bmiHeader.biBitCount == 8 )
	{
		// actually color table should be appended to this header(BITMAPINFO),
		// so we have to reallocate and copy it
		LPBITMAPINFO old_bi = bi;
		// 255 - because there is one in BITMAPINFOHEADER
		bi = (LPBITMAPINFO)new char[ sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD) ];
		memcpy( bi, old_bi, sizeof(BITMAPINFO) );
		// release old header
		delete old_bi;
	}
	// get bitmap info header
	BITMAPINFOHEADER& bih = bi->bmiHeader;
	// get color table (for 256 color mode contains 256 entries of RGBQUAD(=DWORD))
	LPDWORD clr_tbl = (LPDWORD)&bi->bmiColors;
	// fill bits buffer
	res = GetDIBits( dc, hBmp, 0, bih.biHeight, pBits, bi, DIB_RGB_COLORS );
	DeleteDC( dc );

	BITMAP bm;
	GetObject( hBmp, sizeof(BITMAP), &bm );
	// shift bits and byte per pixel (for comparing colors)
	LPBYTE pClr = (LPBYTE)&color;
	// swap red and blue components
	BYTE tmp = pClr[0]; pClr[0] = pClr[2]; pClr[2] = tmp;
	// convert color if curent DC is 16-bit (5:6:5) or 15-bit (5:5:5)
	if ( bih.biBitCount == 16 )
	{
		// for 16 bit
		color = ((DWORD)(pClr[0] & 0xf8) >> 3) |
				((DWORD)(pClr[1] & 0xfc) << 3) |
				((DWORD)(pClr[2] & 0xf8) << 8);
		// for 15 bit
//		color = ((DWORD)(pClr[0] & 0xf8) >> 3) |
//				((DWORD)(pClr[1] & 0xf8) << 2) |
//				((DWORD)(pClr[2] & 0xf8) << 7);
	}

	const DWORD RGNDATAHEADER_SIZE	= sizeof(RGNDATAHEADER);
	const DWORD ADD_RECTS_COUNT		= 40;			// number of rects to be appended
													// to region data buffer

	// BitPerPixel
	BYTE	Bpp = bih.biBitCount >> 3;				// bytes per pixel
	// bytes per line in pBits is DWORD aligned and bmp.bmWidthBytes is WORD aligned
	// so, both of them not
	DWORD m_dwAlignedWidthBytes = (bmp.bmWidthBytes & ~0x3) + (!!(bmp.bmWidthBytes & 0x3) << 2);
	// DIB image is flipped that's why we scan it from the last line
	LPBYTE	pColor = pBits + (bih.biHeight - 1) * m_dwAlignedWidthBytes;
	DWORD	dwLineBackLen = m_dwAlignedWidthBytes + bih.biWidth * Bpp;	// offset of previous scan line
													// (after processing of current)
	DWORD	dwRectsCount = bih.biHeight;			// number of rects in allocated buffer
	INT		i, j;									// current position in mask image
	INT		first = 0;								// left position of current scan line
													// where mask was found
	bool	wasfirst = false;						// set when mask has been found in current scan line
	bool	ismask;									// set when current color is mask color

	// allocate memory for region data
	// region data here is set of regions that are rectangles with height 1 pixel (scan line)
	// that's why first allocation is <bm.biHeight> RECTs - number of scan lines in image
	RGNDATAHEADER* pRgnData = 
		(RGNDATAHEADER*)new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
	// get pointer to RECT table
	LPRECT pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
	// zero region data header memory (header  part only)
	memset( pRgnData, 0, RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) );
	// fill it by default
	pRgnData->dwSize	= RGNDATAHEADER_SIZE;
	pRgnData->iType		= RDH_RECTANGLES;

	for ( i = 0; i < bih.biHeight; i++ )
	{
		if(i == 35)
			int k = 0;
		for ( j = 0; j < bih.biWidth; j++ )
		{
			// get color
			switch ( bih.biBitCount )
			{
			case 8:
				ismask = (clr_tbl[ *pColor ] != color);
				break;
			case 16:
				ismask = (*(LPWORD)pColor != (WORD)color);
				break;
			case 24:
				ismask = ((*(LPDWORD)pColor & 0x00ffffff) != color);
				break;
			case 32:
				ismask = (*(LPDWORD)pColor != color);
			}
			// shift pointer to next color
			pColor += Bpp;
			// place part of scan line as RECT region if transparent color found after mask color or
			// mask color found at the end of mask image
			if ( wasfirst )
			{
				if ( !ismask )
				{
					// save current RECT
					pRects[ pRgnData->nCount++ ] = CRect( first, i, j, i + 1 );
					// if buffer full reallocate it with more room
					if ( pRgnData->nCount >= dwRectsCount )
					{
						dwRectsCount += ADD_RECTS_COUNT;
						// allocate new buffer
						LPBYTE pRgnDataNew = new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
						// copy current region data to it
						memcpy( pRgnDataNew, pRgnData, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT) );
						// delte old region data buffer
						delete []pRgnData;
						// set pointer to new regiondata buffer to current
						pRgnData = (RGNDATAHEADER*)pRgnDataNew;
						// correct pointer to RECT table
						pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
					}
					wasfirst = false;
				}
			}
			else if ( ismask )		// set wasfirst when mask is found
			{
				first = j;
				wasfirst = true;
			}
		}

		if ( wasfirst && ismask )
		{
			// save current RECT
			pRects[ pRgnData->nCount++ ] = CRect( first, i, j, i + 1 );
			// if buffer full reallocate it with more room
			if ( pRgnData->nCount >= dwRectsCount )
			{
				dwRectsCount += ADD_RECTS_COUNT;
				// allocate new buffer
				LPBYTE pRgnDataNew = new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
				// copy current region data to it
				memcpy( pRgnDataNew, pRgnData, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT) );
				// delte old region data buffer
				delete []pRgnData;
				// set pointer to new regiondata buffer to current
				pRgnData = (RGNDATAHEADER*)pRgnDataNew;
				// correct pointer to RECT table
				pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
			}
			wasfirst = false;
		}


		if(pColor - dwLineBackLen >= pBits)
			pColor -= dwLineBackLen;
	}

	for(int i = 0 ; i< (int)pRgnData->nCount; i++)
	{
	//	TRACE("\n count:%2d, [%2d, %2d, %2d,%2d]", i, pRects[i].left,  pRects[i].right,  pRects[i].top,  pRects[i].bottom);
	}
	// release image data
	delete []pBits;
	delete []bi;

	// create region
	HRGN hRgn = ExtCreateRegion( NULL, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT), (LPRGNDATA)pRgnData );
	// release region data
	delete []pRgnData;
	return hRgn;
}
/////////////////////////////////////////////////////////////////////////////
// CxSkinButton
CxSkinButton::CxSkinButton()
{
	m_DrawMode=1;			// normal drawing mode
	m_FocusRectMargin=0;	// disable focus dotted rect
	hClipRgn=NULL;			// no clipping region
	m_normalTextColor=GetSysColor(COLOR_BTNTEXT); // default button text color
	m_downTextColor	= m_normalTextColor;
	m_OverTextColor	= m_normalTextColor;
	m_disableTextColor = m_normalTextColor;
	m_button_down = m_tracking = m_Checked = false;

	m_bNormal = NULL;
	m_bDown = NULL;
	m_bOver = NULL;
	m_bDisabled = NULL;
	m_bMask = NULL;
	m_bFocus = NULL;

	m_bRClick = false;
	m_bSetMask = false;
	
	m_nTextOffsetX = 0;
	m_nTextOffsetY = 0;
	m_strToolTip = "";
	m_pToolTip = new CCWToolTips();


}
/////////////////////////////////////////////////////////////////////////////
CxSkinButton::~CxSkinButton()
{
	if (hClipRgn) DeleteObject(hClipRgn);	// free clip region

	delete m_pToolTip;

}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CxSkinButton, CButton)
	//{{AFX_MSG_MAP(CxSkinButton)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KILLFOCUS()
	//ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER,   OnMouseHover)
	ON_MESSAGE(WM_CXSHADE_RADIO , OnRadioInfo)
	ON_MESSAGE(BM_SETCHECK , OnBMSetCheck)
	ON_MESSAGE(BM_GETCHECK , OnBMGetCheck)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CxSkinButton message handlers
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::PreSubclassWindow()
{
	m_Style=GetButtonStyle();	///get specific BS_ styles
	if ((m_Style & BS_AUTOCHECKBOX)==BS_AUTOCHECKBOX)
//		||((m_Style & BS_CHECKBOX)==BS_CHECKBOX))
		m_Style=BS_CHECKBOX;
	else if ((m_Style & BS_AUTORADIOBUTTON)==BS_AUTORADIOBUTTON)
//			||((m_Style & BS_RADIOBUTTON)==BS_RADIOBUTTON))
		m_Style=BS_RADIOBUTTON;
	else { m_Style=BS_PUSHBUTTON; }

	CButton::PreSubclassWindow();
	ModifyStyle(0, BS_OWNERDRAW);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CxSkinButton::OnEraseBkgnd(CDC* pDC) 
{	return 1; }	// doesn't erase the button background
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if( m_strToolTip == "观看相关MV" )
	{
		int i = 0;
	}
	if(m_bMask != NULL)
		int i = 0;
    ASSERT (lpDrawItemStruct);
    //TRACE("* Captured: %08X\n", ::GetCapture());

    //Check if the button state in not in inconsistent mode...
    POINT mouse_position;
    if ((m_button_down) && (::GetCapture() == m_hWnd) && (::GetCursorPos(&mouse_position))){
		if (::WindowFromPoint(mouse_position) == m_hWnd){
			if ((GetState() & BST_PUSHED) != BST_PUSHED) {
				//TRACE("* Inconsistency up detected! Fixing.\n");
				SetState(TRUE);
				return;
			}
		} else {
			if ((GetState() & BST_PUSHED) == BST_PUSHED) {
				//TRACE("* Inconsistency up detected! Fixing.\n");
				SetState(FALSE);
				return;
			}
		}
	}
	
    //TRACE("* Drawing: %08x\n", lpDrawItemStruct->itemState);
	CString sCaption;
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);	// get device context
	CFont font;
	font.CreateFont(
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
			"宋体");                 // lpszFacename
	CFont *pOldFont = pDC->SelectObject(&font);

	RECT r=lpDrawItemStruct->rcItem;					// context rectangle
	int cx = r.right  - r.left ;						// get width
	int cy = r.bottom - r.top  ;						// get height
	// get text box position
	RECT tr={r.left+m_FocusRectMargin+2,r.top,r.right-m_FocusRectMargin-2,r.bottom};

	if(m_nTextOffsetX != 0 || m_nTextOffsetY != 0)
		tr = CRect(r.left + m_nTextOffsetX, r.top + m_nTextOffsetY, r.right, r.bottom);

	GetWindowText(sCaption);							// get button text
	pDC->SetBkMode(TRANSPARENT);

	COLORREF colorText = m_normalTextColor;
	// Select the correct skin 
	if (lpDrawItemStruct->itemState & ODS_DISABLED)
	{	// DISABLED BUTTON
		if(m_bDisabled==NULL)
			 // no skin selected for disabled state -> standard button
			pDC->FillSolidRect(&r,GetSysColor(COLOR_BTNFACE));
		else // paint the skin
			DrawBitmap(pDC,m_bDisabled,r,m_DrawMode);
		// if needed, draw the standard 3D rectangular border
		if (m_Border) pDC->DrawEdge(&r,EDGE_RAISED,BF_RECT);
		// paint the etched button text
//		pDC->SetTextColor(GetSysColor(COLOR_3DHILIGHT));
//		pDC->DrawText(sCaption,&tr,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		pDC->SetTextColor( m_disableTextColor );
		//OffsetRect(&tr,-1,-1);
		
		if(m_nTextOffsetX != 0 || m_nTextOffsetY != 0)
			pDC->DrawText(sCaption,&tr,DT_SINGLELINE|DT_TOP|DT_LEFT);
		else
			pDC->DrawText(sCaption,&tr,DT_SINGLELINE|DT_VCENTER|DT_CENTER);

	}
	else
	{										// SELECTED (DOWN) BUTTON
		if ((lpDrawItemStruct->itemState & ODS_SELECTED)||m_Checked)
		{
			if(m_bDown==NULL)
				 // no skin selected for selected state -> standard button
				pDC->FillSolidRect(&r,GetSysColor(COLOR_BTNFACE));
            else 
			{ // paint the skin
				DrawBitmap(pDC,m_bDown,r,m_DrawMode);
            }
			OffsetRect(&tr,1,1);  //shift text
			// if needed, draw the standard 3D rectangular border
			if (m_Border) pDC->DrawEdge(&r,EDGE_SUNKEN,BF_RECT);
			colorText = m_downTextColor;
		} 
		else
		{											// DEFAULT BUTTON
			if(m_bNormal==NULL)
				 // no skin selected for normal state -> standard button
				pDC->FillSolidRect(&r,GetSysColor(COLOR_BTNFACE));
			else // paint the skin
                if ((m_tracking)&&(m_bOver!=NULL))
				{
					DrawBitmap(pDC,m_bOver,r,m_DrawMode);
					colorText = m_OverTextColor;
				} 
				else
				{
					if ((lpDrawItemStruct->itemState & ODS_FOCUS)&&(m_bFocus!=NULL)){
						DrawBitmap(pDC,m_bFocus,r,m_DrawMode);
					} 
					else 
					{
						DrawBitmap(pDC,m_bNormal,r,m_DrawMode);
					}
					colorText = m_normalTextColor;
				}
			// if needed, draw the standard 3D rectangular border
			if (m_Border) pDC->DrawEdge(&r,EDGE_RAISED,BF_RECT);
		}
		// paint the focus rect
		if ((lpDrawItemStruct->itemState & ODS_FOCUS)&&(m_FocusRectMargin>0)){
			r.left   += m_FocusRectMargin ;
			r.top    += m_FocusRectMargin ;
			r.right  -= m_FocusRectMargin ;
			r.bottom -= m_FocusRectMargin ;
			DrawFocusRect (lpDrawItemStruct->hDC, &r) ;
		}
		// paint the enabled button text		
		pDC->SetTextColor(colorText);

		if(m_nTextOffsetX != 0 || m_nTextOffsetY != 0)
			pDC->DrawText(sCaption,&tr,DT_SINGLELINE|DT_TOP|DT_LEFT);
		else
			pDC->DrawText(sCaption,&tr,DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	}

	pDC->SelectObject(pOldFont);
}
/////////////////////////////////////////////////////////////////////////////
int CxSkinButton::GetBitmapWidth (CDibBitmap* hBitmap)
{
	return hBitmap->GetWidth();
	//return hBitmap->m_lpBMIH->biWidth;
}
/////////////////////////////////////////////////////////////////////////////
int CxSkinButton::GetBitmapHeight (CDibBitmap* hBitmap)
{
	return hBitmap->GetHeight();
	//return hBitmap->m_lpBMIH->biHeight;
}
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::DrawBitmap(CDC* dc, CDibBitmap* hbmp, RECT r, int DrawMode)
{
//	hbmp->SetCDibRect(r);
//	if(m_bSetMask)
//		hbmp->Draw(dc, true);
//	else
//		hbmp->Draw(dc, false);
////	hbmp->DrawTransParent(dc, r,r, RGB(0,0,0));
//	return;
//	DrawMode: 0=Normal; 1=stretch; 2=tiled fill

	if (m_bMask!=NULL){
		int i = 0;
	}
	if(DrawMode==2){
		FillWithBitmap(dc,hbmp,r);
		return;
	}
	if(!hbmp) return;	//safe check

	int cx=r.right  - r.left;
	int cy=r.bottom - r.top;

	CDC dcBmp,dcMask;
	dcBmp.CreateCompatibleDC(dc);
	int iSaveDC = dcBmp.SaveDC();

	CBitmap hBitmapBmp;                     //需要删除
	hBitmapBmp.CreateCompatibleBitmap(dc,hbmp->m_lpBMIH->biWidth,hbmp->m_lpBMIH->biHeight);
	CBitmap *pOldBitmap = dcBmp.SelectObject(&hBitmapBmp);
	hbmp->SetCDibRect(CRect(0,0,hbmp->m_lpBMIH->biWidth,hbmp->m_lpBMIH->biHeight));
	hbmp->Draw(&dcBmp,FALSE);
	
	if (m_bMask!=NULL && m_bMask->is_Loaded){
		dcMask.CreateCompatibleDC(dc);

		int iSaveMaskDC = dcMask.SaveDC();

		CBitmap hBitmapMask;//需要删除		 
		hBitmapMask.CreateCompatibleBitmap(dc,  m_bMask->m_lpBMIH->biWidth,m_bMask->m_lpBMIH->biHeight);
		dcMask.SelectObject(&hBitmapMask);
		m_bMask->SetCDibRect(CRect(0,0,m_bMask->m_lpBMIH->biWidth,m_bMask->m_lpBMIH->biHeight));
		m_bMask->Draw(&dcMask,FALSE);

		CDC hdcMem;
		hdcMem.CreateCompatibleDC(dc);

		int iSaveDC = hdcMem.SaveDC();

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

		hdcMem.RestoreDC( iSaveDC );

		hdcMem.DeleteDC();
		dcMask.RestoreDC( iSaveMaskDC );

		DeleteDC(dcMask);

		hBitmap.DeleteObject();
		hBitmapMask.DeleteObject();

	} else {
		if(!DrawMode){
			dc->BitBlt(r.left,r.top,cx,cy,&dcBmp,0,0,SRCCOPY);
		} else {
			int bx=GetBitmapWidth(hbmp);
			int by=GetBitmapHeight(hbmp);
			dc->StretchBlt(r.left,r.top,cx,cy,&dcBmp,0,0,bx,by,SRCCOPY);
		}
	}	

	dcBmp.SelectObject(pOldBitmap);
	dcBmp.RestoreDC( iSaveDC );

	DeleteDC(dcBmp);
	hBitmapBmp.DeleteObject();
}
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::FillWithBitmap(CDC* dc, CDibBitmap* hbmp, RECT r)
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
void CxSkinButton::SetSkin(CDibBitmap* normal,CDibBitmap* down, CDibBitmap* over,
				CDibBitmap* disabled, CDibBitmap* focus,CDibBitmap* mask,
				short drawmode,short border,short margin, bool bSetSizeBySkin)
{
	m_bNormal = normal;
	m_bDown = down;
	m_bOver = over;
	m_bFocus = focus;
	if( disabled == 0 )
		m_bDisabled = normal;
	else
		m_bDisabled = disabled;

	m_bMask = mask;

	m_DrawMode=max(0,min(drawmode,2));
	m_Border=border;
	m_FocusRectMargin=max(0,margin);
	
	if(this->m_hWnd == NULL)
		return;

	//if (m_bMask==0 && m_bSetMask){	
	//	m_bMask = normal;//hClipRgn = CreateRegionFromDib(normal,0);
	//}

	if (m_bMask!=0 && m_bMask->is_Loaded){		
		if (hClipRgn) DeleteObject(hClipRgn);

		hClipRgn = CreateRegionFromDib(m_bMask, RGB(255,255,255));//m_bMask->CreateRgnFromBitmap(RGB(255,255,255), this->GetDC());//CreateRgnFromBitmap(m_bMask,RGB(255,255,255));

		if (hClipRgn){
			SetWindowRgn(hClipRgn, TRUE);
			CDC* dc = GetDC();
			SelectClipRgn( dc->m_hDC ,hClipRgn);
			ReleaseDC( dc );
		}
		if (m_DrawMode==0){
		SetWindowPos(NULL,0,0,GetBitmapWidth(m_bMask),
					GetBitmapHeight(m_bMask),SWP_NOZORDER|SWP_NOMOVE);
		}
	}
	else
	{
		if (m_DrawMode==0 && bSetSizeBySkin){
			SetWindowPos(NULL,0,0,GetBitmapWidth(m_bNormal), GetBitmapHeight(m_bNormal),SWP_NOZORDER|SWP_NOMOVE);
		}
	}
	Invalidate(true);

}

/////////////////////////////////////////////////////////////////////////////
HRGN CxSkinButton::CreateRgnFromBitmap(CDibBitmap* hDib, COLORREF color)
{

	HBITMAP hBmp = hDib->CreateBitmap(this->GetDC());
	BITMAP bm;
	GetObject( hBmp, sizeof(BITMAP), &bm );	// get bitmap attributes

	CDC dcBmp;
	dcBmp.CreateCompatibleDC(GetDC());	//Creates a memory device context for the bitmap
	dcBmp.SelectObject(hBmp);			//selects the bitmap in the device context

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
	for ( i = 0; i < bm.bmHeight; i++ )
	for ( j = 0; j < bm.bmWidth; j++ ){
		// get color
		COLORREF pixcolor = dcBmp.GetPixel(j,bm.bmHeight-i-1);
		ismask=(pixcolor!=color);
		// place part of scan line as RECT region if transparent color found after mask color or
		// mask color found at the end of mask image
		if (wasfirst && ((ismask && (j==(bm.bmWidth-1)))||(ismask ^ (j<bm.bmWidth)))){
			// get offset to RECT array if RGNDATA buffer
			pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
			// save current RECT
			pRects[ pRgnData->nCount++ ] = CRect( first, bm.bmHeight - i - 1, j+(j==(bm.bmWidth-1)), bm.bmHeight - i );
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
	dcBmp.DeleteDC();	//release the bitmap
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
/////////////////////////////////////////////////////////////////////////////
COLORREF CxSkinButton::SetNormalTextColor(COLORREF normalcolor)
{
	COLORREF tmp_color=m_normalTextColor;
	m_normalTextColor=normalcolor;
	return tmp_color;			//r
}
COLORREF CxSkinButton::SetDownTextColor(COLORREF downColor)
{
	COLORREF tmp_color=m_downTextColor;
	m_downTextColor=downColor;
	return tmp_color;
}

COLORREF CxSkinButton::SetOverTextColor(COLORREF overColor)
{
	COLORREF tmp_color=m_OverTextColor;
	m_OverTextColor=overColor;
	return tmp_color;
}
COLORREF CxSkinButton::SetDisableTextColor(COLORREF disableColor)
{
	COLORREF tmp_color=m_disableTextColor;
	m_disableTextColor=disableColor;
	return tmp_color;
}

/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::SetToolTipText(CString s)
{
	if(m_strToolTip == "")
	{
		m_strToolTip = s;
		return;
	}
	m_strToolTip = s;
	if(m_tooltip.m_hWnd != NULL && ::IsWindow(m_tooltip.m_hWnd))
	{
		//m_tooltip.DestroyToolTipCtrl();
		//m_tooltip.m_hWnd = NULL;
	}
	if(m_tooltip.m_hWnd==NULL){
		if(m_tooltip.Create(this,TTS_ALWAYSTIP))	//first assignment
			if(m_tooltip.AddTool(this, (LPCTSTR)s))
				m_tooltip.Activate(1);
	} else {
		m_tooltip.UpdateTipText((LPCTSTR)s,this);
	}
}
CString CxSkinButton::GetToolTipText()
{
	return m_strToolTip;
}
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::RelayEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
// This function will create a MSG structure, fill it in a pass it to
// the ToolTip control, m_ttip.  Note that we ensure the point is in window
// coordinates (relative to the control's window).
	if(m_tooltip.m_hWnd == NULL && m_strToolTip != "")
		SetToolTipText(m_strToolTip);
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
		//::PostMessage(m_tooltip.m_hWnd, TTM_RELAYEVENT, 0, (LPARAM)&msg);
	}
}


/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnLButtonDblClk
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NONE
//Parameters..: Used only to be forwarded as WM_LBUTTONDOWN message parameters
//Exceptions..: NONE
//------------
//Description :
//
//  > We do not care about doublelicks - handle this event
//    like an ordinary left-button-down event
//
//---------------------------------------------------------
void CxSkinButton::OnLButtonDblClk(UINT flags, CPoint point) 
{
    SendMessage(WM_LBUTTONDOWN, flags, MAKELPARAM(point.x, point.y));
}


/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnLButtonDown
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NONE
//Parameters..: As follows
//    > [in] nFlags: not used
//    > [in] point: coordinates of the mouse pointer when this event was spawned
//Exceptions..: NONE
//------------
//Description :
//
//  > Handle event when left button is pressed down
//
//---------------------------------------------------------
void CxSkinButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	//Pass this message to the ToolTip control
	if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen())
		m_pToolTip->HideTooTips();

	RelayEvent(WM_LBUTTONDOWN,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));

    //If we are tracking this button, cancel it
    if (m_tracking) {
        TRACKMOUSEEVENT t = {
            sizeof(TRACKMOUSEEVENT),
            TME_CANCEL | TME_LEAVE,
            m_hWnd,
            0
        };
        if (::_TrackMouseEvent(&t))
            m_tracking = false;
    }
    //Default-process the message
	CButton::OnLButtonDown(nFlags, point);
    m_button_down = true;
}

void CxSkinButton::OnRButtonDown(UINT nFlags, CPoint point)
{
	CButton::OnRButtonDown(nFlags, point);
	if(m_bRClick)
		PostMessage(WM_LBUTTONDOWN, (WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));
}
/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnLButtonUp
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NONE
//Parameters..: As follows
//    > [in] nFlags: not used
//    > [in] point: coordinates of the mouse pointer when this event was spawned
//Exceptions..: NONE
//------------
//Description :
//
//  > Handle event when left button is released (goes up)
//
//---------------------------------------------------------
void CxSkinButton::OnLButtonUp(UINT nFlags, CPoint point)
{
    //TRACE("* %08X: up\n", ::GetTickCount());

	if (m_Style){ //track mouse for radio & check buttons
		POINT p2 = point;
		::ClientToScreen(m_hWnd, &p2);
		HWND mouse_wnd = ::WindowFromPoint(p2);
		if (mouse_wnd == m_hWnd){ // mouse is in button
			if (m_Style==BS_CHECKBOX) SetCheck(m_Checked ? 0 : 1);
			if (m_Style==BS_RADIOBUTTON) SetCheck(1);
		}
	}
	//Pass this message to the ToolTip control
	RelayEvent(WM_LBUTTONUP,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));

    //Default-process the message
    m_button_down = false;
	CButton::OnLButtonUp(nFlags, point);
}
void CxSkinButton::SetRightClick(BOOL bRightClick)
{
	this->m_bRClick = bRightClick;
}

void CxSkinButton::OnRButtonUp(UINT nFlags, CPoint point)
{
	CButton::OnRButtonUp(nFlags, point);
	if(m_bRClick)
		PostMessage(WM_LBUTTONUP, (WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));
}

/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnMouseMove
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NONE
//Parameters..: As follows
//    > [in] nFlags: not used
//    > [in] point: coordinates of the mouse pointer when this event was spawned
//Exceptions..: NONE
//------------
//Description :
//
//  > Handle change of mouse position: see the comments in the
//    method for further info.
//
//---------------------------------------------------------
void CxSkinButton::OnMouseMove(UINT nFlags, CPoint point)
{
    //TRACE("* %08X: Mouse\n", ::GetTickCount());

	//Pass this message to the ToolTip control
	if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen())
	{
		CPoint pt;
		::GetCursorPos(&pt);
		if( m_ptToolTipPos != pt )
		{
			m_ptToolTipPos = pt;
			m_pToolTip->HideTooTips();
			m_pToolTip->ShowToolTips( m_strToolTip, pt);
		}
	}else
	{
		RelayEvent(WM_MOUSEMOVE,(WPARAM)nFlags,MAKELPARAM(LOWORD(point.x),LOWORD(point.y)));
	}


    //If we are in capture mode, button has been pressed down
    //recently and not yet released - therefore check is we are
    //actually over the button or somewhere else. If the mouse
    //position changed considerably (e.g. we moved mouse pointer
    //from the button to some other place outside button area)
    //force the control to redraw
    //
    if ((m_button_down) && (::GetCapture() == m_hWnd)) {
	    POINT p2 = point;
        ::ClientToScreen(m_hWnd, &p2);
        HWND mouse_wnd = ::WindowFromPoint(p2);

        bool pressed = ((GetState() & BST_PUSHED) == BST_PUSHED);
        bool need_pressed = (mouse_wnd == m_hWnd);
        if (pressed != need_pressed) {
            //TRACE("* %08X Redraw\n", GetTickCount());
            SetState(need_pressed ? TRUE : FALSE);
            Invalidate();
        }
    } else {

	//Otherwise the button is released. That means we should
    //know when we leave its area - and so if we are not tracking
    //this mouse leave event yet, start now!
    //
        if (!m_tracking) {
            TRACKMOUSEEVENT t = {
                sizeof(TRACKMOUSEEVENT),
                TME_LEAVE|TME_HOVER,
                m_hWnd,
                0
            };
            if (::_TrackMouseEvent(&t)) {
                //TRACE("* Mouse enter\n");
                m_tracking = true;
                Invalidate();
            }
        }
    }

    //Forward this event to superclass
    CButton::OnMouseMove(nFlags, point);
}


/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnMouseLeave
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NULL
//Parameters..: NOT USED
//Exceptions..: NONE
//------------
//Description :
//
//  > Handle situation when mouse cursor leaves area of this
//    window (button). This event might be generated ONLY
//    if we explicitely call 'TrackMouseEvent'. This is
//    signalled by setting the m_tracking flag (see the assert
//    precondition) - in 'OnMouseMove' method
//
//  > When a mouse pointer leaves area of this button (i.e.
//    when this method is invoked), presumably the look of
//    the button changes (e.g. when hover/non-hover images are set)
//    and therefore we force the control to redraw.
//
//---------------------------------------------------------
LRESULT CxSkinButton::OnMouseLeave(WPARAM, LPARAM)
{
    //ASSERT (m_tracking);
    //TRACE("* Mouse leave\n");
	m_pToolTip->HideTooTips();

    m_tracking = false;
    Invalidate();
	::SendMessage( this->GetParent()->m_hWnd, MSG_MOUSELEAVE_BTN, (WPARAM)GetWindowLong(this->m_hWnd, GWL_ID),0 );
	return 0;
}

LRESULT   CxSkinButton::OnMouseHover(WPARAM   wParam,   LPARAM   lParam) 
{ 	
	m_tracking = true;
	::SendMessage( this->GetParent()->m_hWnd, MSG_MOUSEHOVER_BTN, (WPARAM)GetWindowLong(this->m_hWnd, GWL_ID),0 );
	return 0;
} 
/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnKillFocus
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: NONE
//Parameters..: See superclass documentation
//Exceptions..: NONE
//------------
//Description :
//
//  > If focus is killed during capture, we may no longer
//    have the exclusive access to user input and therefore
//    release it.
//
//  > Such a situation might happens when the user left-clicks
//    this button, keeps the button down and simultaneously
//    presses TAB key.
//
//---------------------------------------------------------
void CxSkinButton::OnKillFocus(CWnd *new_wnd)
{
    if (::GetCapture() == m_hWnd) {
        ::ReleaseCapture();
        ASSERT (!m_tracking);
        m_button_down = false;
    }
    CButton::OnKillFocus(new_wnd);
}


/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnClicked
//Class.......: CxSkinButton
//
//Author......: Milan Gardian
//Created.....: MAR-2001
//
//Return value: FALSE (do not stop in this handler - forward to parent)
//Parameters..: NONE
//Exceptions..: NONE
//------------
//Description :
//
//  > Keep consistency of attributes of this instance before
//    submitting click event to the parent.
//
//  > Currently NOT used. To use, umcomment line
//    "ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)" in message map
//    at the beginning of this file.
//
//---------------------------------------------------------
BOOL CxSkinButton::OnClicked() 
{
    if (::GetCapture() == m_hWnd) {
        ::ReleaseCapture();
        ASSERT (!m_tracking);
    }
    m_button_down = false;
    //Invalidate();
    return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
//
//Method......: OnRadioInfo
//Class.......: CxSkinButton
//
//Author......: Rainer Mangold
//Created.....: JUN-2001
//
//Return value: NULL
//Parameters..: WPARAM and LPARAM (LPARAM not used)
//Exceptions..: NONE
//------------
//Description :
//
//  > Handle notification, that a Button in the same group was pushed
//
//---------------------------------------------------------
LRESULT CxSkinButton::OnRadioInfo(WPARAM wparam, LPARAM)
{
	if (m_Checked){	//only checked buttons need to be unchecked
		m_Checked = false;
		Invalidate();
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CxSkinButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ((m_Style)&&(nChar==' ')){ //needed stuff for check & radio buttons
		if (m_Style==BS_CHECKBOX) SetCheck(m_Checked ? 0 : 1);
		if (m_Style==BS_RADIOBUTTON) SetCheck(1);
	}
	CButton::OnKeyDown(nChar, nRepCnt, nFlags);
}
/////////////////////////////////////////////////////////////////////////////
//
//Method......: SetCheck
//Class.......: CxSkinButton
//
//Author......: Rainer Mangold
//Created.....: JUN-2001
//
//Return value: NONE
//Parameters..: bool
//Exceptions..: NONE
//------------
//Description :
//
//  > Set the state of this button (pushed or not). 
//    Works for both, Radio and CheckBox - Buttons
//
//---------------------------------------------------------
LRESULT CxSkinButton::OnBMSetCheck(WPARAM wparam, LPARAM)
{
	m_Checked=wparam!=0;
	switch (m_Style)
	{
	case BS_RADIOBUTTON:
		if (m_Checked) { //uncheck the other radio buttons (in the same group)
			HWND hthis,hwnd2,hpwnd;
			hpwnd=GetParent()->GetSafeHwnd();	//get button parent handle
			hwnd2=hthis=GetSafeHwnd();			//get this button handle
			if (hthis && hpwnd){				//consistency check
				for( ; ; ){	//scan the buttons within the group
					hwnd2=::GetNextDlgGroupItem(hpwnd,hwnd2,0);
					//until we reach again this button
					if ((hwnd2==hthis)||(hwnd2==NULL)) break;
					//post the uncheck message
					::PostMessage(hwnd2, WM_CXSHADE_RADIO, 0, 0);
				}
			}
		}
		break;
	case BS_PUSHBUTTON:
		//m_Checked=false;
		//ASSERT(false); // Must be a Check or Radio button to use this function
		break;
	}

	Invalidate();
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CxSkinButton::OnBMGetCheck(WPARAM wparam, LPARAM)
{ return m_Checked; }	//returns the state for check & radio buttons
/////////////////////////////////////////////////////////////////////////////
//EOF

BOOL CxSkinButton::DestroyWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_pToolTip->DestroyWindow();

	return CButton::DestroyWindow();
}

BOOL CxSkinButton::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: 在此添加专用代码和/或调用基类
	m_pToolTip->m_pClientWnd = AfxGetMainWindow();
	m_pToolTip->m_hParent	 = m_hWnd;
	m_pToolTip->CreateEx(WS_EX_TOOLWINDOW|WS_EX_TOPMOST,TOOLTIPS_CLASS,"",0,CRect(0,0,0,0),NULL,0);	
	m_pToolTip->m_clrBK = RGB(255, 255, 225);

	return CButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);
}

