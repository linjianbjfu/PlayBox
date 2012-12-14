#pragma once
#include <list>

enum PNG_POSITION
{
	LEFT_TOP_CORNER,
	LEFT_BOTTOM_CORNER,
	RIGHT_TOP_CORNER,
	RIGHT_BOTTOM_CORNER
};

class CTranslucentDialog : public CDialog
{
public:
	PNG_POSITION	m_position;
	CTranslucentDialog(UINT nIDTemplate, LPCTSTR lpszFile, CWnd* pParent = NULL);
	virtual ~CTranslucentDialog();

	void SetEnableDrag(bool bEnableDrag);
	void SetCenterAligned(bool bCenterAligned);
	POINT	m_ptPos;
	int		m_nCx;
	int		m_nCy;

	void UpdateView(BOOL bMustUpdate=false);
	void UpdatePosition( POINT p);
	int GetImageHeight();
	int GetImageWidth();
	Gdiplus::Image* m_pImage;
	Gdiplus::Image* m_pLastImage;

	void DeleteImage()
	{
		if( m_pImage != NULL)
			delete m_pImage; 
		m_pImage = NULL;
	};
// й╣ож
protected:
	BLENDFUNCTION m_blend;
	bool	m_bEnableDrag;
	bool	m_bCenterAligned;

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
private:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int	 OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
