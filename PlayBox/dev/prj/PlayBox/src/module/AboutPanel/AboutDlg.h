#pragma once
#include "afxwin.h"
#include "resource.h"
#include "../../gui/CommonControl/xSkinButton.h"

class CAboutDlg : public CDialog
{
	DECLARE_DYNAMIC(CAboutDlg)
public:
	CAboutDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAboutDlg();

// 对话框数据
	enum { IDD = IDD_DLG_ABOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
	afx_msg void OnClickedOK();
	afx_msg void OnClickedClose();
	DECLARE_MESSAGE_MAP()
private:
	CxSkinButton m_btnOK;
	CxSkinButton m_btnClose;
	//CDibBitmap * m_pBmpBkAbout;
public:
	afx_msg void OnPaint();
	afx_msg LRESULT OnNcHitTest(CPoint point);
};
