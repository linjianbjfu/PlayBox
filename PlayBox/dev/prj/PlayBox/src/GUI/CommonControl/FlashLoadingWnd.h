#pragma once
#include "../../gui/CommonControl/shockwaveflash.h"

class CFlashWnd : public CWnd
{
	DECLARE_DYNAMIC(CFlashWnd)

public:
	CFlashWnd();
	virtual ~CFlashWnd();

	afx_msg	int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnPaint();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
private:
	COLORREF	m_colBk;	//netload±³¾°É«
	CRect		m_rcFlash;
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	CShockwaveFlash m_flash;
};
