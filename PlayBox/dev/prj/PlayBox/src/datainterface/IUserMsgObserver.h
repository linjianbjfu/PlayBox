#pragma once

#include "IMessageManager.h"

class IUserMsgObserver : public IMessageObserver
{
public:
	virtual void	UserMsg_Login(){};//��½�ɹ�
	virtual void	UserMsg_LogOut(){};
	virtual void	UserMsg_LogFaild(){};
	virtual void	UserMsg_BeginLogin(){};	 //��ʼ��½
};
