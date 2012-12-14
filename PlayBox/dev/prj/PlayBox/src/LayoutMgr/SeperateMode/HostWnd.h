#pragma once
#include "resource.h"
#include <map>
#include "../../gui/CommonControl/xSkinButton.h"

using namespace std;
// CBasicMouseWnd

#define DIRECTION_LEFT		1
#define DIRECTION_RIGHT		2
#define DIRECTION_TOP		4
#define DIRECTION_BOTTOM	8
#ifndef ID_BTN_CLOSE
	#define ID_BTN_CLOSE		9
#endif

class CHostWnd : public CDialog
{
protected:
	int		m_nDeltaHeight;
	int		m_nDeltaWidth;
	BOOL	m_bMovable;
	BOOL	m_isMainWnd;
	HWND	m_hParentWnd;
	CWnd*	m_pWnd;
	int		m_nL;
	int		m_nR;
	int		m_nT;
	int		m_nB;
	int		m_iDirection;
	bool	m_bChangeCursor;
	bool	m_isResizing;
	CPoint	m_ptStartResizing;
	CRect	m_rectStarResizing;
	CRect	m_rcMain;
	string	m_strName;
	string  m_strBackGround;
	CString m_strContent;
	virtual bool	_needChangeCursor(const CPoint& Pt );
	void	DrawMoveFrame(CRect rc);
	void	MoveContentWindow();
public:
	int		m_nMinCx;
	int		m_nMinCy;
	CHostWnd(CWnd* pParent = NULL);
	virtual ~CHostWnd();

	enum { IDD = IDD_HOST };
protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL	OnInitDialog();
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void	OnPaint();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnBnClickedClose();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnBnClickedOk();

private:
	CxSkinButton	m_btnClose;
public:
	void	SetMinConstrant(int nx, int ny){m_nMinCx = nx; m_nMinCy = ny;};
	void	HostWindow(CWnd *pWnd, bool isMainWnd);
	void	DetachWindow();
    void	SetBorder(int l, int r, int t, int b){m_nL = l; m_nR = r;m_nT = t; m_nB = b;};
	void	SetMainWnd(BOOL isMain){m_isMainWnd = isMain;};
	void	SetName(string name){m_strName = name;};
	string	GetName(){return m_strName;};
	void	SetResizable(bool b){m_bMovable = b;};
	bool	IsResizable(){return m_bMovable;};
	void	SetBackGround(CString strBackGround){m_strBackGround = strBackGround;};
	void	SetContent(CString strContent){m_strContent = strContent;};
	CWnd*	GetContentWnd(){return m_pWnd;};

protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
};

