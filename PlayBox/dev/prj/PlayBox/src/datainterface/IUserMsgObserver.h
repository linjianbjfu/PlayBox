#pragma once

#include "IMessageManager.h"

class IUserMsgObserver : public IMessageObserver
{
public:
	virtual void	UserMsg_Login(){};//登陆成功
	virtual void	UserMsg_LogOut(){};
	virtual void	UserMsg_LogFaild(){};
	virtual void	UserMsg_BeginLogin(){};	 //开始登陆
};
