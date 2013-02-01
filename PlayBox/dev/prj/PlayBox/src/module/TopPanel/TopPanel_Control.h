#ifndef _TOP_PANEL_CONTROL_H
#define _TOP_PANEL_CONTROL_H

class CTopPanelWnd;
class CTopPanelControl	
{
private:
	CTopPanelControl();
	~CTopPanelControl();
public:

	static CTopPanelControl*	GetInstance();
	static CTopPanelControl*	m_pTopPanelControl;
	static void					DeleteInstance();

	void				SetTopPanel(CTopPanelWnd* pWnd){m_pWndTopPanel = pWnd;};
	void				ShowMenu(CShowMenu **ppShowMenu, CPoint p);
	void				ShowMenu(CPoint p);
	void				DoMin();
	void				GetAllSkinName( vector<string>& vSkinName );
	void				HoldWindow();
	BOOL				IsHold();

	void				ClickButton(int pos);
	void				AutoRunHideMainWindow();
private:
	CTopPanelWnd*		m_pWndTopPanel;	
};


#endif