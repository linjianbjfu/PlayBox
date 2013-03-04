#pragma once

class CxSkinButton;
class MyWebBrowserWnd;

class CWebDlg : public CDialog
{
	DECLARE_DYNAMIC(CWebDlg)

	enum{IDD = IDD_DLG_CLEANCHACHE};
public:
	CWebDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CWebDlg();
protected:
	CxSkinButton*	m_pBtnClose;
	CString			m_strTitle;
	CString			m_strUrl;
	MyWebBrowserWnd*		m_pWebBrowser;
	SIZE			m_sizeWnd;

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClose();
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

public:
	virtual LRESULT DoModal(CString strTitle, CString strUrl, int nPageWidth, int nPageHeight);
	afx_msg UINT OnNcHitTest(CPoint point);
};