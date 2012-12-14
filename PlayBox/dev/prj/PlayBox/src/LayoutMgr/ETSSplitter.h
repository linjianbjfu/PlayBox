#pragma once

// Splitter Styles
#define SS_VERT			0x00000001	// vertical splitter
#define SS_HORIZ  		0x00000002	// horizontal splitter

#include "etslayout.h"
#include "Skin.h"
#include "../gui/Bitmap/DibBitmap.h"
#include "ILayoutMgr.h"

class CDibBitmap;
class CSkinMgr;

class  ETSSplitter : public CStatic
{
// Construction
	DECLARE_DYNAMIC(ETSSplitter)
public:
	ETSSplitter(LPCTSTR strName = NULL);   // standard constructor
	virtual ~ETSSplitter();
	//Pane*	m_Pane;

	//virtual void	LoadSkin(CSkinMgr *skin);
	bool		m_bHorizSplitter;
	bool		m_bMouseButtonDownOnTheBar;
	int			m_nMouseOrgPosition;
	int			m_nMouseOffset;
	bool		m_bHideCloseBtn;
	bool		m_bSlim;
	void		SetSlim(bool bSlim);
	//void		SetRootPanel(ETSLayoutFrameWnd * pRoot){m_rootPanel = pRoot;};
	void		SetILayoutMgr(ILayoutMgr * pLayout){m_rootPanel = pLayout;};
	void		SetBitmap(CDibBitmap * vert, CDibBitmap * horz, CDibBitmap *right,
							CDibBitmap *left, CDibBitmap *up, CDibBitmap * down,
							CDibBitmap *right_over, CDibBitmap *left_over, CDibBitmap *up_over, CDibBitmap * down_over);

	//int			m_nSplAbsolutePos;
	virtual void   CloseSplitter();
	bool   IsClosed();
	CDibBitmap	*m_bSplitterVert;
	CDibBitmap	*m_bSplitterHorz;
	CDibBitmap	*m_bSpliterBarRight;
	CDibBitmap	*m_bSpliterBarLeft;
	CDibBitmap	*m_bSpliterBarUp;
	CDibBitmap	*m_bSpliterBarDown;
	CDibBitmap	*m_bSpliterSlimVert;
	CDibBitmap	*m_bSpliterSlimHorz;

	CDibBitmap	*m_bSpliterBarRightOver;
	CDibBitmap	*m_bSpliterBarLeftOver;
	CDibBitmap	*m_bSpliterBarUpOver;
	CDibBitmap	*m_bSpliterBarDownOver;


	CDibBitmap *	m_bVert;
	CDibBitmap *	m_bHorz;
	CDibBitmap *	m_bBarRight;
	CDibBitmap *	m_bBarLeft;
	CDibBitmap *	m_bBarUp;
	CDibBitmap *	m_bBarDown;

	CSkinMgr*	m_pSkin;
	ETSLayoutMgr::PaneSplitter *m_pane;
	ILayoutMgr *m_rootPanel;

	BOOL IsMouseInCenterBar(CPoint point);
	int  GetSplSize();
private:
	BOOL bTrackLeave;
	void ChangeBarBtn(BOOL bOver);
	CString m_skinPath;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ETSLayoutFrameWnd)
	//}}AFX_VIRTUAL
protected:
	virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);
	virtual void	DrawVertBar(int offset);
	virtual void	DrawHorizBar(int offset);
	virtual void 	ConstrainOffset();
// Implementation
protected:

	//{{AFX_MSG(CSSplitter)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nID);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP() 
};

class ETSOuterSplitter : public ETSSplitter
{
protected:
	void 	ConstrainOffset();
	int		m_nMinConstrainHorz;
public:
	void	SetMainWndConstrain(int& sizeX, int& sizeY, LPRECT lpRect, bool bRedraw);
	void	CloseSplitter();
	//void	LoadSkin(CSkinMgr *skin);
};
