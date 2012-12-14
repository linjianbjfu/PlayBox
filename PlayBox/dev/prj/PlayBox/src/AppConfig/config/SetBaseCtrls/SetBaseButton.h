#pragma once


// SetBaseButton

class SetBaseButton : public CButton
{
	DECLARE_DYNAMIC(SetBaseButton)

public:
	SetBaseButton();
	virtual ~SetBaseButton();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH CtlColor( CDC* pDC, UINT nCtlColor );
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );
};


