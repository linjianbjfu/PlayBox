#include "StdAfx.h"
#include "dibbitmap.h"

CDibBitmap::CDibBitmap(void)
{
	m_rect = CRect(0,0,0,0);
	pos_Left = -1;
	pos_Top = -1;
	pos_Right = -1;
	pos_Bottom = -1;
	is_Loaded = 0;
	is_ChangeColor = 1;
	has_PopupError = false;
	m_count = 0;
}

CDibBitmap::~CDibBitmap(void)
{
}
BOOL CDibBitmap::LoadFromFile(CString path)
{
	m_path = path;
	return CDib::AttachMapFile(path,TRUE);
}
BOOL CDibBitmap::LoadFromFile(CString path,P_STRUCT_CUTRECT pCutRect)
{
	m_path = path;
	return CDib::AttachMapFile2(path,pCutRect,TRUE);
}
BOOL CDibBitmap::LoadFromMem(CString path,void* pVoid, UINT nSize, P_STRUCT_CUTRECT pCutRect)
{
	m_path = path;
	return CDib::AttachMapFile2(pVoid,nSize, pCutRect,TRUE);
}
BOOL CDibBitmap::LoadFromID(UINT ID)
{
	LPVOID lpv = (LPVOID) ::LoadResource(NULL,
        ::FindResource(NULL, MAKEINTRESOURCE(ID),
                       RT_BITMAP));
	return CDib::AttachMemory(lpv);
}

BOOL CDibBitmap::DrawStrechFromCenter(CDC *pDC)
{
	if(m_rect!= CRect(0,0,0,0))
	{
		CRect imgrc(0,0,m_lpBMIH->biWidth,m_lpBMIH->biHeight);
		GetShowRect(imgrc,m_rect);
		::StretchDIBits(pDC->GetSafeHdc(), m_rect.left,m_rect.top,m_rect.Width(),m_rect.Height(),
			imgrc.left, imgrc.top, imgrc.Width(), imgrc.Height(),	
			m_lpImage, (LPBITMAPINFO) m_lpBMIH, DIB_RGB_COLORS, SRCCOPY);
	}
	return TRUE;
}

void CDibBitmap::GetShowRect(CRect &Imgrc,const CRect Destrc)
{
	if(Destrc.Width()*Imgrc.Height() - Destrc.Height()*Imgrc.Width() > 0)
	{
		CRect temprc = Imgrc;
		int newheight;
		newheight = Imgrc.Width() * Destrc.Height()/Destrc.Width();
		Imgrc.DeflateRect(0,(temprc.Height() - newheight)/2,0,(temprc.Height() - newheight)/2);
	}
	else 
	{
		CRect temprc = Imgrc;
		int newwidth;
		newwidth = temprc.Height() * Destrc.Width() / Destrc.Height();
		Imgrc.DeflateRect((temprc.Width() - newwidth)/2,0,(temprc.Width() - newwidth)/2,0);
	}
}

BOOL CDibBitmap::Draw(CDC*pDC, int srcX , int srcY, int srcW, int srcH, int dstX, int dstY , int dstW, int dstH)
{
	if(!ValidCheck())
		return FALSE;
	if(m_lpBMIH == NULL) return FALSE;
	HPALETTE hOldPalette = NULL;
	if(m_hPalette != NULL) {
		hOldPalette = ::SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);
	}
	pDC->SetStretchBltMode(COLORONCOLOR);

	::StretchDIBits(pDC->GetSafeHdc(), srcX, srcY, srcW, srcH, dstX, m_lpBMIH->biHeight - dstY, dstW, dstH ,
		m_lpImage, (LPBITMAPINFO) m_lpBMIH, DIB_RGB_COLORS, SRCCOPY);
	if (hOldPalette != NULL)
	{
		::SelectPalette(pDC->GetSafeHdc(), hOldPalette, TRUE);
	}
	return TRUE;
}

