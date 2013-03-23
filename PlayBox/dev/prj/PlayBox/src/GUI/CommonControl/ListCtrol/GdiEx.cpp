#include "StdAfx.h"
#include "GdiEx.h"
#include "wingdi.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CGdiEx::CGdiEx(void)
{
}

CGdiEx::~CGdiEx(void)
{
}
inline float CGdiEx::ToRGB1(FLOAT afValue1, FLOAT afValue2, FLOAT afValue3)
{
	if (afValue3 > 360.0f)
		afValue3 -= 360.0f;
	else if (afValue3 < 0.0f)
		afValue3 += 360.0f;

	if (afValue3 < 60.0f)
		afValue1 = afValue1 + (afValue2 - afValue1) * afValue3 / 60.0f;
	else if (afValue3 < 180.0f)
		afValue1 = afValue2;
	else if (afValue3 < 240.0f)
		afValue1 = afValue1 + (afValue2 - afValue1) * (240.0f - afValue3) / 60.0f;

	return afValue1 * 255;
}
void CGdiEx::SetRGB(COLORREF &aSourceColor, FLOAT afHue, FLOAT afBrightness, FLOAT afSaturation)
{
	FLOAT lfRed = GetRValue(aSourceColor);
	FLOAT lfGreen = GetGValue(aSourceColor);
	FLOAT lfBlue = GetBValue(aSourceColor);

	if (afSaturation == 0.0f)
	{
		lfRed = lfGreen = lfBlue = afBrightness * 255.0f;
	}
	else
	{
		FLOAT lfValue1, lfValue2;

		if (afBrightness <= 0.5f)
			lfValue2 = afBrightness + afBrightness * afSaturation;
		else
			lfValue2 = afBrightness + afSaturation - afBrightness * afSaturation;

		lfValue1 = 2.0f * afBrightness - lfValue2;
		lfRed = ToRGB1(lfValue1, lfValue2, afHue + 120.0f);
		lfGreen = ToRGB1(lfValue1, lfValue2, afHue);
		lfBlue = ToRGB1(lfValue1, lfValue2, afHue - 120.0f);
	}

	aSourceColor = RGB(lfRed, lfGreen, lfBlue);
}
void CGdiEx::SetHBS(COLORREF aSourceColor, FLOAT &afHue, FLOAT &afBrightness, FLOAT &afSaturation)
{
	FLOAT lfRed   = GetRValue(aSourceColor);
	FLOAT lfGreen = GetGValue(aSourceColor);
	FLOAT lfBlue  = GetBValue(aSourceColor);

	FLOAT lfMaxValue = max(lfRed, max(lfGreen, lfBlue));
	FLOAT lfMinValue = min(lfRed, min(lfGreen, lfBlue));

	FLOAT lfDiff, lfSum, lfRedTarget, lfGreenTarget, lfBlueTarget;

	lfDiff = lfMaxValue - lfMinValue;
	lfSum  = lfMaxValue + lfMinValue;
	afBrightness = lfSum / 510.0f;

	if (lfMaxValue == lfMinValue)
	{
		afSaturation = 0.0f;
		afHue = 0.0f;
	}
	else
	{
		lfRedTarget = (lfMaxValue - lfRed) / lfDiff;
		lfGreenTarget = (lfMaxValue - lfGreen) / lfDiff;
		lfBlueTarget = (lfMaxValue - lfBlue) / lfDiff;

		if (afBrightness <= 0.5f)
			afSaturation = lfDiff / lfSum;
		else
			afSaturation = lfDiff / (510.0f - lfSum);

		if (lfRed == lfMaxValue)
			afHue = 60.0f * (6.0f + lfBlueTarget - lfGreenTarget);
		if (lfGreen == lfMaxValue)
			afHue = 60.0f * (2.0f + lfRedTarget - lfBlueTarget);
		if (lfBlue == lfMaxValue)
			afHue = 60.0f * (4.0f + lfGreenTarget - lfRedTarget);
		if (afHue > 360.0f)
			afHue = afHue - 360.0f;
	}
}
#define   MAXNUM   4096   
HRGN CGdiEx::CreateRgnFromBitmap(HBITMAP hBmp, COLORREF cTransparentColor , COLORREF cTolerance)
{
	/*if (!hBmp) return NULL;



	BITMAP bm;
	GetObject( hBmp, sizeof(BITMAP), &bm );	

	CDC dcBmp;
	CDC* lpdc = CWnd::FromHandle(handle)->GetDC();
	dcBmp.CreateCompatibleDC(lpdc );
	dcBmp.SelectObject(hBmp);			

	const DWORD RDHDR = sizeof(RGNDATAHEADER);
	const DWORD MAXBUF = 40;		
	LPRECT	pRects;								
	DWORD	cBlocks = 0;		

	INT		i, j;
	INT		first = 0;		
	bool	wasfirst = false;	
	bool	ismask;		
	RGNDATAHEADER* pRgnData = (RGNDATAHEADER*)new BYTE[ RDHDR + ++cBlocks * MAXBUF * sizeof(RECT) ];
	memset( pRgnData, 0, RDHDR + cBlocks * MAXBUF * sizeof(RECT) );
	pRgnData->dwSize	= RDHDR;
	pRgnData->iType		= RDH_RECTANGLES;
	pRgnData->nCount	= 0;
	for ( i = 0; i < bm.bmHeight; i++ )
	for ( j = 0; j < bm.bmWidth; j++ ){
	ismask=(dcBmp.GetPixel(j,bm.bmHeight-i-1)!=color);
	if (wasfirst && ((ismask && (j==(bm.bmWidth-1)))||(ismask ^ (j<bm.bmWidth)))){
	pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
	pRects[ pRgnData->nCount++ ] = CRect( first, bm.bmHeight - i - 1, j+(j==(bm.bmWidth-1)), bm.bmHeight - i );
	if ( pRgnData->nCount >= cBlocks * MAXBUF ){
	LPBYTE pRgnDataNew = new BYTE[ RDHDR + ++cBlocks * MAXBUF * sizeof(RECT) ];
	memcpy( pRgnDataNew, pRgnData, RDHDR + (cBlocks - 1) * MAXBUF * sizeof(RECT) );
	delete pRgnData;
	pRgnData = (RGNDATAHEADER*)pRgnDataNew;
	}
	wasfirst = false;
	} else if ( !wasfirst && ismask ){
	first = j;
	wasfirst = true;
	}
	}
	dcBmp.DeleteDC();	
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
	ReleaseDC(handle,lpdc->m_hDC);
	delete pRgnData;
	return hRgn;*/


	 HRGN hRgn = NULL;

    if (hBmp)
   {
        // Create a memory DC inside which we will scan the bitmap content
        HDC hMemDC = CreateCompatibleDC(NULL);
        if (hMemDC)
        {
            // Get bitmap size
            BITMAP bm;
            GetObject(hBmp, sizeof(bm), &bm);

            // Create a 32 bits depth bitmap and select it into the memory DC 
            BITMAPINFOHEADER RGB32BITSBITMAPINFO = {    
                    sizeof(BITMAPINFOHEADER),    // biSize 
                    bm.bmWidth,                    // biWidth; 
                    bm.bmHeight,                // biHeight; 
                    1,                            // biPlanes; 
                    32,                            // biBitCount 
                    BI_RGB,                        // biCompression; 
                    0,                            // biSizeImage; 
                    0,                            // biXPelsPerMeter; 
                    0,                            // biYPelsPerMeter; 
                    0,                            // biClrUsed; 
                    0                            // biClrImportant; 
            };
            VOID * pbits32; 
            HBITMAP hbm32 = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
            if (hbm32)
            {
                HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);

                // Create a DC just to copy the bitmap into the memory DC
                HDC hDC = CreateCompatibleDC(hMemDC);
                if (hDC)
                {
                    // Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
                    BITMAP bm32;
                    GetObject(hbm32, sizeof(bm32), &bm32);
                    while (bm32.bmWidthBytes % 4)
                        bm32.bmWidthBytes++;

                    // Copy the bitmap into the memory DC
                    HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);
                    BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

                    // For better performances, we will use the ExtCreateRegion() function to create the
                    // region. This function take a RGNDATA structure on entry. We will add rectangles by
                    // amount of ALLOC_UNIT number in this structure.
                    #define ALLOC_UNIT    100
                    DWORD maxRects = ALLOC_UNIT;
                    HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
                    RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
                    pData->rdh.dwSize = sizeof(RGNDATAHEADER);
                    pData->rdh.iType = RDH_RECTANGLES;
                    pData->rdh.nCount = pData->rdh.nRgnSize = 0;
                    SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

                    // Keep on hand highest and lowest values for the "transparent" pixels
                    BYTE lr = GetRValue(cTransparentColor);
                    BYTE lg = GetGValue(cTransparentColor);
                    BYTE lb = GetBValue(cTransparentColor);
                    BYTE hr = min(0xff, lr + GetRValue(cTolerance));
                    BYTE hg = min(0xff, lg + GetGValue(cTolerance));
                    BYTE hb = min(0xff, lb + GetBValue(cTolerance));

                    // Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
                    BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
                    for (int y = 0; y < bm.bmHeight; y++)
                    {
                        // Scan each bitmap pixel from left to right
                        for (int x = 0; x < bm.bmWidth; x++)
                        {
                            // Search for a continuous range of "non transparent pixels"
                            int x0 = x;
                            LONG *p = (LONG *)p32 + x;
                            while (x < bm.bmWidth)
                            {
                                BYTE b = GetRValue(*p);
                                if (b >= lr && b <= hr)
                                {
                                    b = GetGValue(*p);
                                    if (b >= lg && b <= hg)
                                    {
                                        b = GetBValue(*p);
                                        if (b >= lb && b <= hb)
                                            // This pixel is "transparent"
                                            break;
                                    }
                                }
                                p++;
                                x++;
                            }

                            if (x > x0)
                            {
                                // Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
                                if (pData->rdh.nCount >= maxRects)
                                {
                                    GlobalUnlock(hData);
                                    maxRects += ALLOC_UNIT;
                                    hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
                                    pData = (RGNDATA *)GlobalLock(hData);
                                }
                                RECT *pr = (RECT *)&pData->Buffer;
                                SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
                                if (x0 < pData->rdh.rcBound.left)
                                    pData->rdh.rcBound.left = x0;
                                if (y < pData->rdh.rcBound.top)
                                    pData->rdh.rcBound.top = y;
                                if (x > pData->rdh.rcBound.right)
                                    pData->rdh.rcBound.right = x;
                                if (y+1 > pData->rdh.rcBound.bottom)
                                    pData->rdh.rcBound.bottom = y+1;
                                pData->rdh.nCount++;

                                // On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
                                // large (ie: > 4000). Therefore, we have to create the region by multiple steps.
                                if (pData->rdh.nCount == 2000)
                                {
                                    HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
                                    if (hRgn)
                                    {
                                        CombineRgn(hRgn, hRgn, h, RGN_OR);
                                        DeleteObject(h);
                                    }
                                    else
                                        hRgn = h;
                                    pData->rdh.nCount = 0;
                                    SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
                                }
                            }
                        }

                        // Go to next row (remember, the bitmap is inverted vertically)
                        p32 -= bm32.bmWidthBytes;
                    }

                    // Create or extend the region with the remaining rectangles
                    HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
                    if (hRgn)
                    {
                        CombineRgn(hRgn, hRgn, h, RGN_OR);
                        DeleteObject(h);
                    }
                    else
                        hRgn = h;

                    // Clean up
                    GlobalFree(hData);
                    SelectObject(hDC, holdBmp);
                    DeleteDC(hDC);
                }

                DeleteObject(SelectObject(hMemDC, holdBmp));
            }

            DeleteDC(hMemDC);
        }    
    }

    return hRgn;

}

