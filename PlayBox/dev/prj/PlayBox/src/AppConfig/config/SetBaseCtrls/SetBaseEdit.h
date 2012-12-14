#pragma once

class SetBaseEdit : public CEdit
{
	DECLARE_DYNAMIC(SetBaseEdit)

public:
	SetBaseEdit();
	virtual ~SetBaseEdit();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH CtlColor( CDC* pDC, UINT nCtlColor );
	afx_msg void OnChange();
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );
};


