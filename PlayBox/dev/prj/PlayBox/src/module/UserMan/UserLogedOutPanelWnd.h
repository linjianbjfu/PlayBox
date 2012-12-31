#pragma once
#include "../../gui/CommonControl/BasicWnd.h"

class CxSkinButton;

class CUserLogedOutWnd : public CBasicWnd
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
private:
	CxSkinButton*		m_pBtnLogin;
	CxSkinButton*		m_pBtnRegister;
};