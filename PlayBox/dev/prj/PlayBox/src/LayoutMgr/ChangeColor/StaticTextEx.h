#if !defined USLabelH
#define USLabelH

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "GdiPlusEx.h"



//以下定义为消息
#ifndef MSG_LABEL_MOUSEDOWN
#define MSG_LABEL_MOUSEDOWN		(20001)
#endif  
#ifndef MSG_LABEL_MOUSEUP
#define MSG_LABEL_MOUSEUP		(20002)
#endif 

enum ENUM_PAINT_STYLE {epsNormal, epsOpaque, epsHGradient, epsVGradient, espToolBar,epsInfoHeader};

class CStaticText : public CStatic
{
private:
	//成员变量
	WNDPROC mWndProc;
	COLORREF mBackColor;
	COLORREF mBeginColor;
	COLORREF mEndColor;
	COLORREF mBorderColor;
	ENUM_PAINT_STYLE mPaintStyle;
	bool mbSetText;
	CString mstrText;
	CString mLinkText;
	COLORREF mTextColor;
	UINT mTextSet;
	HBRUSH mhBrush;		//控制客户区背景颜色的画刷
	int miTextTop;
	LONG mlBold;
	bool mbCaptured;
	bool mbUrl;
	bool mbMouseOver;
	void MouseLeave();
	void MouseEnter();
	void InvalidateBack();
	UINT mFormat;
	UINT mWidth;
	UINT mSize;
	CString m_fontName;

protected:

	//{{AFX_VIRTUAL(CULLabel)
	virtual void PreSubclassWindow();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_VIRTUAL
	static LRESULT CALLBACK SelfProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	//自定义消息回调函数
	afx_msg HBRUSH CtlColor (CDC* pDC, UINT nCtlColor);
	void DrawFace(HWND hwnd,HDC hDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

public:
	//构造，析构
	CStaticText();
	virtual ~CStaticText();

	// $_FUNCTION_BEGIN ******************************
	// 函数名称：  SetGradient
	// 函数参数：  aBeginColor	渐变开始颜色
	//             aEndColor    渐变结束颜色
	// 返 回 值：  无
	// 函数说明：  绘制渐变风格(水平渐进)
	// $_FUNCTION_END ********************************
	void SetGradient(COLORREF aBeginColor, COLORREF aEndColor);


	// $_FUNCTION_BEGIN ******************************
	// 函数名称：  SetVerticalGradient
	// 函数参数：  aBeginColor	渐变开始颜色
	//             aEndColor    渐变结束颜色
	// 返 回 值：  无
	// 函数说明：  绘制渐变风格(竖直渐进)
	// $_FUNCTION_END ********************************
	void SetVerticalGradient(COLORREF aBeginColor, COLORREF aEndColor);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：  SetOpaque
	// 函数参数：  aBackColor  背景颜色
	// 返 回 值：  无
	// 函数说明：  绘制不透明风格
	// $_FUNCTION_END ********************************
	void SetOpaque(COLORREF aBackColor);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：  SetToolBarBK
	// 函数参数：  aBorderColor  边框颜色
	// 返 回 值：  无
	// 函数说明：  绘制不透明风格
	// $_FUNCTION_END ********************************
	void SetToolBarBK(COLORREF aBorderColor);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：  SetNormal
	// 函数参数：  无
	// 返 回 值：  无
	// 函数说明：  绘制默认风格(透明)
	// $_FUNCTION_END ********************************
	void SetNormal();

	void SetInfoHeader(/*COLORREF aBorderColor,COLORREF aBackColor*/);


	// $_FUNCTION_BEGIN ******************************
	// 函数名称：  OnSetText
	// 函数参数：  wParam, lParam  消息参数
	// 返 回 值：  LRESULT
	// 函数说明：  截获设置文本消息WM_SETTEXT事件
	// $_FUNCTION_END ********************************
	LRESULT OnSetText(WPARAM wParam, LPARAM lParam);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：  SetTextColor
	// 函数参数：  acolTextColor  颜色
	// 返 回 值：  无
	// 函数说明： 设置文本颜色
	// $_FUNCTION_END ********************************
	void SetTextColor(COLORREF acolTextColor);

	// $_FUNCTION_BEGIN ******************************
	// 函数名称：  SetTextColor
	// 函数参数：  acolTextColor  颜色
	// 返 回 值：  无
	// 函数说明： 设置文本颜色
	// $_FUNCTION_END ********************************
	

	// $_FUNCTION_BEGIN ******************************
	// 函数名称：  SetTextColor
	// 函数参数：  acolTextColor  颜色
	// 返 回 值：  无
	// 函数说明： 设置文本颜色
	// $_FUNCTION_END ********************************
	void SetFontBold(LONG alBold = FW_BOLD);



	// $_FUNCTION_BEGIN ******************************
	// 函数名称：  SetTextTop
	// 函数参数：  aiValue  文本的上边距
	// 返 回 值：  无
	// 函数说明： 设置文本上边距
	// $_FUNCTION_END ********************************
	void SetTextTop(int aiValue );

	BOOL SetLinkAddress(CString linkaddress);
	BOOL EnableWindow(BOOL aValue = TRUE);

	void SetTextFormat(UINT anFormat);
	void SetTextWidth(UINT anWidth);
	void SetTextSize(int aiFontSize);
	void SetTextSet( UINT anTextSet );
	void SetFontName( CString fontName){m_fontName = fontName;}
};
#endif