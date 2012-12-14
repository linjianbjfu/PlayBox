#ifndef _SHOW_MENU_H
#define _SHOW_MENU_H

#include "BCMenu.h"

class CShowMenu
{
public:
	CShowMenu( CWnd* pParent,DWORD dwMenuID,DWORD dwSubItem = 0);
	CShowMenu(){};
	~CShowMenu(){};

public:
	void	CreatePopupMenu();
	void	InsertSeperator(int pos);
	void	InsertPopupMenu(int pos,UINT hMenu,const char* pszMenuStr);
	void	InsertMenu(int pos,DWORD dwMenuItemID,const char* pszMenuStr);
	void	InsertMenu(int submenupos,int pos,DWORD dwMenuItemID,const char* pszMenuStr);
	void	InsertMenu(int submenupos,int pos, UINT nFlag);
	void	DeletePopupItem(int pos);
	void	DeleteItem(DWORD dwMenuItemID);
	void	DeleteItem(DWORD dwMenuItemID, BC_Seperator sPos);
	void	DeleteItem(const char* pText);
	void	DisableItem(DWORD dwMenuItemID);
	void	CheckItem( DWORD dwMenuItemID);
	void	ShowMenu(const CPoint& point);
	bool	MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

public:
	BCMenu	m_bcMenu;
	BCMenu*	m_pMenu;
	CWnd*	m_pWnd;
};


#endif;