COLORREF CGdiEx::BrightColor(COLORREF aSourceColor, FLOAT afAmount)
{
	FLOAT lfHue, lfBrightness, lfSaturation;
	COLORREF lColor = aSourceColor;

	SetHBS(lColor, lfHue, lfBrightness, lfSaturation);
	if (afAmount > 0 && afAmount < 1)
	{
		lfBrightness = afAmount;
		SetRGB(lColor, lfHue, lfBrightness, lfSaturation);
	}

	return lColor;
}

inline void CGdiEx::RGBtoHSL(COLORREF rgb, double* H, double* S, double* L)
{
	double delta;
	double r = (double)GetRValue(rgb)/255;
	double g = (double)GetGValue(rgb)/255;
	double b = (double)GetBValue(rgb)/255;
	double cmax = max(r,max(g,b));
	double cmin = min(r,min(g,b));
	*L = (cmax+cmin)/2.0;
	if (cmax == cmin)
	{
		*S = 0;
		*H = 0; // it's really undefined
	}
	else
	{
		if (*L < 0.5)
			*S = (cmax-cmin)/(cmax+cmin);
		else
			*S = (cmax-cmin)/(2.0-cmax-cmin);
		delta = cmax - cmin;
		if (r==cmax)
			*H = (g-b)/delta;
		else if (g==cmax)
			*H = 2.0 +(b-r)/delta;
		else
			*H = 4.0+(r-g)/delta;
		*H /= 6.0;
		if (*H < 0.0)
			*H += 1;
	}
}
inline COLORREF CGdiEx::HLStoRGB(const double& H, const double& L, const double& S)
{
	double r,g,b;
	double m1, m2;

	if (S==0)
	{
		r=g=b=L;
	}
	else
	{
		if (L <= 0.5)
			m2 = L*(1.0+S);
		else
			m2 = L+S-L*S;
		m1 = 2.0*L-m2;
		r = HuetoRGB(m1, m2, H+1.0/3.0);
		g = HuetoRGB(m1, m2, H);
		b = HuetoRGB(m1, m2, H-1.0/3.0);
	}
	return RGB((BYTE)(r*255),(BYTE)(g*255),(BYTE)(b*255));
}


