#pragma once
#include "afxwin.h"
#include "resource.h"
#include "../../gui/CommonControl/xSkinButton.h"

class CDibBitmap;

class CDlgLogin : public CDialog
{
	DECLARE_DYNAMIC(CDlgLogin)

public:
	CDlgLogin(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLogin();

// 对话框数据
	enum { IDD = IDD_DLG_LOGIN };

protected:
	BOOL OnInitDialog();
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
private:
	CFont m_editFont;
	CxSkinButton m_btnClose;
	CEdit m_editUserName;
	CEdit m_editPassWord;
	CxSkinButton m_btnLoginReg;
	CxSkinButton m_btnForgetPass;
	CxSkinButton m_btnRememberPassCheck;
	CxSkinButton m_btnRememberPassUnCheck;
	CxSkinButton m_btnAutoLoginCheck;
	CxSkinButton m_btnAutoLoginUnCheck;
	CxSkinButton m_btnLogin;
	CxSkinButton m_btnCancel;
	bool m_bRememberPassChecked;
	bool m_bAutoLoginChecked;
	CDibBitmap*  m_bkg;
	
	afx_msg void OnCloseClicked();
	afx_msg void OnRegClicked();
	afx_msg void OnForgetPassClicked();

	afx_msg void OnRememberPassCheckClicked();
	afx_msg void OnRememberPassUnCheckClicked();
	afx_msg void OnAutoLoginCheckClicked();
	afx_msg void OnAutoLoginUnCheckClicked();
	afx_msg void OnLoginClicked();
	afx_msg void OnCancelClicked();
private:
	void InitEditControl(CEdit* pEdit, UINT nID);
	void ValidateCheckBox();
};
