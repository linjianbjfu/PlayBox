#include "stdafx.h"
#include "ShowMenu.h"

CShowMenu::CShowMenu(CWnd* pParent,DWORD dwMenuID,DWORD dwSubItem)
{

	ISkinMgr* pSkinMgr = AfxGetUIManager()->UIGetSkinMgr();
	
	m_bcMenu.SetTextColor( pSkinMgr->GetColor( "MenuText") );
	m_bcMenu.SetMenuItemBkColor( pSkinMgr->GetColor( "MenuBk") );
	m_bcMenu.SetMenuItemBitmapBkColor(pSkinMgr->GetColor( "MenuLeftBk"));
	m_bcMenu.m_menuItemSelectBKColor  = pSkinMgr->GetColor( "MenuSelBk");
	m_pWnd = pParent;
	m_bcMenu.SetMenuDrawMode(1);
	m_bcMenu.LoadMenu(dwMenuID);
	m_pMenu	= (BCMenu*)(m_bcMenu.GetSubMenu(dwSubItem));
}

void CShowMenu::CreatePopupMenu()
{
	m_pMenu = new BCMenu();
	m_pMenu->CreatePopupMenu();
}

void CShowMenu::DeletePopupItem(int pos)
{
	m_pMenu->DeleteMenu(pos,MF_BYPOSITION);
}
void CShowMenu::DeleteItem(DWORD dwMenuItemID)
{
	m_pMenu->DeleteMenu( dwMenuItemID,MF_BYCOMMAND);
}
void CShowMenu::DeleteItem(DWORD dwMenuItemID, BC_Seperator sPos)
{
	CString str;
	m_pMenu->GetMenuText(dwMenuItemID, str, MF_BYCOMMAND);
	m_pMenu->DeleteMenu((char*)(LPCSTR)str, sPos);
}
void CShowMenu::DeleteItem(const char* pText)
{
	m_pMenu->DeleteMenu((char*)pText);
}
void CShowMenu::DisableItem(DWORD dwMenuItemID)
{
	m_pMenu->EnableMenuItem(dwMenuItemID, MF_GRAYED);
}

void CShowMenu::CheckItem( DWORD dwMenuItemID)
{
	m_pMenu->CheckMenuItem( dwMenuItemID,MF_BYCOMMAND|MF_CHECKED);
}

void CShowMenu::ShowMenu(const CPoint& point)
{
	m_pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,m_pWnd);
}

void CShowMenu::InsertSeperator(int pos)
{
	m_pMenu->InsertMenu( pos,MF_BYPOSITION|MF_SEPARATOR,0,"-");
}

void CShowMenu::InsertMenu(int pos,DWORD dwMenuItemID,const char* pszMenuStr)
{
	string strInput = pszMenuStr;
	
	int iPos  = -1;
	
	while( (iPos = strInput.find('&',iPos+1)) != string::npos )
	{
		string strLeft = strInput.substr(0,iPos);
		string strRight = strInput.substr(iPos,strInput.length()-iPos+1);
		iPos++;

		strInput = strLeft + string("&") + strRight;
	}

	m_pMenu->InsertMenu(pos,MF_BYPOSITION|MF_STRING,dwMenuItemID,strInput.c_str());
}

void CShowMenu::InsertMenu(int subpos,int pos,DWORD dwMenuItemID,const char* pszMenuStr)
{
	BCMenu* pSubMenu = (BCMenu*)m_pMenu->GetSubMenu(subpos);

	string strInput = pszMenuStr;

	int iPos  = -1;

	while( (iPos = strInput.find('&',iPos+1)) != string::npos )
	{
		string strLeft = strInput.substr(0,iPos);
		string strRight = strInput.substr(iPos,strInput.length()-iPos+1);
		iPos++;

		strInput = strLeft + string("&") + strRight;
	}

	pSubMenu->InsertMenu( pos,MF_BYPOSITION|MF_STRING,dwMenuItemID,strInput.c_str() );
}

void CShowMenu::InsertMenu(int subpos,int pos, UINT nFlag)
{
	BCMenu* pSubMenu = (BCMenu*)m_pMenu->GetSubMenu(subpos);
	pSubMenu->InsertMenu( pos, nFlag );
}

bool CShowMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if( BCMenu::IsMenu((HMENU)lpMeasureItemStruct->itemID ) )
	{
		m_pMenu->MeasureItem( lpMeasureItemStruct );
		return true;
	}
	return false;
}

void CShowMenu::InsertPopupMenu(int pos,UINT hMenu,const char* pszMenuStr)
{
	m_pMenu->InsertMenu(pos,MF_POPUP|MF_BYPOSITION|MF_STRING,(UINT)hMenu,pszMenuStr);
}