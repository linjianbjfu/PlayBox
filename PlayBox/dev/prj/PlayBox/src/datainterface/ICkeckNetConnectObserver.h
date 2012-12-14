#pragma once

#include "IMessageObserver.h"

//播放歌曲数据类事件接口
class ICkeckNetConnectObserver: 
	public IMessageObserver
{
public:
	virtual void	NetConnect_State(BOOL bConnect) = 0;
};