inline double CGdiEx::HuetoRGB(double m1, double m2, double h)
{
	if (h < 0)
		h += 1.0;
	else if (h > 1)
		h -= 1.0;

	if (6.0*h < 1)
		return (m1+(m2-m1)*h*6.0);
	else if (2.0*h < 1)
		return m2;
	else if (3.0*h < 2.0)
		return (m1+(m2-m1)*((2.0/3.0)-h)*6.0);
	return m1;
}
void CGdiEx::FillRectangle(HDC DC, RECT aRect,COLORREF aBeginColor, COLORREF aEndColor, ENUM_DrawOrientation Orientation, BOOL abEclosion, UINT anStep)
{
	if(aBeginColor == aEndColor)
	{
		FillRectangle(DC, aRect, aBeginColor,  abEclosion);
	}
	else
	{
		int nWidth  = aRect.right  - aRect.left;
		int nHeight = aRect.bottom - aRect.top;  
		if(!(nWidth<=0|| nHeight<= 0))
		{
			float fStep;
			RECT rect;
			HBRUSH hbrFill;
			anStep = anStep == 1 ?  10:anStep;
			if( -1 == anStep)
			{
				if(true == Orientation)
				{
					anStep = nHeight > 10 ? 10 : nHeight;

				}
				else
				{
					anStep = nWidth >= 10 ? 10: nWidth;
				}
			}

			int nR = GetRValue( aBeginColor );
			int nG = GetGValue( aBeginColor );
			int nB = GetBValue( aBeginColor );

			float fRStep = ( GetRValue( aEndColor ) - GetRValue( aBeginColor ) ) / (float)anStep;
			float fGStep = ( GetGValue( aEndColor ) - GetGValue( aBeginColor ) ) / (float)anStep;
			float fBStep = ( GetBValue( aEndColor ) - GetBValue( aBeginColor ) ) / (float)anStep;

			HDC hdcMem   = ::CreateCompatibleDC( DC );
			HBITMAP hbmpMem  = ::CreateCompatibleBitmap( DC, nWidth, nHeight );
			HGDIOBJ hobjOld  = ::SelectObject( hdcMem, hbmpMem );

			rect.top = rect.left = 0;

			if ( Orientation )
			{
				rect.right= nWidth;
				fStep= (float)nHeight / (float)anStep;

				for ( int nCount=0; nCount<=anStep; nCount++ )
				{
					rect.bottom = (int)( (nCount+1) * fStep );

					hbrFill = ::CreateSolidBrush( RGB( nR+fRStep*nCount, nG+fGStep*nCount, nB+fBStep*nCount ) );
					::FillRect( hdcMem, &rect, hbrFill );
					::DeleteObject( hbrFill );

					rect.top = rect.bottom;
				}
			}
			else
			{
				rect.bottom = nHeight;
				fStep= (float)nWidth / (float)anStep;

				for ( int nCount=0; nCount<=anStep; nCount++ )
				{
					rect.right= (int)( (nCount+1) * fStep );

					hbrFill = ::CreateSolidBrush( RGB( nR+fRStep*nCount, nG+fGStep*nCount, nB+fBStep*nCount ) );
					::FillRect( hdcMem, &rect, hbrFill );
					::DeleteObject( hbrFill );

					rect.left = rect.right;
				}
			}

			::BitBlt( DC, aRect.left, aRect.top, nWidth, nHeight, hdcMem, 0, 0, SRCCOPY );

			::SelectObject( hdcMem, hobjOld );
			::DeleteObject( hbmpMem );
			::DeleteDC( hdcMem );
		}
	}
}
void CGdiEx::FillRectangleHue( HDC DC, RECT aRect,COLORREF aBeginColor, COLORREF aEndColor, ENUM_DrawOrientation Orientation, BOOL abEclosion, UINT anStep )
{
	if(aBeginColor == aEndColor)
	{
		FillRectangle(DC, aRect, aBeginColor,  abEclosion);
	}
	else
	{
		int nWidth  = aRect.right  - aRect.left;
		int nHeight = aRect.bottom - aRect.top;  
		if(!(nWidth<=0|| nHeight<= 0))
		{
			float fStep;
			RECT rect;
			HBRUSH hbrFill;
			anStep = anStep == 1 ?  10:anStep;
			if( -1 == anStep)
			{
				if(true == Orientation)
				{
					anStep = nHeight > 10 ? 10 : nHeight;

				}
				else
				{
					anStep = nWidth >= 10 ? 10: nWidth;
				}
			}

			int    lR1,lG1,lB1;
			double lH1,lS1,lL1;
			lR1 = GetRValue( aBeginColor );
			lG1 = GetGValue( aBeginColor );
			lB1 = GetBValue( aBeginColor );
			RGBtoHSL(RGB(lR1,lG1,lB1),&lH1,&lS1,&lL1);

			int    lR2,lG2,lB2;
			double lH2,lS2,lL2;
			lR2 = GetRValue( aEndColor );
			lG2 = GetGValue( aEndColor );
			lB2 = GetBValue( aEndColor );
			RGBtoHSL(RGB(lR2,lG2,lB2),&lH2,&lS2,&lL2);

			float fHStep = ( lH2 - lH1 )*240 / (float)anStep;
			HDC hdcMem   = ::CreateCompatibleDC( DC );
			HBITMAP hbmpMem  = ::CreateCompatibleBitmap( DC, nWidth, nHeight );
			HGDIOBJ hobjOld  = ::SelectObject( hdcMem, hbmpMem );

			rect.top = rect.left = 0;

			if ( Orientation )
			{
				rect.right= nWidth;
				fStep= (float)nHeight / (float)anStep;

				for ( int nCount=0; nCount<=anStep; nCount++ )
				{
					rect.bottom = (int)( (nCount+1) * fStep );

					hbrFill = ::CreateSolidBrush( HLStoRGB(lH1+fHStep*nCount/(float)(240) ,0.5,1.0 )  );
					::FillRect( hdcMem, &rect, hbrFill );
					::DeleteObject( hbrFill );

					rect.top = rect.bottom;
				}
			}
			else
			{
				rect.bottom = nHeight;
				fStep= (float)nWidth / (float)anStep;

				for ( int nCount=0; nCount<=anStep; nCount++ )
				{
					rect.right= (int)( (nCount+1) * fStep );
					hbrFill = ::CreateSolidBrush( HLStoRGB(lH1+fHStep*nCount/(float)(240) ,0.5,1.0 )  );
					::FillRect( hdcMem, &rect, hbrFill );
					::DeleteObject( hbrFill );

					rect.left = rect.right;
				}
			}

			::BitBlt( DC, aRect.left, aRect.top, nWidth, nHeight, hdcMem, 0, 0, SRCCOPY );

			::SelectObject( hdcMem, hobjOld );
			::DeleteObject( hbmpMem );
			::DeleteDC( hdcMem );
		}
	}
}
void CGdiEx::FillRectangleLight( HDC DC, RECT aRect,COLORREF aBeginColor, COLORREF aEndColor,UINT aHue, ENUM_DrawOrientation Orientation, BOOL abEclosion, UINT anStep )
{
	aHue = aHue > 239 ? 239 : aHue;
	if(aBeginColor == aEndColor)
	{
		FillRectangle(DC, aRect, aBeginColor,  abEclosion);
	}
	else
	{
		int nWidth  = aRect.right  - aRect.left;
		int nHeight = aRect.bottom - aRect.top;  
		if(!(nWidth<=0|| nHeight<= 0))
		{
			float fStep;
			RECT rect;
			HBRUSH hbrFill;
			anStep = anStep == 1 ?  10:anStep;
			if( -1 == anStep)
			{
				if(true == Orientation)
				{
					anStep = nHeight > 10 ? 10 : nHeight;

				}
				else
				{
					anStep = nWidth >= 10 ? 10: nWidth;
				}
			}

			int    lR1,lG1,lB1;
			double lH1,lS1,lL1;
			lR1 = GetRValue( aBeginColor );
			lG1 = GetGValue( aBeginColor );
			lB1 = GetBValue( aBeginColor );
			RGBtoHSL(RGB(lR1,lG1,lB1),&lH1,&lS1,&lL1);

			int    lR2,lG2,lB2;
			double lH2,lS2,lL2;
			lR2 = GetRValue( aEndColor );
			lG2 = GetGValue( aEndColor );
			lB2 = GetBValue( aEndColor );
			RGBtoHSL(RGB(lR2,lG2,lB2),&lH2,&lS2,&lL2);

			float fLStep = ( lL2 - lL1 )*240 / (float)anStep;
			HDC hdcMem   = ::CreateCompatibleDC( DC );
			HBITMAP hbmpMem  = ::CreateCompatibleBitmap( DC, nWidth, nHeight );
			HGDIOBJ hobjOld  = ::SelectObject( hdcMem, hbmpMem );

			rect.top = rect.left = 0;

			if ( Orientation )
			{
				rect.right= nWidth;
				fStep= (float)nHeight / (float)anStep;

				for ( int nCount=0; nCount<=anStep; nCount++ )
				{
					rect.bottom = (int)( (nCount+1) * fStep );

					hbrFill = ::CreateSolidBrush( HLStoRGB((double)(aHue/(float)(239) ) ,lL1+fLStep*nCount/(float)(240),1.0 )  );
					::FillRect( hdcMem, &rect, hbrFill );
					::DeleteObject( hbrFill );

					rect.top = rect.bottom;
				}
			}
			else
			{
				rect.bottom = nHeight;
				fStep= (float)nWidth / (float)anStep;

				for ( int nCount=0; nCount<=anStep; nCount++ )
				{
					rect.right= (int)( (nCount+1) * fStep );
					hbrFill = ::CreateSolidBrush( HLStoRGB((double)(aHue/(float)(239)) ,lL1+fLStep*nCount/(float)(240),1.0 )  );
					::FillRect( hdcMem, &rect, hbrFill );
					::DeleteObject( hbrFill );

					rect.left = rect.right;
				}
			}

			::BitBlt( DC, aRect.left, aRect.top, nWidth, nHeight, hdcMem, 0, 0, SRCCOPY );

			::SelectObject( hdcMem, hobjOld );
			::DeleteObject( hbmpMem );
			::DeleteDC( hdcMem );
		}
	}
}
void CGdiEx::FrameRect(HDC DC, RECT& aRect, CBrush* pBrush)
{
	CDC * pDC = CDC::FromHandle(DC);
	pDC->FrameRect( &aRect,pBrush );
}
void CGdiEx::FillRectangle(HDC DC, RECT& aRect, COLORREF aColor )
{
	CDC * pDC = CDC::FromHandle(DC);
	pDC->FillSolidRect(aRect.left, aRect.top, aRect.right - aRect.left, aRect.bottom - aRect.top,aColor);
}
void CGdiEx::FillRectangle(HDC DC, RECT aRect, COLORREF aColor, BOOL abEclosion)
{
	CDC * pDC = CDC::FromHandle(DC);
	CBrush Brush, *pOldBrush;
	Brush.CreateSolidBrush(aColor);
	pOldBrush = pDC->SelectObject(&Brush);
	pDC->FillSolidRect(aRect.left, aRect.top, aRect.right - aRect.left, aRect.bottom - aRect.top,aColor);
	pDC->SelectObject(pOldBrush);
	Brush.DeleteObject();

}
void CGdiEx::FillRectangle(HDC DC, RECT aRect, COLORREF aColor, BOOL abEclosion, int maxX, int maxY)
{
	if( aRect.bottom < 0) return;
	if( aRect.right < 0)  return;
	if( aRect.left>maxX)  return;
	if( aRect.top> maxY)  return;
	
	
	if( aRect.top < 0) aRect.top = 0;
	if( aRect.left < 0) aRect.left = 0;
	if( aRect.right>maxX)   aRect.right = maxX;
	if( aRect.bottom>maxY ) aRect.bottom = maxY;

	CDC * pDC = CDC::FromHandle(DC);
	/*CBrush Brush, *pOldBrush;
	Brush.CreateSolidBrush(aColor);
	pOldBrush = pDC->SelectObject(&Brush);*/
	pDC->FillRect(&aRect,&CBrush(aColor) );
	/*pDC->SelectObject(pOldBrush);
	Brush.DeleteObject();*/

}
void CGdiEx::DrawLine(HDC DC, COLORREF aColor, FLOAT afPenWidth, INT aiBeginLeft, INT aiBeginTop, INT aiEndLeft, INT aiEndTop, BOOL abEclosion)
{
	CDC * pDC = CDC::FromHandle(DC);
	CPen Pen, *pOldPen;
	Pen.CreatePen(PS_SOLID, afPenWidth, aColor);	
	pOldPen = pDC->SelectObject(&Pen);

	pDC->MoveTo(aiBeginLeft, aiBeginTop);
	pDC->LineTo(aiEndLeft, aiEndTop);
	pDC->SelectObject(pOldPen);
	Pen.DeleteObject();
}

