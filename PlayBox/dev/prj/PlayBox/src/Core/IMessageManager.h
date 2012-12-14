#ifndef _I_MESSAGE_MANAGER_H
#define _I_MESSAGE_MANAGER_H

#include "./IMessageObserver.h"
#include "KwGUID.h"
#include <list>


struct MESSAGE_OBSERVER
{
	IMessageObserver* pMessageObserver;
	MESSAGE_OBSERVER_PRIORITY enumMessageObserverPriority;

};

//事件注册管理类
class IMessageManager
{

public:
	virtual ~IMessageManager(){};

public:

	//注册一类事件
	virtual void	AttachMessage( 	const DWORD& guidMessage, 
									IMessageObserver* pObserver,
									MESSAGE_OBSERVER_PRIORITY enumPri = NORMAL_PRIORITY ) = 0 ;

	//反注册一类事件
	virtual void	DetachMessage(	const DWORD& guidMessage, 
									IMessageObserver* pObserver ) = 0;

	//获得事件注册接口
	virtual void	QueryObservers(	const DWORD& guidMessage, 
									std::list<IMessageObserver*>& listOb ) = 0;
};


#endif