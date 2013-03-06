#pragma once
#include "../../gui/CommonControl/BasicWnd.h"

class CxSkinButton;

class CGameCenterWelcomeWnd : public CBasicWnd
{
	DECLARE_DYNAMIC(CGameCenterWelcomeWnd)
public:
	CGameCenterWelcomeWnd();
	virtual ~CGameCenterWelcomeWnd();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int	 OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnClickedGotoGameCenter();
private:
	CxSkinButton*	m_pBtnGoToGameCenter;
};