void CGdiEx::DrawArrow(HDC DC, COLORREF aColor, INT aiFirstLeft, INT aiFirstTop, INT aiSecondLeft, INT aiSecondTop, INT aiThirdLeft, INT aiThirdTop, BOOL aEclosion)
{
	CPoint Points[] = {CPoint(aiFirstLeft, aiFirstTop),
		CPoint(aiSecondLeft, aiSecondTop),
		CPoint(aiThirdLeft, aiThirdTop),
	};

	CDC * pDC = CDC::FromHandle(DC);
	CBrush Brush, *pOldBrush;
	Brush.CreateSolidBrush(aColor);
	pOldBrush = pDC->SelectObject(&Brush);
	CPen Pen, *pOldPen;
	Pen.CreatePen(PS_SOLID, 1, aColor);	
	pOldPen = pDC->SelectObject(&Pen);

	pDC->Polygon(Points, 3);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);

	Pen.DeleteObject();
	Brush.DeleteObject();
}
void CGdiEx::DrawPointFrameXOR(HDC DC,  RECT& aRect)
{
	CDC * pDC = CDC::FromHandle(DC);

	int nRop = pDC->SetROP2(R2_NOTXORPEN);
	CPen Pen, *pOldPen;
	Pen.CreatePen(0, 1, RGB(0, 0, 0));
	pOldPen = pDC->SelectObject(&Pen);

	int i,j;
	for( i= aRect.left+1, j=aRect.top; i<aRect.right; i++ )
	{
		pDC->SetPixel(i,j,0);
		i++;
	}
	for( i= aRect.right, j=aRect.top+1; j<aRect.bottom; j++ )
	{
		pDC->SetPixel(i,j,0 );
		j++;
	}
	for( i= aRect.right-1, j=aRect.bottom; i>aRect.left; i-- )
	{
		pDC->SetPixel(i,j,0);
		i--;
	}
	for( i= aRect.left, j=aRect.bottom-1; j>aRect.top; j-- )
	{
		pDC->SetPixel(i,j,0);
		j--;
	}
	pDC->SetROP2(nRop);
	pDC->SelectObject(pOldPen);
	Pen.DeleteObject();
}
void CGdiEx::DrawText(CDC* apDC, CString astrText, COLORREF acolText, RECT &lrectText )
{
	
	if( !(apDC->m_hDC) ) return; 
	apDC->SetTextColor(acolText);
	SetBkMode(apDC->m_hDC, TRANSPARENT);
	COLORREF ltempColor = apDC->GetTextColor();
	LOGFONT  lf;
	ZeroMemory(&lf,sizeof(lf));
	lf.lfHeight=-MulDiv(9,GetDeviceCaps(apDC->m_hDC,LOGPIXELSY),72);
	lf.lfWeight=FW_MEDIUM;
	lf.lfCharSet=GB2312_CHARSET;
	lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
	lf.lfQuality=PROOF_QUALITY;
	lf.lfPitchAndFamily=FF_DONTCARE | DEFAULT_PITCH;
	CFont font;
	font.CreateFontIndirect(&lf);
	CFont* oldFont = (CFont*)apDC->SelectObject(&font);
	::DrawText(
		apDC->m_hDC,
		astrText,
		astrText.GetLength(),
		&lrectText,
		DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS
		);
	apDC->SetTextColor(ltempColor);
	apDC->SelectObject(oldFont);
	font.DeleteObject();
}
void CGdiEx::DrawTextQuick(CDC* apDC, CString& astrText, COLORREF acolText, RECT &lrectText,CFont& font,int maxX, int maxY,int anFormat )
{
	
	if( !(apDC->m_hDC) ) return; 

	if( lrectText.bottom < 0) return;
	if( lrectText.right < 0)  return;
	if( lrectText.left>maxX)  return;
	if( lrectText.top> maxY)  return;

	
	
	apDC->SetTextColor(acolText);
	SetBkMode(apDC->m_hDC, TRANSPARENT);
	COLORREF ltempColor = apDC->GetTextColor();
	CFont* oldFont = (CFont*)apDC->SelectObject(&font);
	::DrawText(
		apDC->m_hDC,
		astrText,
		astrText.GetLength(),
		&lrectText,
		anFormat|DT_NOPREFIX
		);
	apDC->SetTextColor(ltempColor);	
	apDC->SelectObject(oldFont);
}
void CGdiEx::DrawTextQuick(CDC* apDC, CString& astrText, COLORREF acolText, RECT &lrectText,CFont& font,int anFormat )
{
	
	if( !(apDC->m_hDC) ) return; 

	apDC->SetTextColor(acolText);
	SetBkMode(apDC->m_hDC, TRANSPARENT);
	COLORREF ltempColor = apDC->GetTextColor();
	CFont* oldFont = (CFont*)apDC->SelectObject(&font);

	::DrawText(
		apDC->m_hDC,
		astrText,
		astrText.GetLength(),
		&lrectText,
		anFormat|DT_NOPREFIX
		);
	apDC->SetTextColor(ltempColor);	
	apDC->SelectObject(oldFont);
}
void CGdiEx::DrawTextQuick(CDC* apDC, CString& astrText, COLORREF acolText, RECT &lrectText,int anFormat )
{
	
	if( !(apDC->m_hDC) ) return; 
	
	apDC->SetTextColor(acolText);
	SetBkMode(apDC->m_hDC, TRANSPARENT);
	COLORREF ltempColor = apDC->GetTextColor();
	::DrawText(
		apDC->m_hDC,
		astrText,
		astrText.GetLength(),
		&lrectText,
		anFormat|DT_NOPREFIX
		);
	apDC->SetTextColor(ltempColor);	
}

