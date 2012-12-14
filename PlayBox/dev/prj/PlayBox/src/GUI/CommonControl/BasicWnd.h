#pragma once

class CBasicWnd : public CWnd
{
	DECLARE_DYNAMIC(CBasicWnd)

public:
	CBasicWnd();
	virtual ~CBasicWnd();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void	OnPaint();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
};
