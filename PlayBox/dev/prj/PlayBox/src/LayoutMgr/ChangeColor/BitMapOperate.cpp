#include "StdAfx.h"
#include "BitMapOperate.h"
#include "wingdi.h"
#include "shlwapi.h"


#pragma  warning(disable:4244)
#pragma  warning(disable:4805)
#pragma  warning(disable:4018)
#pragma  warning(disable:4067)


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef COLORREF2RGB(Color)
#define COLORREF2RGB(Color) (Color & 0xff00) | ((Color >> 16) & 0xff) | ((Color << 16) & 0xff0000)
#endif
CBitMapOperate::CBitMapOperate(void)
{
	m_nLastLightPos = 120;
}

CBitMapOperate::~CBitMapOperate(void)
{
}

inline void CBitMapOperate::RGBtoHSL( float r, float g, float b, float* H, float* S, float* L)
{
	float delta;
	//double r = (double)GetRValue(rgb)/255;
	//double g = (double)GetGValue(rgb)/255;
	//double b = (double)GetBValue(rgb)/255;
	r /= 255;
	g /=255;
	b /= 255;

	float cmax = max(r,max(g,b));
	float cmin = min(r,min(g,b));
	*L = (cmax+cmin)/(float)2.0;
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
			*S = (cmax-cmin)/((float)2.0-cmax-cmin);

		//return ;

		delta = cmax - cmin;
		if (r==cmax)
			*H = (g-b)/delta;
		else if (g==cmax)
			*H = (float)2.0 +(b-r)/delta;
		else
			*H = (float)4.0+(r-g)/delta;
		*H /= (float)6.0;
		if (*H < 0.0)
			*H += 1;
	}
}


inline void CBitMapOperate::RGBtoHSL(COLORREF rgb, double* H, double* S, double* L)
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
inline void CBitMapOperate::HLStoRGB(const float& H, const float& L, const float& S, unsigned char * B, unsigned char * G, unsigned char * R)
{
	float r,g,b;
	float m1, m2;

	if (S==0)
	{
		r=g=b=L;
		*R = 255.0 * r;
		*G = *B = *R;
	}
	else
	{
		if (L <= 0.5)
			m2 = L*((float)1.0+S);
		else
			m2 = L+S-L*S;
		m1 = (float)2.0*L-m2;
		*R = (unsigned char) (HuetoRGB(m1, m2, H+(float)0.66666667) * (float)255.0);
		*G = (unsigned char) (HuetoRGB(m1, m2, H) * (float)255.0);
		*B = (unsigned char )(HuetoRGB(m1, m2, H-(float)0.66666667) * (float)255.0);
	}
}