void CGdiEx::SetDrawTextFont(CDC* apDC,CFont& font,bool bUnderline )
{
	
	if( !(apDC->m_hDC) ) return ; 
	LOGFONT  lf;
	ZeroMemory(&lf,sizeof(lf));
	lf.lfHeight=-MulDiv(9,GetDeviceCaps(apDC->m_hDC,LOGPIXELSY),72);
	lf.lfWeight=FW_MEDIUM;
	lf.lfCharSet=GB2312_CHARSET;
	lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
	lf.lfQuality=DEFAULT_QUALITY;
	lf.lfPitchAndFamily=FF_DONTCARE | DEFAULT_PITCH;
	if(bUnderline)
		lf.lfUnderline = BYTE(1);
	font.CreateFontIndirect(&lf);
}
CFont* CGdiEx::SetDrawTextFont(CDC* apDC,CFont* apFontSave )
{
	
	if( !(apDC->m_hDC) ) return NULL; 
	CFont* oldFont = (CFont*)apDC->SelectObject(apFontSave);
	return oldFont;
}
void CGdiEx::DrawText(CDC* apDC, CString astrText, COLORREF acolText, RECT &lrectText,UINT anFormat,bool abUnderLine,UINT anWidth,UINT anSize,UINT anSet,string strTextName )
{
	if( !(apDC->m_hDC) ) return; 
	apDC->SetTextColor(acolText);
	SetBkMode(apDC->m_hDC, TRANSPARENT);
	COLORREF ltempColor = apDC->GetTextColor();
	LOGFONT  lf;
	ZeroMemory(&lf,sizeof(lf));
	lf.lfHeight=-MulDiv(anSize,GetDeviceCaps(apDC->m_hDC,LOGPIXELSY),72); // 12
	lf.lfWeight= anWidth;//FW_NORMAL
	lf.lfWidth =0;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet=anSet;
	lf.lfOutPrecision=OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision=CLIP_DEFAULT_PRECIS;
	lf.lfQuality=DEFAULT_QUALITY;
	lf.lfPitchAndFamily=FF_DONTCARE | DEFAULT_PITCH;
	lf.lfUnderline = abUnderLine;
	if( 0 != strTextName.size() )
	{
		strcpy(lf.lfFaceName, "隶书"/*strTextName.c_str()*/ );
	}

	CFont font;
	font.CreateFontIndirect(&lf);
	CFont* oldFont = (CFont*)apDC->SelectObject(&font);
	if ( abUnderLine )
	{
		::DrawText(
			apDC->m_hDC,
			astrText,
			astrText.GetLength(),
			&lrectText,
			anFormat|DT_WORDBREAK|DT_END_ELLIPSIS
			);
	}
	else
	{
		::DrawText(
			apDC->m_hDC,
			astrText,
			astrText.GetLength(),
			&lrectText,
			anFormat|DT_END_ELLIPSIS
			);
	}
	apDC->SetTextColor(ltempColor);
	apDC->SelectObject(oldFont);
	font.DeleteObject();
}
BOOL   CGdiEx::GetWindowRectInParentCoordinates(HWND   hwnd,   PRECT   prc)   
{      
	if(!hwnd   ||   !prc   ||   !IsWindow(hwnd))   
		return   FALSE;   

	RECT   rc;   
	if(!GetWindowRect(hwnd,   &rc))   
		return   FALSE;   

	if((GetWindowLong(hwnd,   GWL_STYLE)   &   WS_CHILD))   
	{   
		if(!MapWindowPoints(HWND_DESKTOP,   GetParent(hwnd),   (LPPOINT)&rc,   2))   
			return   FALSE;   
	}   

	*prc   =   rc;   
	return   TRUE;   
}   
void CGdiEx::DrawBitmap( CDC* apDC, CBitmap& abmpBigmap,CRect& aRect,UINT aMapID )
{
	if( !abmpBigmap.m_hObject )
		abmpBigmap.LoadBitmap(aMapID);
	CDC dcMem;
	dcMem.CreateCompatibleDC(apDC); 
	CBitmap *pbmpOld=dcMem.SelectObject(&abmpBigmap);
	apDC->BitBlt(aRect.left,aRect.top,aRect.Width(),aRect.Height(),&dcMem,0,0,SRCCOPY);
	dcMem.DeleteDC();
}
void CGdiEx::DrawBitmap( CDC* apDC, CBitmap& abmpBigmap,CRect& aRect )
{
	CDC dcMem;
	dcMem.CreateCompatibleDC(apDC); 
	CBitmap *pbmpOld=dcMem.SelectObject(&abmpBigmap);
	apDC->BitBlt(aRect.left,aRect.top,aRect.Width(),aRect.Height(),&dcMem,0,0,SRCCOPY);
	dcMem.DeleteDC();
}
void CGdiEx::DrawStretchBitmap( CDC* apDC, CBitmap& abmpBigmap,CRect& aRect )
{
	BITMAP lbmp;
	abmpBigmap.GetBitmap( &lbmp );
	CDC dcMem;
	dcMem.CreateCompatibleDC(apDC); 
	CBitmap *pbmpOld=dcMem.SelectObject(&abmpBigmap);
	apDC->StretchBlt(aRect.left,aRect.top,aRect.Width(),aRect.Height(),&dcMem, 0,0,lbmp.bmWidth, lbmp.bmHeight, SRCCOPY);
	dcMem.DeleteDC();
}
void CGdiEx::DrawStretchBitmap(  CDC* apDC, CBitmap& abmpBigmap,CRect& aRect,CRect& srcRect )
{
	CDC dcMem;
	dcMem.CreateCompatibleDC(apDC); 
	CBitmap *pbmpOld=dcMem.SelectObject(&abmpBigmap);
	apDC->StretchBlt(aRect.left,aRect.top,aRect.Width(),aRect.Height(),&dcMem, srcRect.left,srcRect.top,srcRect.Width(),srcRect.Height(),SRCCOPY);
	dcMem.DeleteDC();
}
void CGdiEx::DrawStretchBitmap2(  CDC* apDC, CBitmap& abmpBigmap,CRect& aRect,CRect& srcRect )
{
	if( aRect.IsRectEmpty() ) return ;
	int lpos1;
	int lpos2;
	int lpos3;
	int lhight1;
	int lhight2;
	if( ( srcRect.bottom - srcRect.top+1)%2 ==0 )
	{
		 lpos1 = srcRect.top+(srcRect.bottom - srcRect.top+1)/2 -1-1;
		 lpos2 = lpos1+2;
		 lhight1 = lpos1- srcRect.top+1;
		 lhight2 = 2;
	}
	else
	{
		 lpos1 = srcRect.top+(srcRect.bottom - srcRect.top+1)/2-1;
		 lpos2 = lpos1+1;
		 lhight1 = lpos1- srcRect.top+1;
		 lhight2 = 1;
	}
	lpos3 = lpos2+1;
	CDC dcMem;
	dcMem.CreateCompatibleDC(apDC); 
	CBitmap *pbmpOld=dcMem.SelectObject(&abmpBigmap);
	apDC->StretchBlt(aRect.left,aRect.top,aRect.Width(),lhight1,&dcMem, srcRect.left,srcRect.top,srcRect.Width(),lhight1,SRCCOPY);
	apDC->StretchBlt(aRect.left,aRect.top+lhight1 ,aRect.Width(),aRect.Height()-2*lhight1,&dcMem, srcRect.left,lpos2,srcRect.Width(),lhight2,SRCCOPY);
	apDC->StretchBlt(aRect.left,aRect.bottom-lhight1,aRect.Width(),lhight1,&dcMem, srcRect.left,lpos3,srcRect.Width(),lhight1,SRCCOPY);
	dcMem.DeleteDC();
}

