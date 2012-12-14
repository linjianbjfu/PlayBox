#pragma once
#include "afxwin.h"
#include "xSkinButton.h"
#include "resource.H"

#define IDC_MSGD_BTN_CLOSE	10
#define IDC_MSGD_BTN_1		20
#define IDC_MSGD_BTN_2		30
#define IDC_MSGD_BTN_CHECK	40
#define IDC_MSGD_BTN_UNCHECK 50
// CDialogMsg dialog

class CDialogMsg : public CDialog
{
	DECLARE_DYNAMIC(CDialogMsg)

public:
	CDialogMsg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDialogMsg();

// Dialog Data
	enum { IDD = IDD_MSG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:

	void	SetTitle(const CString& strTitle ){m_strTitle = strTitle;}
	void	SetSize(int iXSize,int iYSize){m_iXSize = iXSize;m_iYSize=iYSize;}

	void	SetBtnCount(int iBtnCount);
	void	SetBtnText(int iBtnIdx,const CString& strText);
	void	SetMsg(const CString& strMsg){m_strMsg = strMsg;}

	void	ShowCheckBox(BOOL bShow);
	void	SetCheckBoxText(const CString& str);

protected:
	CString	m_strTitle;
	int		m_iXSize;
	int		m_iYSize;
	int		m_iBtnCount;

	CString	m_strBtnText1;
	CString m_strBtnText2;
	CString	m_strMsg;

	CRect	m_titleRect;
	CRect	m_msgRect;
	CRect   m_NoMoreHintRect;

	CFont	m_titleFont;
	CFont	m_msgFont;

	BOOL	m_bShowCheckBox;
	CString	m_strCheckBox;
public:
	CxSkinButton m_btnClose;
	CxSkinButton m_btn1;
	CxSkinButton m_btn2;

	CxSkinButton m_btnCheck;
	CxSkinButton m_btnUnCheck;	

	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnBnClickedBtn1();
	afx_msg void OnBnClickedBtn2();
	afx_msg void OnBnClickedCheck();
	afx_msg void OnBnClickedUnCheck();

public:

	virtual void OnClickBtn1();
	virtual void OnClickBtn2();
	virtual void OnClickClose();

	virtual void CheckBox_Check(){};

	virtual void PaintSomething(CDC* pDC );

	virtual void CheckBox_Checked(){};
	virtual void CheckBox_UnChecked(){};
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