inline COLORREF CBitMapOperate::HLStoRGB(const double& H, const double& L, const double& S)
{
	unsigned char B, G, R;
	HLStoRGB(H,L,S, &R,&G,&B);

	return RGB(R,G,B);
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

inline float CBitMapOperate::HuetoRGB(float m1, float m2, float h)
{
	if (h < 0)
		h += 1.0;
	else if (h > 1)
		h -= 1.0;
	if (h < 0.16666667)
		return (m1+(m2-m1)*h*6.0);
	if (h < 0.5)
		return m2;
	if (h < 0.66666667)
		return (m1+(m2-m1)*(0.66666667-h)*6.0);
	return m1;
}
void CBitMapOperate::SetHBS(COLORREF aSourceColor, FLOAT &afHue, FLOAT &afBrightness, FLOAT &afSaturation)
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
void CBitMapOperate::SetRGB(COLORREF &aSourceColor, FLOAT afHue, FLOAT afBrightness, FLOAT afSaturation)
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
		lfRed = HuetoRGB(lfValue1, lfValue2, afHue + 120.0f);
		lfGreen = HuetoRGB(lfValue1, lfValue2, afHue);
		lfBlue = HuetoRGB(lfValue1, lfValue2, afHue - 120.0f);
	}

	aSourceColor = RGB(lfRed, lfGreen, lfBlue);
}
HBITMAP CBitMapOperate::ChangeBmpSkin(HBITMAP &ahSourceBmp,UINT &aColorGroup, COLORREF &aBaseColor, COLORREF &aMaskColor,UINT &aiLightPos)
{
	HBITMAP lhNewBmp=NULL;
	if (ahSourceBmp)
	{	
		HDC lBufferDC=CreateCompatibleDC(NULL);	
		if (lBufferDC)
		{
			HGDIOBJ lSourceBufferObject = SelectObject(lBufferDC,ahSourceBmp);	
			HDC lDirectDC=CreateCompatibleDC(NULL);
			if (lDirectDC)
			{
				BITMAP lbMap;
				GetObject(ahSourceBmp, sizeof(lbMap), &lbMap);
				BITMAPINFO RGB24Bit; 
				ZeroMemory(&RGB24Bit,sizeof(BITMAPINFO));
				RGB24Bit.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
				RGB24Bit.bmiHeader.biWidth=lbMap.bmWidth;
				RGB24Bit.bmiHeader.biHeight=lbMap.bmHeight;
				RGB24Bit.bmiHeader.biPlanes=1;
				RGB24Bit.bmiHeader.biBitCount=32;
				UINT * lptPixels;
				HBITMAP lDirectBitmap= CreateDIBSection(lDirectDC, (BITMAPINFO *)&RGB24Bit, DIB_RGB_COLORS,(void **)&lptPixels, NULL, 0);
				if (lDirectBitmap)
				{
					HGDIOBJ lSourceObject=SelectObject(lDirectDC, lDirectBitmap);
					BitBlt(lDirectDC,0,0,lbMap.bmWidth,lbMap.bmHeight,lBufferDC,0,0,SRCCOPY);	

					int lR2,lG2,lB2;
					double  lH2,lS2,lL2;
					lR2 = GetRValue(aBaseColor);
					lG2 = GetGValue(aBaseColor);
					lB2 = GetBValue(aBaseColor);
					RGBtoHSL(RGB(lR2,lG2,lB2),&lH2,&lS2,&lL2);

					for (int i=((lbMap.bmWidth*lbMap.bmHeight)-1 );i>0;i--)
					{
						if(  lptPixels[i] != aMaskColor ) 
						{
							int    lR,lG,lB;
							double lH,lS,lL;
							lR =  GetBValue(lptPixels[i]);
							lG =  GetGValue(lptPixels[i]);
							lB =  GetRValue(lptPixels[i]);

							RGBtoHSL(RGB(lR,lG,lB),&lH,&lS,&lL);
							double lAdjuest = min( (double)(1.0 - lL), lL ); 
							if( aiLightPos>120 )
							{
								lAdjuest = ( ((double)(aiLightPos-120) )/ (double)120 ) * lAdjuest;
								lL += lAdjuest;
							}
							else
							{
								if( aiLightPos<120)
								{
									lAdjuest = ( ((double)(abs(int(aiLightPos-120) )) )/ (double)120 ) * lAdjuest;
									lL -= lAdjuest;
								}
							}
							/*int lgroup;
							if( lH!= 0)
							{
							lgroup = ( lH/0.125 + 1 );
							}
							else
							{
							lgroup = 1; 
							}*/
							COLORREF c   = COLORREF2RGB( HLStoRGB(lH2,lL,lS ) );
//							lptPixels[i] = c;

//							int lR3 = GetRValue(aColor);
//							int lG3 = GetGValue(aColor);
//							int lB3 = GetBValue(aColor);

							WORD wL1,wH1,wS1,wL2,wH2,wS2;
							ColorRGBToHLS(lptPixels[i],&wH1,&wL1,&wS1);
							ColorRGBToHLS(aBaseColor,&wH2,&wL2,&wS2);

							lptPixels[i] = ColorHLSToRGB(wH2,wL1,wS1);							

							//}
							/*	else
							{
							lptPixels[i] = COLORREF2RGB( HLStoRGB(lH,lL,lS2) );
							}*/
						}
					}
					SelectObject(lDirectDC,lSourceObject);
					lhNewBmp=lDirectBitmap;
				}
				DeleteDC(lDirectDC);
			}			
			SelectObject(lBufferDC,lSourceBufferObject);
			DeleteDC(lBufferDC);
		}
	}
	return lhNewBmp;
}

