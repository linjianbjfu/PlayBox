#if !defined(AFX_XSTATICTEXT_H__CA6E2F82_11B1_11D5_B6BF_00E07D8144D0__INCLUDED_)
#define AFX_XSTATICTEXT_H__CA6E2F82_11B1_11D5_B6BF_00E07D8144D0__INCLUDED_

#if _MSC_VER > 1000


#include "../Bitmap/DibBitmap.h"
#pragma once
#endif // _MSC_VER > 1000
// xStaticText.h : header file
/* 15/03/2001
 * ing.davide.pizzolato@libero.it
 */
#define WM_STATICCLICKED	WM_USER + 100

#define SST_NORMAL	0
#define SST_SUNKEN	1
#define SST_RAISED	2

/////////////////////////////////////////////////////////////////////////////
// CxStaticText window
class CxStaticText : public CStatic
{
// Construction
public:
	CxStaticText();
// Attributes
public:
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CxStaticText)
	//}}AFX_VIRTUAL
// Implementation
	virtual ~CxStaticText();
	void SetWindowText( LPCTSTR lpszString );
	//void SetTextStyle(int style);
	void SetSkin(CDibBitmap* abmpNormal ,CDibBitmap* abmpMask=NULL, short aiDrawmode =1 );
	void SetBlink(BOOL bBlink=TRUE, int blink_time=333, int blink_count=0);
	COLORREF SetTextColor(COLORREF new_color);
	COLORREF SetBlinkColor(COLORREF new_color);
	COLORREF SetBackgroundColor(COLORREF new_color);

	void SetSingleLine(bool single);
	void SetUnderLine(bool under);
	void SetFontFace(const CString& face);
	void SetFontSize(unsigned int size);
	bool IsEnableClick(){return m_EnableClick;};
	void EnableClick(bool enable);

	CRect GetSingleLineTextRect();
	void SizeToTextRect();

	void EnableToolTip(bool enable);

protected:
	//int m_Style;
	//COLORREF	m_ActualColor;
	int			m_BlinkCounter;
	COLORREF	m_BackgroundColor;
	COLORREF	m_BlinkColor;
	COLORREF	m_TextColor;			//button text color
	COLORREF	m_ActualColor;

	bool		m_SingleLine;
	bool		m_UnderLine;
	bool		m_EnableClick;	

	HCURSOR		m_hCurFinger;

	CString		m_FontFace;
	unsigned int m_FontSize;

	CString		m_strWindowText;

	bool		m_EnableToolTip;

	UINT		m_TimerID;				//Blink Timer ID
	
	short	m_DrawMode;				//0=normal; 1=stretch; 2=tiled;
	CDibBitmap* mpBmpNormal;
	CDibBitmap* mpBmpMask;

	CToolTipCtrl m_tooltip;
	CString m_strToolTip;

	void SetToolTipText(CString s);
	void RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);

	void InvalidateParent();
	HRGN	hClipRgn;	
	HRGN	CreateRgnFromBitmap(CDibBitmap* hBmp, COLORREF color);
	void	DrawBitmap(CDC* dc, CDibBitmap* hbmp, RECT r, int DrawMode);
	int		GetBitmapWidth (CDibBitmap* hBitmap);
	int		GetBitmapHeight (CDibBitmap* hBitmap);
	void	FillWithBitmap(CDC* dc, CDibBitmap* hbmp, RECT r);
	//{{AFX_MSG(CxStaticText)
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual void PreSubclassWindow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnStnClicked();
	//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_XSTATICTEXT_H__CA6E2F82_11B1_11D5_B6BF_00E07D8144D0__INCLUDED_)
