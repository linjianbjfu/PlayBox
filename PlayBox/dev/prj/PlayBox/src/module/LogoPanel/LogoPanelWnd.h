#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"

class CShowMenu;

class CLogoPanelWnd : public CBasicWnd
{
	DECLARE_DYNAMIC(CLogoPanelWnd)
public:
	CLogoPanelWnd();
	virtual ~CLogoPanelWnd();
	void	LoadSkin();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void	OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
};