//void CGdiEx::DrawBitmap( CDC* apDC, CRect& aRect, CString astrFileName)
//{
//	if( 0 == astrFileName.GetLength() ) return ;
//	TCHAR   szPath[MAX_PATH];
//	GetCurrentDirectory(MAX_PATH,szPath) ;
//	CString  strRootPath(szPath);
//	strRootPath += _T("\\skin\\") ;
//	strRootPath += astrFileName;
//	HBITMAP   lhBitmap  = (HBITMAP)LoadImage(NULL,strRootPath, IMAGE_BITMAP,aRect.Width(),aRect.Height(), LR_LOADFROMFILE);  //用控件大小显示图片
//	CBitmap lBitMap;
//	lBitMap.Attach(lhBitmap);
//	if( lBitMap.m_hObject )
//	{
//	CDC dcMem;
//	dcMem.CreateCompatibleDC(apDC); 
//	CBitmap* pbmpOld=(CBitmap*)dcMem.SelectObject(&lBitMap);
//	apDC->BitBlt(aRect.left,aRect.top,aRect.Width(),aRect.Height(),&dcMem,0,0,SRCCOPY);
//	lBitMap.Detach();
//	lBitMap.DeleteObject();
//	}
//}

BOOL CGdiEx::AttachBmpFromFile(CBitmap* apBitmap, CString astrFileName, UINT anWidth,UINT anHeight)
{	if( NULL == apBitmap) return FALSE;
if( 0 == astrFileName.GetLength() ) return FALSE;
TCHAR   szPath[MAX_PATH];
GetCurrentDirectory(MAX_PATH,szPath) ;
CString  strRootPath(szPath);
strRootPath += _T("\\skin\\");
strRootPath += astrFileName;
HBITMAP   lhBitmap  = (HBITMAP)LoadImage(NULL,strRootPath, IMAGE_BITMAP,anWidth,anHeight, LR_LOADFROMFILE);  
return apBitmap->Attach( lhBitmap );

}
BOOL CGdiEx::GetImageListFromFile(CImageList* apImagelist, CString astrFileName, UINT anListCount,UINT anWidth,UINT anHeight )
{
	if( ( NULL != apImagelist )&& (apImagelist->m_hImageList) ) return TRUE; 
	if(0==apImagelist) return FALSE;
	if( 0 == astrFileName.GetLength() ) return FALSE;
	TCHAR   szPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,szPath) ;
	CString  strRootPath(szPath);
	strRootPath += _T("\\skin\\") ;
	strRootPath += astrFileName;
	HBITMAP   lhBitmap  = (HBITMAP)LoadImage(NULL,strRootPath, IMAGE_BITMAP,anWidth,anHeight, LR_LOADFROMFILE);
	CBitmap lBitMap;
	lBitMap.Attach(lhBitmap);
	if( lBitMap.m_hObject )
	{
		BITMAP   ltagBitMap;
		lBitMap.GetBitmap(&ltagBitMap);
		if( !apImagelist->m_hImageList )
		{
			apImagelist->Create(ltagBitMap.bmWidth/anListCount, ltagBitMap.bmHeight, ILC_COLOR24|ILC_MASK, anListCount, anListCount);
			apImagelist->Add(&lBitMap,0xff00ff);
		}
		lBitMap.Detach();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

CGdiEx gGdiExOperate;

CGdiEx* AfxGdiOperator()
{
	return &gGdiExOperate;
}
