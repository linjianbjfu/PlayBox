#ifndef _C_MESSAGE_MANAGER_H
#define _C_MESSAGE_MANAGER_H

#include "IMessageManager.h"
#include <map>
#include <list>
using namespace std;

class CMessageManager: public IMessageManager
{

public:
	CMessageManager();
	~CMessageManager();

public:

	//注册一类事件
	virtual void	AttachMessage( 	const DWORD& guidMessage, 
									IMessageObserver* pObserver,
									MESSAGE_OBSERVER_PRIORITY enumPri = NORMAL_PRIORITY  );

	virtual void	AttachMessage( 	const GUID& guidMessage, 
									IMessageObserver* pObserver,
									MESSAGE_OBSERVER_PRIORITY enumPri = NORMAL_PRIORITY );

	//反注册一类事件
	virtual void	DetachMessage(	const DWORD& guidMessage, 
									IMessageObserver* pObserver );

	virtual void	DetachMessage(	const GUID& guidMessage, 
									IMessageObserver* pObserver );

	//获得事件注册接口
	virtual void	QueryObservers(	const DWORD& guidMessage, 
									std::list<IMessageObserver*>& listOb );


//	virtual void	EnterCritical();

//	virtual void	LeaveCritical();

private:
	map< DWORD,list<MESSAGE_OBSERVER*> > mapObservers;
	CRITICAL_SECTION		m_cs;
};



#endif