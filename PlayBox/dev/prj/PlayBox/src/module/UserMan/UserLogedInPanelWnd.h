#pragma once
#include "../../gui/CommonControl/BasicWnd.h"

class CUserLogedInWnd : public CBasicWnd
{
	DECLARE_DYNAMIC(CUserLogedInWnd)
public:
	CUserLogedInWnd();
	virtual ~CUserLogedInWnd();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
private:
	
public:
	afx_msg void OnPaint();
};