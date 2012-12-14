#pragma once

class CSubLabel : public CStatic
{
public:
	CSubLabel();
	~CSubLabel();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	void SetText(CString str);
	void SetSelect(BOOL bsel);
private:
	CFont   *m_Fontstatic;   
	CString m_textstr;
	BOOL	m_bselect;
};