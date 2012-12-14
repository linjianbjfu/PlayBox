#ifndef COLORREF2RGB(Color)
#define COLORREF2RGB(Color) (Color & 0xff00) | ((Color >> 16) & 0xff) | ((Color << 16) & 0xff0000)
#endif

#include <string>
#include  <iostream>
using namespace std;



#pragma once
	enum ENUM_DrawOrientation {edoHorizontal, edoVertical};   
class CGdiEx
{
	
public:
	CGdiEx(void);
	~CGdiEx(void);

public:
	BOOL   GetWindowRectInParentCoordinates(HWND   hwnd,   PRECT   prc);
	HRGN   CreateRgnFromBitmap(HBITMAP hBmp, COLORREF cTransparentColor =  0, COLORREF cTolerance = 0x101010);
	inline float ToRGB1(FLOAT afValue1, FLOAT afValue2, FLOAT afValue3);
    void SetHBS(COLORREF aSourceColor, FLOAT &afHue, FLOAT &afBrightness, FLOAT &afSaturation);
    void SetRGB(COLORREF &aSourceColor, FLOAT afHue, FLOAT afBrightness, FLOAT afSaturation);
    COLORREF BrightColor(COLORREF aSourceColor, FLOAT afAmount);
	inline void RGBtoHSL(COLORREF rgb, double* H, double* S, double* L);
	inline COLORREF HLStoRGB(const double& H, const double& L, const double& S);
	inline double HuetoRGB(double m1, double m2, double h);

	virtual void FrameRect(HDC DC, RECT& aRect, CBrush* pBrush);
	virtual void FillRectangle(HDC DC, RECT& aRect, COLORREF aColor );   // 重复调用时使用
	virtual void FillRectangle(HDC DC, RECT aRect, COLORREF aColor, BOOL abEclosion); // 大块区域
	virtual void FillRectangle(HDC DC, RECT aRect,COLORREF aBeginColor, COLORREF aEndColor, ENUM_DrawOrientation Orientation,BOOL abEclosion, UINT anStep =1 );
	virtual void FillRectangleHue( HDC DC, RECT aRect,COLORREF aBeginColor, COLORREF aEndColor, ENUM_DrawOrientation Orientation, BOOL abEclosion, UINT anStep=1);
	virtual void FillRectangleLight(HDC DC, RECT aRect,COLORREF aBeginColor, COLORREF aEndColor,UINT aHue, ENUM_DrawOrientation Orientation, BOOL abEclosion,UINT anStep=1);
	virtual void FillRectangle(HDC DC, RECT aRect, COLORREF aColor, BOOL abEclosion, int maxX, int maxY);
	virtual void DrawLine(HDC DC, COLORREF aColor, FLOAT afPenWidth, INT aiBeginLeft, INT aiBeginTop, INT aiEndLeft, INT aiEndTop, BOOL abEclosion);
	virtual void DrawArrow(HDC DC, COLORREF aColor, INT aiFirstLeft, INT aiFirstTop, INT aiSecondLeft, INT aiSecondTop, INT aiThirdLeft, INT aiThirdTop, BOOL aEclosion);
	virtual void DrawPointFrameXOR(HDC DC,  RECT& aRect);


	virtual void SetDrawTextFont(CDC* apDC,CFont& font,bool bUnderline=false ); // 返回 oldCFont , aFontSave 用于重新设置字体
	virtual CFont* SetDrawTextFont(CDC* apDC,CFont* apFontSave );
	virtual void DrawText(CDC* apDC, CString astrText, COLORREF acolText, RECT &lrectText );//SetDrawTextFont后， 快速DrawText
	virtual void DrawTextQuick(CDC* apDC, CString& astrText, COLORREF acolText, RECT &lrectText,CFont& font, int maxX, int maxY,int anFormat =DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
	virtual void DrawTextQuick(CDC* apDC, CString& astrText, COLORREF acolText, RECT &lrectText,CFont& font, int anFormat =DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
	virtual void DrawTextQuick(CDC* apDC, CString& astrText, COLORREF acolText, RECT &lrectText,int anFormat );
	virtual void DrawText(CDC* apDC, CString astrText, COLORREF acolText, RECT &lrectText,UINT anFormat,bool abUnderLine=false,UINT anWidth = FW_NORMAL,UINT anSize = 9,UINT anSet = ANSI_CHARSET,string strTextName="");
	
	virtual void DrawBitmap( CDC* apDC, CBitmap& abmpBigmap,CRect& aRect,UINT aMapID );
	virtual void DrawBitmap( CDC* apDC, CBitmap& abmpBigmap,CRect& aRect);
	virtual void DrawStretchBitmap( CDC* apDC, CBitmap& abmpBigmap,CRect& aRect );
	virtual void DrawStretchBitmap(  CDC* apDC, CBitmap& abmpBigmap,CRect& aRect,CRect& srcRect );
	virtual void DrawStretchBitmap2(  CDC* apDC, CBitmap& abmpBigmap,CRect& aRect,CRect& srcRect );
	//virtual void DrawBitmap( CDC* apDC, CRect& aRect, CString astrFileName);
	virtual BOOL AttachBmpFromFile(CBitmap* apBitmap, CString astrFileName, UINT anWidth,UINT anHeight);
	virtual BOOL GetImageListFromFile(CImageList* apImagelist, CString astrFileName, UINT anListCount,UINT anWidth,UINT anHeight );
};
class CMemDC : public CDC 
{
private:
	CBitmap* mpBitmap;
	CBitmap* mpOldBitmap;
	CDC* mpDC;
	CRect mrcBounds;
public:
	CMemDC(CDC* apDC, const CRect& arcBounds) : CDC()
	{
		CreateCompatibleDC(apDC);
		mpBitmap = new CBitmap;
		mpBitmap->CreateCompatibleBitmap(apDC, arcBounds.Width() + arcBounds.left, arcBounds.Height() + arcBounds.top);
		mpOldBitmap = SelectObject(mpBitmap);
		mpDC = apDC;
		mrcBounds = arcBounds;
	}
	~CMemDC() 
	{
		mpDC->BitBlt(mrcBounds.left, mrcBounds.top, mrcBounds.Width(), mrcBounds.Height(), 
					this, mrcBounds.left, mrcBounds.top, SRCCOPY);
		SelectObject(mpOldBitmap);
		if (mpBitmap != NULL)
		{
			delete mpBitmap;
			mpBitmap = NULL;
		}
	}
	CMemDC* operator->() 
	{
		return this;
	}
};

 CGdiEx* AfxGdiOperator();