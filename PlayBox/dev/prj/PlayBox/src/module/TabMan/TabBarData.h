#pragma once
#include "StdAfx.h"
#include "ITabBarObserver.h"
#include <vector>
using std::vector;

class CTabBarData : public ITabBarData
{
private:
	int					m_iPos;		//当前选中的tab
	vector<TAB_ITEM>	m_vecItem;	//所有tab
    int					m_idCounter;
private:
	CTabBarData();
	void NotifyCreateTab(TAB_ITEM &item);
	void NotifyOpenExistTab(TAB_ITEM &item);
	void NotifyDelTab(TAB_ITEM &item);
	void NotifyOpenTabError( int iErrorCode );
	void NotifyTabItemDataChanged(TAB_ITEM &item);
public:
	static		CTabBarData*	m_pTabBarData;
	static		IData*			GetInstance();
	static		void			DelInstance();
	void		DataAppStart();
	void		DataAppExit();

	void		ITabBar_ChangeTab(TAB_ITEM &item);
	void		ITabBar_DeleteTab(TAB_ITEM &item);
	int			ITabBar_GetCurPos();
	bool		ITabBar_GetCurItem( TAB_ITEM &TabItem );
	void		ITabBar_GetTabBarData( vector<TAB_ITEM>& vec );
	void		ITabBar_SetBarData(TAB_ITEM &tabItem);
};