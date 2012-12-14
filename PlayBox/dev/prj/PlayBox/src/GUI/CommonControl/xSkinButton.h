#if !defined(AFX_MYB_H__3832DDEF_0C12_11D5_B6BE_00E07D8144D0__INCLUDED_)
#define AFX_MYB_H__3832DDEF_0C12_11D5_B6BE_00E07D8144D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CxSkinButton.h : header file
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
#include "../Bitmap/DibBitmap.h"

#include "AfxGlobals.h"
#include "IPanelChange.h"
#include "../../gui/util/WToolTips.h"


#define WM_CXSHADE_RADIO WM_USER+0x100
#define MSG_MOUSELEAVE_BTN  WM_USER+0x101
#define MSG_MOUSEHOVER_BTN  WM_USER+0x102
/////////////////////////////////////////////////////////////////////////////
// CxSkinButton window
class CxSkinButton : public CButton
{
// Construction
public:
	CxSkinButton();

// Attributes
private:
	BOOL m_bRClick;
	CString m_strToolTip;
// Operations
public:
	void SetRightClick(BOOL bRightClick);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CxSkinButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
// Implementation
public:

	virtual BOOL DestroyWindow();
	virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	CString	GetToolTipText();
	void	SetToolTipText(CString s);
	COLORREF SetNormalTextColor(COLORREF normalcolor);
	COLORREF SetDownTextColor(COLORREF downColor);
	COLORREF SetOverTextColor(COLORREF overColor);
	COLORREF SetDisableTextColor(COLORREF disableColor);
	/*void SetSkin(UINT normal,UINT down, UINT over=0, UINT disabled=0, UINT focus=0,UINT mask=0,
				short drawmode=1,short border=1,short margin=4);*/
	void SetSkin(CDibBitmap* normal,CDibBitmap* down, CDibBitmap* over=NULL,
				CDibBitmap* disabled=NULL, CDibBitmap* focus=NULL,CDibBitmap* mask=NULL,
				short drawmode=1,short border=1,short margin=4,bool bSetSizeBySkin=false);
	virtual ~CxSkinButton();
	void	SetMask(bool bMask){m_bSetMask = true;};
	// Generated message map functions
public:

	CCWToolTips*	m_pToolTip;
	CPoint	m_ptToolTipPos;


	DWORD	m_nTextOffsetX;
	DWORD	m_nTextOffsetY;
	bool	m_bSetMask;
	bool	m_Checked;					//radio & check buttons
	DWORD	m_Style;					//radio & check buttons
    bool m_tracking;
    bool m_button_down;
	void RelayEvent(UINT message, WPARAM wParam, LPARAM lParam);
	CToolTipCtrl m_tooltip;
	CDibBitmap* m_bNormal,*m_bDown,*m_bDisabled,*m_bMask,*m_bOver,*m_bFocus; //skin bitmaps
	short	m_FocusRectMargin;		//dotted margin offset
	COLORREF m_normalTextColor;			//button text color
	COLORREF m_downTextColor;
	COLORREF m_OverTextColor;
	COLORREF m_disableTextColor;
	HRGN	hClipRgn;				//clipping region
	BOOL	m_Border;				//0=flat; 1=3D;
	short	m_DrawMode;				//0=normal; 1=stretch; 2=tiled;
	HRGN	CreateRgnFromBitmap(CDibBitmap* hBmp, COLORREF color);
	HRGN	CreateRegionFromDib(CDibBitmap* dib, COLORREF color);
	void	FillWithBitmap(CDC* dc, CDibBitmap* hbmp, RECT r);
	void	DrawBitmap(CDC* dc, CDibBitmap* hbmp, RECT r, int DrawMode);
	int		GetBitmapWidth (CDibBitmap* hBitmap);
	int		GetBitmapHeight (CDibBitmap* hBitmap);
	//{{AFX_MSG(CxSkinButton)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnClicked();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg LRESULT OnMouseHover(WPARAM   wParam,   LPARAM   lParam) ;
	afx_msg LRESULT OnRadioInfo(WPARAM, LPARAM);
	afx_msg LRESULT OnBMSetCheck(WPARAM, LPARAM);
	afx_msg LRESULT OnBMGetCheck(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_MYB_H__3832DDEF_0C12_11D5_B6BE_00E07D8144D0__INCLUDED_)
