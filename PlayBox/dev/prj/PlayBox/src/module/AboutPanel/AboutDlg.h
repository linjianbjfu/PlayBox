#pragma once
#include "afxwin.h"
#include "resource.h"
#include "../../gui/CommonControl/xSkinButton.h"

class CAboutDlg : public CDialog
{
	DECLARE_DYNAMIC(CAboutDlg)
public:
	CAboutDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CAboutDlg();

// �Ի�������
	enum { IDD = IDD_DLG_ABOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
	afx_msg void OnClickedOK();
	DECLARE_MESSAGE_MAP()
private:
	CxSkinButton m_btnOK;
};
