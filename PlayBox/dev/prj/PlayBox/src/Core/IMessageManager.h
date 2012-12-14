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

//�¼�ע�������
class IMessageManager
{

public:
	virtual ~IMessageManager(){};

public:

	//ע��һ���¼�
	virtual void	AttachMessage( 	const DWORD& guidMessage, 
									IMessageObserver* pObserver,
									MESSAGE_OBSERVER_PRIORITY enumPri = NORMAL_PRIORITY ) = 0 ;

	//��ע��һ���¼�
	virtual void	DetachMessage(	const DWORD& guidMessage, 
									IMessageObserver* pObserver ) = 0;

	//����¼�ע��ӿ�
	virtual void	QueryObservers(	const DWORD& guidMessage, 
									std::list<IMessageObserver*>& listOb ) = 0;
};


#endif