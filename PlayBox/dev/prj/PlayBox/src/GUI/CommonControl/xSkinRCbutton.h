#ifndef ZHP_XSKINRCBUTTON_KOOWO_H
#define ZHP_XSKINRCBUTTON_KOOWO_H

#include "xSkinButton.h"
#include "xStaticText.h"
#include <map>

#define MSG_RCBUTTON_CLICKED	WM_USER+4679


class CxSkinStatic : public CStatic
{
	DECLARE_DYNAMIC(CxSkinStatic)

public:
	CxSkinStatic();
	virtual ~CxSkinStatic();
	void SetSkin(COLORREF &colorText,COLORREF &colorBkg);
	void SetMsgWnd(CWnd *pWnd);
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH CtlColor( CDC* pDC, UINT nCtlColor );
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );
private:
	COLORREF m_colorBkg;
	COLORREF m_colorText;
	HBRUSH m_hbrBkg;
	CWnd *m_pMsgWnd;
public:
	virtual BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};



class CxSkinRCbutton:public CxSkinButton
{
public:
	CxSkinRCbutton();

	virtual ~CxSkinRCbutton();

	void CxSkinRCbutton::SetSkin(CDibBitmap *pBmpCheckNormal,CDibBitmap *pBmpCheckOver,CDibBitmap *pBmpUnCheckNormal,CDibBitmap *pBmpUnCheckOver,CDibBitmap *pBmpDisable,COLORREF colorText=RGB(0,0,0),COLORREF colorTextBkg=RGB(255,255,255));

	void SetWindowText(LPCTSTR lpszString);

	void GetWindowText(CString& rString);

	void SetCheck(BOOL bCheck);

	int GetCheck();

	void SetGroupId(int iGroupId);

	int GetGroupId();

private:
	void MoveText();

	CxSkinStatic m_text;

	CDibBitmap *m_pBmpCheckNormal;

	CDibBitmap *m_pBmpCheckOver;

	CDibBitmap *m_pBmpUnCheckNormal;

	CDibBitmap *m_pBmpUnCheckOver;

	CDibBitmap *m_pBmpDisable;

	static map<CxSkinRCbutton *,int> s_mapGroup;

	int m_iGroupId;

	CFont m_font;

	bool m_bCheck;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM, LPARAM);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

#endif