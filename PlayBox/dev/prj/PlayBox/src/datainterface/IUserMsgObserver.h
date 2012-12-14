#pragma once

#include "IMessageManager.h"

class IUserMsgObserver : public IMessageObserver
{
public:
	virtual void	UserMsg_AutoLogin(bool bAuto=false){};
	virtual void	UserMsg_Login(){};
	virtual void	UserMsg_LogOut(){};
	virtual void	UserMsg_LogFaild(){};
	virtual void	UserMsg_LogReshow() {};
	virtual void	UserMsg_BeginLogin(){};
};
