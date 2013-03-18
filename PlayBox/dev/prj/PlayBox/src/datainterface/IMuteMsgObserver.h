#pragma once

#include "IMessageManager.h"

class IMuteMsgObserver : public IMessageObserver
{
public:
	virtual void	MuteMsg_Change(bool bMute){};
};