inline float CBitMapOperate::GetOriLValue(float l, float lAdjust)
{
	float v1, v2;

	v1 = (l - lAdjust)/(1-lAdjust);


	if(v1 > 0.5)
		return v1;
	else
	{
		v2 = l/ (1+lAdjust);
		return v2;
	}
}

DIBINFO* CBitMapOperate::ChangeBmpSkin(DIBINFO &ahSourceDib,UINT &aColorGroup, COLORREF &aBaseColor, COLORREF &aMaskColor,UINT &aiLightPos)
{
	if( (0==ahSourceDib.m_hight) || (0 == ahSourceDib.m_width) ) return &ahSourceDib;

	if(aBaseColor == 0)
		return &ahSourceDib;

	BOOL bJustBaseColor = false;
	BOOL bJustLight = false;

	if(aiLightPos == m_nLastLightPos)
		bJustBaseColor = true;

	if( 24 != ahSourceDib.m_biBitCount ) return &ahSourceDib;
	int lR2,lG2,lB2;
	int lMR, lMG, lMB;
	double lH2,lS2,lL2;
	lR2 = GetRValue(aBaseColor);
	lG2 = GetGValue(aBaseColor);
	lB2 = GetBValue(aBaseColor);

	lMR = GetRValue(aMaskColor);
	lMG = GetGValue(aMaskColor);
	lMB = GetBValue(aMaskColor);

	RGBtoHSL(RGB(lR2,lG2,lB2),&lH2,&lS2,&lL2);

	int lnFillBit = ahSourceDib.m_width * 3%4 == 0 ? 0 : 4-ahSourceDib.m_width * 3%4;
	LPBYTE lpBitTable =  (LPBYTE)(ahSourceDib.m_lpvColorTable);
		static int count = 0;
		static int count1 = 0;

		count1 ++;

	try
	{
		int lRowWidth = ahSourceDib.m_width*3 + lnFillBit;
		int lRowStart = 0;

		float lAdjust1 = ((float)aiLightPos / 120.0 - 1.0);
		float lLastAdjust = ((float)m_nLastLightPos / 120.0 - 1.0);

		lAdjust1 /= 5.0;
		lLastAdjust /= 5.0;

		float lH,lS,lL;
		//memset(&m_clrMap, 0, sizeof(COLORREF) * 65536);

		CHANGE_COLOR_MAP::iterator it;
		for (int lHightIndex = 0; lHightIndex<ahSourceDib.m_hight;  lHightIndex++, lRowStart += lRowWidth  )
		{
			for(int lWidthIndex = 0; lWidthIndex< lRowWidth ; lWidthIndex++  )
			{
				if( lWidthIndex + 2 > lRowWidth - lnFillBit ) break; 

				if(lMR == lpBitTable[lRowStart + lWidthIndex + 2] && 
				   lMG ==  lpBitTable[lRowStart + lWidthIndex + 1] &&
				   lMB == lpBitTable[lRowStart + lWidthIndex])
				{
					lWidthIndex += 2;
					continue;
				}
				if(lpBitTable[lRowStart + lWidthIndex + 2] == 0
					&&  lpBitTable[lRowStart + lWidthIndex + 1] == 0
					&& lpBitTable[lRowStart + lWidthIndex ]  == 0)
				{
					lWidthIndex += 2;
					continue;
				}


				RGBtoHSL(
					lpBitTable[lRowStart + lWidthIndex + 2],
					lpBitTable[lRowStart + lWidthIndex + 1],
					lpBitTable[lRowStart + lWidthIndex ],
				&lH,&lS,&lL);

				/*if(!bJustBaseColor)
				{
					lL = this->GetOriLValue(lL, lLastAdjust);
					float lAdjuest = (1.0 - lL > lL) ? lL: 1.0-lL; 
					lL += lAdjust1 * lAdjuest;
				}*/
	/*			lL = lL + lL2/2;
				if(lL > 1.0)
					lL = 1.0;*/

				lH = lH2;
				if(lH > 1.0)
					lH -= 1.0;
				if(lH < 0)
					lH += 1.0;

				lL = lL + (lL2 - 0.5)/4;
				if(lL > 1.0)
					lL = 1.0;
				if(lL <= 0)
					lL = 0.1;
				lS = lS + (lS2 - 0.5) * 2;
				if(lS > 1.0)
						lS = 1.0;
				if(lS <= 0)
						lS = 0.1;

				HLStoRGB(lH,lL,lS,
					lpBitTable +  lRowStart + lWidthIndex + 2,
					lpBitTable +  lRowStart + lWidthIndex + 1,
					lpBitTable +  lRowStart + lWidthIndex );

				lWidthIndex += 2;
					count ++;

			}
		}
	}
	catch(...)
	{
		CMemoryException lmemexcept;
		lmemexcept.ReportError();
	}
	
	//TRACE("<%d, %d> \n ", count, count1);
	return &ahSourceDib;
}

