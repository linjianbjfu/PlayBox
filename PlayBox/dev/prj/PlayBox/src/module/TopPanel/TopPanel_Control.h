#ifndef _TOP_PANEL_CONTROL_H
#define _TOP_PANEL_CONTROL_H

#define STR_SKIN_SMALL_PICTURE "\\small.jpg"
#define STR_SKIN_OLD_SMALL_BMP "\\small.bmp"

enum{
	TOPPANEL_NOWPLAYING,
	TOPPANEL_COMMEND,
	TOPPANEL_NETLIB,
	TOPPANEL_DOWNLOAD
};

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