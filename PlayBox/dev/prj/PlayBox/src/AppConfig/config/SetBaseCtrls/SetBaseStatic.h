#pragma once


// SetBaseStatic

class SetBaseStatic : public CStatic
{
	DECLARE_DYNAMIC(SetBaseStatic)

public:
	COLORREF	m_TextColor;
	int			m_FontSize;
	BOOL	m_bTrans;
	BOOL	m_bBold;
	SetBaseStatic();
	virtual ~SetBaseStatic();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH CtlColor( CDC* pDC, UINT nCtlColor );
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );
	afx_msg LRESULT OnSetText(WPARAM wParam,LPARAM lParam);
	afx_msg void	OnPaint();
};


