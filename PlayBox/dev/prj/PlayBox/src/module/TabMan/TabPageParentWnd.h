#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../DataInterface/ILayoutChangeObserver.h"
#include "../../LayoutMgr/ISkinMgr.h"

class CShowMenu;
class IPanelChange;

class CTabPageParentWnd : public CBasicWnd,	 
						  public ILayoutChangeObserver
{
	DECLARE_DYNAMIC(CTabPageParentWnd)
public:
	CTabPageParentWnd();
	virtual ~CTabPageParentWnd();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void	OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL	OnEraseBkgnd(CDC* pDC);
};