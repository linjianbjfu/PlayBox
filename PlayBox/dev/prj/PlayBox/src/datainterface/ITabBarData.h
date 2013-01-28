#pragma once

#include "IData.h"
#include "../Global/GlobalVariable.h"


class ITabBarData:public IData
{
public:
	virtual void ITabBar_ChangeTab(TAB_ITEM &TabItem) = 0;
	virtual void ITabBar_DeleteTab(TAB_ITEM &TabItem) = 0;
	virtual int	 ITabBar_GetCurPos() = 0;
	virtual bool ITabBar_GetCurItem( TAB_ITEM &TabItem ) = 0;
	virtual void ITabBar_GetTabBarData( vector<TAB_ITEM>& vec ) = 0;
	virtual void ITabBar_SetBarData(TAB_ITEM &tabItem) = 0; //e.g. title changed
};
