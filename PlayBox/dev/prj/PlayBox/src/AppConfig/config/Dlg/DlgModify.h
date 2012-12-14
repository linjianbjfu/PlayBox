#pragma once
#include "resource.h"
#include "../../../Global/GlobalVariable.h"

class CModifyDlg : public CDialog
{
	DECLARE_DYNAMIC(CModifyDlg)

public:
	CModifyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CModifyDlg();
	void	SetPlayingData( TAB_ITEM &ti );

	void	OnOK();
	void	OnCancel();
// 对话框数据
	enum { IDD = IDD_MODIFY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	CButton m_ckError1;
	CButton m_ckError2;
	CButton m_ckError3;
	CButton m_ckError4;
	CString strSelectResult;
	CString strTitle;
	CString strOtherReason;
	CEdit m_strOtherReason;
	CString m_strCkeckCode;
	CString m_strRandowWords;
	bool m_isSubmit;

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
private:
	TAB_ITEM m_ti;
	CFont	m_fName;
public:
	afx_msg void OnEnSetfocusEditCheckWords();
};
