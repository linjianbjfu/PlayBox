#pragma once

#include "IMessageObserver.h"

//���Ÿ����������¼��ӿ�
class ICkeckNetConnectObserver: 
	public IMessageObserver
{
public:
	virtual void	NetConnect_State(BOOL bConnect) = 0;
};