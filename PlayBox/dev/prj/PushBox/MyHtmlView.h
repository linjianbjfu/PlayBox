#pragma once

#include "PushBoxDlg.h"

// CMyHtmlView html view

class CMyHtmlView : public CHtmlView
{
	DECLARE_DYNCREATE(CMyHtmlView)
	DECLARE_DISPATCH_MAP()

protected:
	CMyHtmlView();           // protected constructor used by dynamic creation
	virtual ~CMyHtmlView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	enum {IDD = IDD_HTML_VIEW};
	//Declare a pointer for the parent's dialog.
	CPushBoxDlg *m_pPushViewDlg;
	virtual void OnInitialUpdate();
	afx_msg void OnPaint();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel);
	BOOL btnHideBox(void);
	HRESULT OnGetExternal( LPDISPATCH *);
protected:
	virtual void OnDraw(CDC* /*pDC*/);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