void CBitMapOperate::changeColor(COLORREF &aColor, COLORREF aBaseColor,UINT anLightPos)
{
	COLORREF c2 = aColor;

	int lR2,lG2,lB2;
	double lH2,lS2,lL2;

	if(aColor == 0)
		return;

	if(aBaseColor == 0)
		return ;
	lR2 = GetRValue(aBaseColor);
	lG2 = GetGValue(aBaseColor);
	lB2 = GetBValue(aBaseColor);
	RGBtoHSL(RGB(lR2,lG2,lB2),&lH2,&lS2,&lL2);

	int    lR,lG,lB;
	double lH,lS,lL;
	lR = GetRValue(aColor);
	lG = GetGValue(aColor);
	lB = GetBValue(aColor);
	RGBtoHSL(RGB(lR,lG,lB),&lH,&lS,&lL);

	lH = lH2;
	if(lH > 1.0)
		lH -= 1.0;
	if(lH < 0)
		lH += 1.0;

	lL = lL + (lL2 - 0.5)/4;
	if(lL > 1.0)
		lL = 1.0;
	if(lL < 0)
		lL = 0;
	lS = lS + (lS2 - 0.5) * 2;
	if(lS > 1.0)
			lS = 1.0;
	if(lS < 0)
			lS = 0;

	aColor =  HLStoRGB(lH,lL,lS );
}

