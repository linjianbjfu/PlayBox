#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../LayoutMgr/ISkinMgr.h"
#include "xStaticText.h"
#include "Global/GlobalVariable.h"

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
private:
	static DWORD WINAPI ThreadGetRecommandWebGame(void* pPara);
	afx_msg LRESULT	OnBottomWebGameIsOK(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnClickStatic(WPARAM wParam, LPARAM lParam);
	
	string	m_strRecommandWebGameUrl;
	CxStaticText	m_staticWegGame1;
	CxStaticText	m_staticWegGame2;
	TAB_ITEM	m_webGame1;
	TAB_ITEM	m_webGame2;
};