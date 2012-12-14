#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"

class CShowMenu;

class CBottomPanelWnd : public CBasicWnd
{
	DECLARE_DYNAMIC(CBottomPanelWnd)
public:
	CBottomPanelWnd();
	virtual ~CBottomPanelWnd();
	void	LoadSkin();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void	OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
};