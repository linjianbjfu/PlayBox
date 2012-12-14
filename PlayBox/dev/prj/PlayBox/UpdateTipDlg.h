#pragma once

#include "resource.h"
#include "src/gui/CommonControl/HyperLinkCtrl.h"

class CHyperLinkExpand : public CHyperLink
{
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
};

// CUpdateTipDlg dialog

class CUpdateTipDlg : public CDialog
{
	DECLARE_DYNAMIC(CUpdateTipDlg)

public:
	CUpdateTipDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUpdateTipDlg();

	void SetForceUpdate(int nForce);

// Dialog Data
	enum { IDD = IDD_DLG_UPDATE_TIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	int m_nForceUpdate;

	CString m_strUpdateInfo;
	CHyperLinkExpand m_linkUpdateInfo;
public:
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnViewUpdateInfo(WPARAM wParam, LPARAM lParam);
};
