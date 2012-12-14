//***************************************************************************
// Copyright 1999                                  
//***************************************************************************
// @Author: Christopher Brown
// @E-Mail: AllKnowing@EarthLink.net
// @ICQ:	875098
// @WEB:	http://home.earthlink.net/~allknowing
//
// @Cedits:
//
//	  Giving credit where credit is due...  Thanks for the help!!!
//    -----------------------------------------------------------
//		  Iuri Apollonio - warch@dnet.it
//		  Michael Dunn   - mdunn@inreach.com
//		  Dave Buches	 - ??
//
//		  Anyone else?
//		  Sorry if I forgot to include your name... 
//    -----------------------------------------------------------
//
// @Blah, blah, blah...
//
// THIS CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.
// You may use and distribute this code provided that you do not
// remove this title header and that you do not charge money for it.  
//
//***************************************************************************
#if !defined(AFX_WNDSLIDER_H__90B1F5F3_B5AC_11D2_877B_0000C042C40D__INCLUDED_)
#define AFX_WNDSLIDER_H__90B1F5F3_B5AC_11D2_877B_0000C042C40D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//***************************************************************************
// WndSlider.h : header file
//***************************************************************************

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif
//#include "Resource.h"
//#include "musicsourceskin.H"
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
#define SOB_ERROR			0xffffffff // == -1
#define WM_QQSHIFT			WM_USER + 203
#define WM_SHOW_CONTENTWND	WM_USER + 10

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
const int SOB_BAR_DEFAULT_FIRST_VIEW_ID	 = 0xDEAD;

/////////////////////////////////////////////////////////////////////////////
// Increase or decrease value to adjust button height.
/////////////////////////////////////////////////////////////////////////////
const int SOB_BAR_DEFAULT_BTN_HEIGHT	 = 22;

/////////////////////////////////////////////////////////////////////////////
// Increase this value to slow down the animation.  
/////////////////////////////////////////////////////////////////////////////
const int SOB_BAR_DEFAULT_ANIMATION_SPEED =	10;

/////////////////////////////////////////////////////////////////////////////
// Increase or decrease this value to to limit the amount of time spent 
// animating...
/////////////////////////////////////////////////////////////////////////////
const int SOB_BAR_DEFAULT_ANIMATION_LIMIT =	200;


/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
const DWORD SOB_VIEW_HORZ		=	0x00000001;
const DWORD SOB_VIEW_VERT		=	0x00000002;
const DWORD SOB_VIEW_ANIMATE	=	0x00000004;

const DWORD SOB_BTN_ANIMATE		=	0x00001000;
const DWORD SOB_BTN_FANCYFACE	=   0x00002000;


/////////////////////////////////////////////////////////////////////////////
// Constance used by CWndSliderView & CSliderButton.  
/////////////////////////////////////////////////////////////////////////////
typedef const enum tagButtonState
{ 
	SOB_BTN_UP   = 0x00000010,
	SOB_BTN_HIT  = 0x00000020, 
	SOB_BTN_OVER = 0x00000040 

} BUTTON_STATE, * LPBUTTON_STATE;

class CDibBitmap;

struct WND_SLIDER_DATA
{
	WND_SLIDER_DATA(): pBitmapLeftNormal(NULL),
					   pBitmapLeftOver(NULL),
					   pBitmapLeftDown(NULL),
					   pBitmapMidNormal(NULL),
					   pBitmapMidOver(NULL),
					   pBitmapMidDown(NULL),
					   pBitmapRightNormal(NULL),
					   pBitmapRightOver(NULL),
					   pBitmapRightDown(NULL),
					   pBitmapIcon(NULL),
					   pBitmapIconRight(NULL),
					   strTitle(""),
					   pFont(NULL),
					   pWnd(NULL),
					   colorFont(RGB(0,0,0)),
					   bNew(false)
	{
		pFont = new CFont();
		HFONT hFont =  (HFONT)::GetStockObject(DEFAULT_GUI_FONT); 
		LOGFONT lf = { 0 }; 
		if(::GetObject(hFont, sizeof(LOGFONT), &lf) == sizeof(LOGFONT)) 
		{ 
			lf.lfWeight = FW_NORMAL/*FW_BOLD*/; 
			pFont->CreateFontIndirect(&lf);
		} 		
	}
	~WND_SLIDER_DATA()
	{
		pFont->DeleteObject();
		delete pFont;
	}

	CDibBitmap* pBitmapIcon;
	CDibBitmap* pBitmapIconRight;

	CDibBitmap*	pBitmapLeftNormal;
	CDibBitmap*	pBitmapLeftOver;
	CDibBitmap*	pBitmapLeftDown;

	CDibBitmap* pBitmapMidNormal;
	CDibBitmap* pBitmapMidOver;
	CDibBitmap* pBitmapMidDown;

