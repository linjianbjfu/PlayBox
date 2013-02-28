#pragma once
#include "../../gui/CommonControl/BasicWnd.h"

struct UserInfo;
class CxSkinButton;

class CUserLogedInWnd : public CBasicWnd
{
	DECLARE_DYNAMIC(CUserLogedInWnd)
public:
	CUserLogedInWnd();
	virtual ~CUserLogedInWnd();
	void SetUserInfo(UserInfo* pUserInfo);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int	 OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnClickedLogout();
private:
	CxSkinButton*	m_pBtnLogout;
	UserInfo* m_pUserInfo;
	CFont	m_font;
};