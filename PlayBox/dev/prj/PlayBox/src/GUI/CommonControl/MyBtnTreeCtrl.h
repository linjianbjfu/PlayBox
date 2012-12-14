#pragma once

#include "MyTreeCtrl.h"
#include "../util/WToolTips.h"
#include "../Bitmap/DibBitmap.h"

// CMyBtnTreeCtrl

class CMyBtnTreeCtrl : public CMyTreeCtrl
{
	DECLARE_DYNAMIC(CMyBtnTreeCtrl)

public:
	CMyBtnTreeCtrl();

	virtual ~CMyBtnTreeCtrl();

	void	SetButtonBitmap( CDibBitmap*,CDibBitmap*,CDibBitmap*);
	void	DrawImage( CDibBitmap* pDibBitmap, CRect &r, CDC & memDC );

protected:
	CDibBitmap*	m_pBitmapOver;
	CDibBitmap*	m_pBitmapNormal;
	CDibBitmap*	m_pBitmapDown;


	CRect	m_BtnRect;
	bool	m_bMouseOverBtn;
	bool	m_bTrackMouse;
	bool	m_bHideBtn;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void	OnCustomdraw ( NMHDR* pNMHDR, LRESULT* pResult ) ;
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);	
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	
	BOOL			OnEraseBkgnd(CDC* pDC);
};