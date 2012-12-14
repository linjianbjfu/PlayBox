#pragma once


class CDibBitmap;
class CMyListCtrl;

class CMyHeaderCtrl : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CMyHeaderCtrl)

public:
	CMyHeaderCtrl();
	virtual ~CMyHeaderCtrl();

	CDibBitmap*	m_pBitmapBk;
	CDibBitmap*	m_pBitmapMidLine;
	CDibBitmap*	m_pBitmapAsc;
	CDibBitmap*	m_pBitmapBkOver;
	CDibBitmap*	m_pBitmapMidLineOver;
	CDibBitmap*	m_pBitmapBkDown;
	CDibBitmap* m_pBitmapDesc;
	int			m_nHoverItem;
	bool		m_bTrackMouse;
	bool		m_bLButtonDown;

	CFont		m_fHeaderFont;
	COLORREF	m_fontColor;
	
	CMyListCtrl* m_pList;
	
	void	SetHeaderColor(COLORREF colorfont );
	void	SetBitmap( CDibBitmap* pBk,CDibBitmap* pMidLine,CDibBitmap* pAsc,CDibBitmap* pDesc);

	virtual BOOL SubclassWindow(HWND hwnd);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg int	 OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LPARAM  OnMouseLeave(WPARAM wp, LPARAM lp);
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnLayOut (WPARAM wParam, LPARAM lParam);
};