HRGN CBitMapOperate::CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color, CWnd* pWnd )
{
	if (!hBmp) return NULL;
	if(NULL == pWnd) return NULL;

	BITMAP bm;
	GetObject( hBmp, sizeof(BITMAP), &bm );	// get bitmap attributes

	CDC dcBmp;
	dcBmp.CreateCompatibleDC(CDC::FromHandle( GetDC(pWnd->m_hWnd) ));	//Creates a memory device context for the bitmap
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
			ismask=(dcBmp.GetPixel(j,bm.bmHeight-i-1)!=color);
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
void CBitMapOperate::FillRectangle(HDC DC, RECT aRect,COLORREF aBeginColor, COLORREF aEndColor, ENUM_DrawOrientation Orientation, BOOL abEclosion, UINT anStep)
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

				for ( int nCount=0; nCount<=(int)anStep; nCount++ )
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

				for ( int nCount=0; nCount<=(int)anStep; nCount++ )
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
void CBitMapOperate::FillRectangleHue( HDC DC, RECT aRect,COLORREF aBeginColor, COLORREF aEndColor, ENUM_DrawOrientation Orientation, BOOL abEclosion, UINT anStep )
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

			float fHStep = (1.0 )*240 / (float)anStep;
			HDC hdcMem   = ::CreateCompatibleDC( DC );
			HBITMAP hbmpMem  = ::CreateCompatibleBitmap( DC, nWidth, nHeight );
			HGDIOBJ hobjOld  = ::SelectObject( hdcMem, hbmpMem );

			rect.top = rect.left = 0;

			if ( Orientation )
			{
				rect.right= nWidth;
				fStep= (float)nHeight / (float)anStep;

				for ( int nCount=0; nCount<=(int)anStep; nCount++ )
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
					COLORREF col = HLStoRGB(lH1+fHStep*nCount/(float)(240) ,0.5,1.0 );
					hbrFill = ::CreateSolidBrush( col );
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
void CBitMapOperate::FillRectangleS( HDC DC, RECT aRect,COLORREF aBeginColor, COLORREF aEndColor,UINT aHue, ENUM_DrawOrientation Orientation, BOOL abEclosion, UINT anStep )
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

			float fLStep = ( lS2 - lS1 )*240 / (float)anStep;
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

					COLORREF col = HLStoRGB((double)(aHue/(float)(239) ) ,0.5, lS1+fLStep*nCount/(float)(240) );
					hbrFill = ::CreateSolidBrush( col );
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
					COLORREF col = HLStoRGB((double)(aHue/(float)(239)) ,0.5,lS1+fLStep*nCount/(float)(240));
					hbrFill = ::CreateSolidBrush( col );
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
void CBitMapOperate::FillRectangleLight( HDC DC, RECT aRect,COLORREF aBeginColor, COLORREF aEndColor,UINT aHue, ENUM_DrawOrientation Orientation, BOOL abEclosion, UINT anStep )
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

					COLORREF col = HLStoRGB((double)(aHue/(float)(239) ) ,lL1+fLStep*nCount/(float)(240),0 );
					hbrFill = ::CreateSolidBrush( col );
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
					COLORREF col = HLStoRGB((double)(aHue/(float)(239)) ,lL1+fLStep*nCount/(float)(240),0);
					hbrFill = ::CreateSolidBrush( col );
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
void CBitMapOperate::FillRectangle(HDC DC, RECT aRect, COLORREF aColor, BOOL abEclosion)
{
	CDC * pDC = CDC::FromHandle(DC);
	CBrush Brush, *pOldBrush;
	Brush.CreateSolidBrush(aColor);
	pOldBrush = pDC->SelectObject(&Brush);
	pDC->FillSolidRect(aRect.left, aRect.top, aRect.right - aRect.left, aRect.bottom - aRect.top,aColor);
	pDC->SelectObject(pOldBrush);
	Brush.DeleteObject();

}
void CBitMapOperate::DrawLine(HDC DC, COLORREF aColor, FLOAT afPenWidth, INT aiBeginLeft, INT aiBeginTop, INT aiEndLeft, INT aiEndTop, BOOL abEclosion)
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

void CBitMapOperate::DrawArrow(HDC DC, COLORREF aColor, INT aiFirstLeft, INT aiFirstTop, INT aiSecondLeft, INT aiSecondTop, INT aiThirdLeft, INT aiThirdTop, BOOL aEclosion)
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
void CBitMapOperate::BeforeChangeColor()
{
	memset(&m_clrMap, 0, sizeof(COLORREF) * 65535);
}
COLORREF CBitMapOperate::BrightColor(COLORREF aSourceColor, FLOAT afAmount)
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
void CBitMapOperate::DrawText(CDC* apDC, CString astrText, COLORREF acolText, RECT &lrectText,UINT anFormat,bool abUnderLine,UINT anWidth,UINT anSize,UINT anSet,string strTextName )
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
		strcpy(lf.lfFaceName, "Á¥Êé"/*strTextName.c_str()*/ );
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
void CBitMapOperate::AfterChangeColor()
{
}

CBitMapOperate gBitMapOperate;


