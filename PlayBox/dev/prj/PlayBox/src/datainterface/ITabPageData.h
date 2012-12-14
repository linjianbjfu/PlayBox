#pragma once

class ITabPageData:public IData
{
public:
	virtual void ITabBar_ShowPage(TAB_ITEM &TabItem) = 0;
};

