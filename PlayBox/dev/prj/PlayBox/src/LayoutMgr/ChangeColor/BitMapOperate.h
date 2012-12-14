#include <map>
#include "windows.h"
using std::map;

typedef map<COLORREF, COLORREF> CHANGE_COLOR_MAP;

#pragma once
class DIBINFO
{
public:
	LPVOID m_lpvColorTable;
	UINT   m_width;
	UINT   m_hight;
	WORD   m_biBitCount;
	LPBYTE m_lpImage; 
	DIBINFO()
	{
		::ZeroMemory(this,sizeof(*this) );
	};
	DIBINFO(LPVOID alpvColorTable,UINT aWidth,UINT aHight, WORD   aBiBitCount = 24,LPBYTE alpImage = NULL )
	:m_lpvColorTable(alpvColorTable),m_width(aWidth),m_hight(aHight),m_biBitCount(aBiBitCount), m_lpImage(alpImage)
	{
	}
	virtual ~DIBINFO()
	{
		::ZeroMemory(this,sizeof(*this) );
	};
};
class CBitMapOperate : public DIBINFO
{
private:
	int  m_nLastLightPos;
	COLORREF  m_clrMap[65536];
public:
	void SetAdjustValue(int lastLightPos){m_nLastLightPos = lastLightPos;};
	void BeforeChangeColor();
	void AfterChangeColor();
	enum ENUM_DrawOrientation {edoHorizontal, edoVertical};   
	CBitMapOperate(void);
	~CBitMapOperate(void);

public:
	inline void RGBtoHSL(COLORREF rgb, double* H, double* S, double* L);
	inline float HuetoRGB(float m1, float m2, float h);
	inline COLORREF HLStoRGB(const double& H, const double& L, const double& S);
	inline void HLStoRGB(const float& H, const float& L, const float& S, unsigned char*  B, unsigned char*  G, unsigned char*  R);
	inline void RGBtoHSL( float r, float g, float b, float* H, float* S, float* L);
	void SetHBS(COLORREF aSourceColor, FLOAT &afHue, FLOAT &afBrightness, FLOAT &afSaturation);
	void SetRGB(COLORREF &aSourceColor, FLOAT afHue, FLOAT afBrightness, FLOAT afSaturation);

	HBITMAP  ChangeBmpSkin(HBITMAP &ahSourceBmp,UINT &aColorGroup, COLORREF &aBaseColor, COLORREF &aMaskColor,UINT &aiLightPos);
	DIBINFO* ChangeBmpSkin(DIBINFO &ahSourceDib,UINT &aColorGroup, COLORREF &aBaseColor, COLORREF &aMaskColor,UINT &aiLightPos);
	void changeColor(COLORREF &aColor, COLORREF aBaseColor ,UINT anLightPos);
	virtual void FillRectangle(HDC DC, RECT aRect,COLORREF aBeginColor, COLORREF aEndColor, ENUM_DrawOrientation Orientation,BOOL abEclosion, UINT anStep =1 );
	virtual void FillRectangleHue( HDC DC, RECT aRect,COLORREF aBeginColor, COLORREF aEndColor, ENUM_DrawOrientation Orientation, BOOL abEclosion, UINT anStep=1);
	virtual void FillRectangleLight(HDC DC, RECT aRect,COLORREF aBeginColor, COLORREF aEndColor,UINT aHue, ENUM_DrawOrientation Orientation, BOOL abEclosion,UINT anStep=1);
	void FillRectangleS( HDC DC, RECT aRect,COLORREF aBeginColor, COLORREF aEndColor,UINT aHue, ENUM_DrawOrientation Orientation, BOOL abEclosion, UINT anStep );
	virtual void FillRectangle(HDC DC, RECT aRect, COLORREF aColor, BOOL abEclosion);
	virtual void DrawLine(HDC DC, COLORREF aColor, FLOAT afPenWidth, INT aiBeginLeft, INT aiBeginTop, INT aiEndLeft, INT aiEndTop, BOOL abEclosion);
	virtual void DrawArrow(HDC DC, COLORREF aColor, INT aiFirstLeft, INT aiFirstTop, INT aiSecondLeft, INT aiSecondTop, INT aiThirdLeft, INT aiThirdTop, BOOL aEclosion);
	virtual void DrawText(CDC* apDC, CString astrText, COLORREF acolText, RECT &lrectText,UINT anFormat,bool abUnderLine,UINT anWidth,UINT anSize,UINT anSet,string strTextName );

	HRGN CreateRgnFromBitmap(HBITMAP hBmp, COLORREF color, CWnd* pWnd);
	COLORREF BrightColor(COLORREF aSourceColor, FLOAT afAmount);
	float GetOriLValue(float l, float adjust);
};
