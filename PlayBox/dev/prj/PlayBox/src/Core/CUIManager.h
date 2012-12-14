#ifndef _C_UI_MANAGER_H
#define _C_UI_MANAGER_H

#include "IUIManager.h"
#include "../LayoutMgr/ETSSplitter.h"


class CPanesMgr;
class CSkinMgr;
class CTopPanelWnd;
class CLogoPanelWnd;
class CBottomPanelWnd;
class CTabBarWnd;
class CTabPageParentWnd;

//界面管理类
class CUIManager: public IUIManager
{

public:
	CUIManager();
	~CUIManager();

public:

	bool	UIManAppStart();
	void	UIManAppExit();		
	bool	UICanExit();
	void	UIAddDialog(DWORD dwPtr);
	void	UIRemoveDialog(DWORD dwPtr);

	int		UIDlgSize();
	void	UIAddDialog(CDialog* pDialog);
	void	UIRemoveDialog(CDialog* pDialog);

	void	UIOnPaint( CDC* pDC );
	void	UIOnMove(int xPos,int yPos );
	void	CreateUIWindow(const char* pszWindowName );

	ILayoutMgr*			UIGetLayoutMgr();
	ISkinMgr*			UIGetSkinMgr();
	IChangeColorMgr*	UIGetChangeColorMgr();
private:

	CTopPanelWnd*			m_pWndTopPanel;
	CLogoPanelWnd*			m_pWndLogoPanel;
	CBottomPanelWnd*		m_pWndBottomPanel;
	CTabBarWnd*				m_pWndTabBar;
	CTabPageParentWnd*		m_pWndTabPageParent;

	ETSSplitter				m_SplitterMid;
	vector<DWORD>			m_vecDialog;

	CPanesMgr*				m_pPanesMgr;
	CSkinMgr*				m_pAppSkin;
	IChangeColorMgr*		m_pChangeColorMgr;
};


#endif