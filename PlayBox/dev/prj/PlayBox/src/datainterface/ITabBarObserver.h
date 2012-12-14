#pragma once
#include "ITabBarData.h"
#include "IMessageManager.h"

class ITabBarObserver : public IMessageObserver
{
public:
	virtual void ITabBarOb_CreateNewTab(TAB_ITEM & item)=0;
	virtual void ITabBarOb_OpenExistTab(TAB_ITEM & item)=0;
	virtual void ITabBarOb_DelTab(TAB_ITEM & item)=0;
	virtual void ITabBarOb_OpenTabError(int iErrorCode)=0;
};