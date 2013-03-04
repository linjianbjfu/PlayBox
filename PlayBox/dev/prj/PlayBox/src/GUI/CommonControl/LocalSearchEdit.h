#pragma once
 
//#include "LocalSearchKeyTipDlg.h"

#define LOCAL_SEARCH_EDIT_TOOLTIP  ""
#define MSG_EDIT_BORDER_CHANGE	WM_USER + 118

class CLocalSearchEdit : public CEdit
{
	DECLARE_DYNAMIC(CLocalSearchEdit)

public:
	CLocalSearchEdit();
	virtual ~CLocalSearchEdit();
private:
	CString               m_curString;
// 	CLocalSearchKeyTipDlg m_tip;
	bool	m_bFocus;    //是否有焦点
	bool	m_bMouseOver;//是否鼠标over

	void ResizeWindow();
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	BOOL    PreTranslateMessage(MSG *pMsg);
	void    OnMainWindowMove();
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnEnKillfocus();
	afx_msg void OnEnSetfocus();
	afx_msg LRESULT OnSearchKey(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LPARAM OnMouseLeave(WPARAM wp, LPARAM lp);

	void OnShowHistory();
	void SetEditBorder();
	void OnAddHistory(LPCTSTR pszHistory);
};