BOOL CDibBitmap::ValidCheck()
{
	if(this == NULL)
		return false;
	if(!this->is_Loaded)
	{
#ifdef _DEBUG
		if(!has_PopupError)
			;//AfxMessageBox("Í¼Æ¬ " + this->m_strName  + " ÎÞÐ§");
		has_PopupError = true;
#endif
		return FALSE;
	}
	return TRUE;
}
BOOL CDibBitmap::Draw(CDC *pDC,BOOL isBKTransparent, COLORREF color, int mode)
{
	if(!ValidCheck())
		return FALSE;
	return CDib::Draw(pDC, CPoint(m_rect.left,m_rect.top), m_rect.Size(),isBKTransparent, color, (CDib::DrawMode)mode);
}
BOOL CDibBitmap::DrawImageList(CDC* pDC, int nIndex, POINT pt,int nCount)
{
	if( (m_count <= 0) || (nIndex>=m_count) || (nCount<1) )
		return FALSE;
	

	if(!ValidCheck())
		return FALSE;
	if(m_lpBMIH == NULL) return FALSE;
	if(m_hPalette != NULL) {
		::SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);
	}
	pDC->SetStretchBltMode(COLORONCOLOR);

	int nWidth;
	if( nCount == 1 )
	{
		nWidth =m_lpBMIH->biWidth/m_count;
	}
	else
	{
		nWidth = m_rect.Width()  / nCount;
	}
	int nHight = m_rect.Height();
	::StretchDIBits(pDC->GetSafeHdc(), pt.x, pt.y, nWidth,nHight, m_lpBMIH->biWidth/m_count*nIndex, 0, m_lpBMIH->biWidth/m_count, m_lpBMIH->biHeight ,
		m_lpImage, (LPBITMAPINFO) m_lpBMIH, DIB_RGB_COLORS, SRCCOPY);
	return TRUE;
}
void CDibBitmap::SetCDibRect(CRect rect)
{
	if(!ValidCheck())
		return;
	m_rect = rect;
}
void CDibBitmap::SetCount( int nCount)
{
	m_count = nCount;

}
HRGN CDibBitmap::CreateRgnFromBitmap(COLORREF color, CDC* pDC)
{
	if(!ValidCheck())
		return NULL;
	HBITMAP hBmp = CreateBitmap(pDC);
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

	for(int i = 0 ; i<(int)pRgnData->nCount; i++)
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
//HRGN CDibBitmap::CreateRgnFromBitmap(COLORREF color,CDC* dc)
//{
//	//if (!hBmp) return NULL;
//
//	//BITMAP bm;
//	//GetObject( hBmp, sizeof(BITMAP), &bm );	// get bitmap attributes
//
//	//CDC dcBmp;
//	//dcBmp.CreateCompatibleDC(GetDC());	//Creates a memory device context for the bitmap
//	//dcBmp.SelectObject(hBmp);			//selects the bitmap in the device context
//	int cx = this->m_lpBMIH->biWidth;
//	int cy = this->m_lpBMIH->biHeight;
//	CDC hdcMem;
//	hdcMem.CreateCompatibleDC(NULL);
//	CBitmap hBitmap;
//	hBitmap.CreateCompatibleBitmap(dc,cx,cy);
//	hdcMem.SelectObject(&hBitmap);
//	CRect rect(0,0,cx,cy);	
//	//this->Draw(&hdcMem,FALSE);
//	CDib::Draw(&hdcMem, CPoint(0,0), m_rect.Size(),FALSE);
//
//	const DWORD RDHDR = sizeof(RGNDATAHEADER);
//	const DWORD MAXBUF = 40;		// size of one block in RECTs
//									// (i.e. MAXBUF*sizeof(RECT) in bytes)
//	LPRECT	pRects;								
//	DWORD	cBlocks = 0;			// number of allocated blocks
//
//	INT		i, j;					// current position in mask image
//	INT		first = 0;				// left position of current scan line
//									// where mask was found
//	bool	wasfirst = false;		// set when if mask was found in current scan line
//	bool	ismask;					// set when current color is mask color
//
//	// allocate memory for region data
//	RGNDATAHEADER* pRgnData = (RGNDATAHEADER*)new BYTE[ RDHDR + ++cBlocks * MAXBUF * sizeof(RECT) ];
//	memset( pRgnData, 0, RDHDR + cBlocks * MAXBUF * sizeof(RECT) );
//	// fill it by default
//	pRgnData->dwSize	= RDHDR;
//	pRgnData->iType		= RDH_RECTANGLES;
//	pRgnData->nCount	= 0;
//	/*HRGN rgnResult = ::CreateRectRgn(0,0,0,0);*/
//	for ( i = 0; i < cy; i++ )
//	for ( j = 0; j < cx; j++ ){
//		// get color
//		COLORREF c =  hdcMem.GetPixel(j,cy-i-1);
//		ismask=(hdcMem.GetPixel(j,cy-i-1)!=color);
//		/*if(!ismask)
//		{
//			HRGN rgnTemp = ::CreateRectRgn(i,j,i+1,j+1);
//			::CombineRgn(rgnResult,rgnResult,rgnTemp,RGN_OR);
//			if (rgnTemp) DeleteObject(rgnTemp);
//		}*/
//		/*if(c==RGB(0x00,0x00,0x00))
//		{
//			c=0;
//		}
//		if(c==RGB(0xff,0x00,0x00))
//		{
//			c=RGB(0xff,0x00,0x00);
//		}*/
//		// place part of scan line as RECT region if transparent color found after mask color or
//		// mask color found at the end of mask image
//		if (wasfirst && ((ismask && (j==(cx-1)))||(ismask ^ (j<cx)))){
//			// get offset to RECT array if RGNDATA buffer
//			pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
//			// save current RECT
//			pRects[ pRgnData->nCount++ ] = CRect( first, cy - i - 1, j+(j==(cx-1)), cy - i );
//			// if buffer full reallocate it
//			if ( pRgnData->nCount >= cBlocks * MAXBUF ){
//				LPBYTE pRgnDataNew = new BYTE[ RDHDR + ++cBlocks * MAXBUF * sizeof(RECT) ];
//				memcpy( pRgnDataNew, pRgnData, RDHDR + (cBlocks - 1) * MAXBUF * sizeof(RECT) );
//				delete pRgnData;
//				pRgnData = (RGNDATAHEADER*)pRgnDataNew;
//			}
//			wasfirst = false;
//		} else if ( !wasfirst && ismask ){		// set wasfirst when mask is found
//			first = j;
//			wasfirst = true;
//		}
//	}
//	hdcMem.DeleteDC();	//release the bitmap
//	hBitmap.DeleteObject();
//	// create region
///*  Under WinNT the ExtCreateRegion returns NULL (by Fable@aramszu.net) */
////	HRGN hRgn = ExtCreateRegion( NULL, RDHDR + pRgnData->nCount * sizeof(RECT), (LPRGNDATA)pRgnData );
///* ExtCreateRegion replacement { */
//	HRGN hRgn=CreateRectRgn(0, 0, 0, 0);
//	ASSERT( hRgn!=NULL );
//	pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
//	for(i=0;i<(int)pRgnData->nCount;i++)
//	{
//		HRGN hr=CreateRectRgn(pRects[i].left, pRects[i].top, pRects[i].right, pRects[i].bottom);
//		VERIFY(CombineRgn(hRgn, hRgn, hr, RGN_OR)!=ERROR);
//		if (hr) DeleteObject(hr);
//	}
//	ASSERT( hRgn!=NULL );
///* } ExtCreateRegion replacement */
//
//	delete []pRgnData;
//	return hRgn;
//	//return rgnResult;
//}

int CDibBitmap::GetWidth()
{
	if(!ValidCheck())
		return 0;
	return this->m_lpBMIH->biWidth;
}
int CDibBitmap::GetHeight()
{
	if(!ValidCheck())
		return 0;
	return this->m_lpBMIH->biHeight;
}
CString CDibBitmap::GetPath()
{
	return m_path;
}



/////////////////////////////////////////////////////////////////////////////
HRGN CDibBitmap::CreateRgnFromBitmap(COLORREF color , CWnd * pWnd)
{
	return CreateRgnFromBitmap(color,pWnd->GetDC());
}

HBITMAP CDibBitmap::CreateBitmap(CDC* pDC)
{
	if(!ValidCheck())
		return NULL;
	return __super::CreateBitmap(pDC);
}
