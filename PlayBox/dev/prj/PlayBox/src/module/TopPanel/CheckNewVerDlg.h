#pragma once
#include "stdafx.h"
#include "resource.h"
#include "../../gui/CommonControl/HyperLinkCtrl.h"
// CCheckNewVerDlg dialog

#define WM_CHECK_RESULT  WM_USER + 11223

class CHyperLinkClose : public CHyperLink
{
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
};

class CCheckNewVerDlg : public CDialog
{
	DECLARE_DYNAMIC(CCheckNewVerDlg)

public:
	CCheckNewVerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCheckNewVerDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_CHECK_NEW_VER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	string m_strDownUrl;
	CHyperLinkClose m_linkNewVer;
public:
	afx_msg LRESULT OnCheckResult(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedOk();
};