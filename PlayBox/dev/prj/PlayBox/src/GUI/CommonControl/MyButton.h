#pragma once


// CMyButton

class CMyButton : public CButton
{
	DECLARE_DYNAMIC(CMyButton)

public:
	CMyButton();
	virtual ~CMyButton();

protected:
	DECLARE_MESSAGE_MAP()

	void Draw();
	void DrawPreview(vector<COLORREF> &src,BOOL bGradient);
	void DrawPreview(string &str);
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	void Update(vector<COLORREF> &src,BOOL bGradient);
	void Update(string &str);

private:
	CRect m_targetRect;
	vector<COLORREF> m_colorSrc;
	BOOL m_bGradient;
	string m_imagePath;
	int   m_nType;
};


