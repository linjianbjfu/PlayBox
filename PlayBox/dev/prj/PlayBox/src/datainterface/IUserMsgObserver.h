#pragma once

#include "IMessageManager.h"

class IUserMsgObserver : public IMessageObserver
{
public:
	virtual void	UserMsg_Login() = 0;//��½�ɹ�
	virtual void	UserMsg_LogOut() = 0;
	virtual void	UserMsg_LogFaild() {};
	virtual void	UserMsg_BeginLogin() {};	 //��ʼ��½
};
