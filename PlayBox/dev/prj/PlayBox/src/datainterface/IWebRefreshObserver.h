#pragma once

#include "IMessageManager.h"

class IWebRefreshObserver : public IMessageObserver
{
public:
	virtual void IWebRefreshOb_Refresh(const char* psz){};
};
