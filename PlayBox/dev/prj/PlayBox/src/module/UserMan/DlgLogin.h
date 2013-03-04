#pragma once
#include "afxwin.h"
#include "resource.h"
#include "../../gui/CommonControl/xSkinButton.h"
#include "EditGlow.h"
#include "IUserMsgObserver.h"
#include "xStaticText.h"

class CDibBitmap;

class CDlgLogin : public CDialog, public IUserMsgObserver
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
	CEditGlow m_editUserName;
	CEditGlow m_editPassWord;
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
	bool m_bShowErrStatic;
	CDibBitmap*  m_bkg;
	CxStaticText m_staticError;
	
	afx_msg void OnCloseClicked();
	afx_msg void OnRegClicked();
	afx_msg void OnForgetPassClicked();
	afx_msg void OnRememberPassCheckClicked();
	afx_msg void OnRememberPassUnCheckClicked();
	afx_msg void OnAutoLoginCheckClicked();
	afx_msg void OnAutoLoginUnCheckClicked();
	afx_msg void OnLoginClicked();
	afx_msg void OnCancelClicked();
	afx_msg void OnUserNameEditSetFocus();
	afx_msg void OnPasswordEditSetFocus();
	afx_msg UINT OnNcHitTest(CPoint point);
	void OnOK();
	void ValidateCheckBoxOrShowFailText();
	void ShowErrStaticAndLaterDisappear(LPCTSTR lpszText);
	void HideErrStatic();
	void InitEditControl(CEditGlow* pEdit, 
		LPCSTR lpszPicPath, LPCSTR lpszGlowPicPath);
	void UserMsg_Login();
	void UserMsg_LogOut();
	void UserMsg_LogFaild();
	void UserMsg_BeginLogin();
	void WriteConf();
public:
	void AddTask(const TAB_ITEM& ti);
private:
	TAB_ITEM m_task;
	LPCSTR m_lpszPassPlaceHolder;
};