#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"

class CShowMenu;

class CGameInfoWnd : public CBasicWnd
{
	DECLARE_DYNAMIC(CGameInfoWnd)
public:
	CGameInfoWnd();
	virtual ~CGameInfoWnd();
	void	LoadSkin();
	void	SetGameInfo( string& str );

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void	OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnPaint();
private:
	string	m_strGameInfo;
	CFont	m_font;
		
};