	CDibBitmap*	pBitmapRightNormal;
	CDibBitmap*	pBitmapRightOver;
	CDibBitmap*	pBitmapRightDown;
		
	CString		strTitle;
	CFont*		pFont;
	COLORREF	colorFont;
	CWnd*		pWnd;
	bool		bNew;
};


//***************************************************************************
//
//***************************************************************************
class CSliderButton : public CObject
{
	friend class CWndSliderView;
	DWORD		m_dwButtonStyle;
	CWnd*		m_pWndParent;
	CString		m_strButtonTitle;
	CRect		m_rectButton;
	CDibBitmap*	m_pBitmapLeftNormal;
	CDibBitmap*	m_pBitmapLeftOver;
	CDibBitmap*	m_pBitmapLeftDown;
	CDibBitmap* m_pBitmapMidNormal;
	CDibBitmap* m_pBitmapMidOver;
	CDibBitmap* m_pBitmapMidDown;
	CDibBitmap*	m_pBitmapRightNormal;
	CDibBitmap*	m_pBitmapRightOver;
	CDibBitmap*	m_pBitmapRightDown;
	CDibBitmap* m_pBitmapIcon;
	CDibBitmap* m_pBitmapIconRight;

	CFont		m_cFont;
	COLORREF	m_cFontColor;
	bool		m_bNew;
public:
	
	CSliderButton();
	~CSliderButton(); 

	CWnd*	GetParentWnd( void );
	void	SetParentWnd( CWnd* pWnd );
	  
	DWORD	GetButtonStyle( void ) const;
	void	ModifyButtonStyle( const DWORD dwRemove, const DWORD dwAdd );

	void	SetButtonTitle( LPCSTR szcTitle );
	LPCSTR	GetButtonTitle( void ) const;

	void	SetBitmap(  CDibBitmap* pLeftNormal, CDibBitmap* pLeftOver, CDibBitmap* pLeftDown,
						CDibBitmap* pMidNormal, CDibBitmap* pMidOver,CDibBitmap* pMidDown,
						CDibBitmap* pRightNormal, CDibBitmap* pRightOver, CDibBitmap* pRightDown,
						CDibBitmap* pIcon, CDibBitmap* pIconRight);
	void	SetButtonFont( CFont* pfont,COLORREF color);
protected:

	void Draw( CDC* pDC, CRect& rectScreen, const DWORD dwFlags = SS_CENTER , int x = false);
	void Size( const CRect& rectNewSize, const DWORD dwStyle );

};

//***************************************************************************
typedef CArray< CSliderButton*, CSliderButton* > CSliderButtonArray; 


class CWndSliderView : public CWnd
{
protected:

	DECLARE_DYNCREATE(CWndSliderView)

public:
	CWndSliderView();

public:

	int				m_nButtonHeight;
	CWnd*			m_pWndParent;

protected:

	CSliderButtonArray  m_buttonArray;
	DWORD			m_dwViewStyle;

	int				m_nViewCount;
	int				m_nActiveView;

	int				m_nMouseOnButton;
	BOOL			m_bLButtonDown;

	BOOL			m_bTrackLeave;
	HCURSOR			m_hHandCursor;

private:
	int				m_nPriviousButton;
	BOOL			m_bTrackingMouse;

// Operations
public:

	void	SetButtonTitle( int nView, CString title );
	void	GetButtonTitle( int nView,CString& title );
	int		CreateView( const WND_SLIDER_DATA& );
	void	RecalcLayout( const BOOL bRepaint = TRUE );
	CWnd*	GetView( const int nView ) const;
	BOOL	RemoveView( const int nView );
	BOOL	RemoveView(const CWnd *pWnd);
	int		GetViewPos(const CWnd*);
	int		GetViewCount() const;
	int		ButtonHitTest( const CPoint& point );
	CSliderButton* GetButton( const int nButton ) const;
	int		GetActiveView( void ) const;
	int		SetActiveView( const int nNewView, const BOOL bRedraw = TRUE );
	void	MySetActiveView(int nNiew);
	CRect	GetButtonRect( const int nButton, const CRect& rectClient ) const;
	CRect	GetButtonRect( const int nButton ) const;
	CRect	GetInsideRect( const int nButton ) const;
	void	InvalidateButton( const int nButton );
	void	LoadSkin(const WND_SLIDER_DATA& wndSliderData,const string& );
protected:

	void	OnPrintClient( HDC hDC, UINT uFlags );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWndSliderView)
	public:
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWndSliderView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWndSliderView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSysColorChange();
	afx_msg LRESULT OnShowContentWnd(WPARAM,LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//***************************************************************************

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WNDSLIDER_H__90B1F5F3_B5AC_11D2_877B_0000C042C40D__INCLUDED_)
