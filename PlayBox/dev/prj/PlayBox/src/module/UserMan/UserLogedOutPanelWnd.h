#pragma once
#include "../../gui/CommonControl/BasicWnd.h"
#include "IUserMsgObserver.h"
#include "xStaticText.h"

class CxSkinButton;

class CUserLogedOutWnd : public CBasicWnd, 
						 public IUserMsgObserver
{
	DECLARE_DYNAMIC(CUserLogedOutWnd)
public:
	CUserLogedOutWnd();
	virtual ~CUserLogedOutWnd();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnClickedLogin();
	afx_msg void	OnClickedRegister();
	afx_msg void	OnTimer(UINT_PTR nIDEvent);
private:
	void UserMsg_Login();
	void UserMsg_LogOut();
	void UserMsg_LogFaild();
	void UserMsg_BeginLogin();

	CxSkinButton*	m_pBtnLogin;
	CxSkinButton*	m_pBtnRegister;
	CxStaticText	m_staticError;
};