#include "CMessageManager.h"

CMessageManager::CMessageManager()
{
	InitializeCriticalSection( &m_cs );
}
CMessageManager::~CMessageManager()
{
	DeleteCriticalSection( &m_cs );
}

//注册一类事件
void CMessageManager::AttachMessage( const DWORD& guidMessage, IMessageObserver* pObserver,MESSAGE_OBSERVER_PRIORITY enumPri )
{
	
	MESSAGE_OBSERVER* pMo = new MESSAGE_OBSERVER;
	pMo->enumMessageObserverPriority = enumPri;
	pMo->pMessageObserver = pObserver;

EnterCriticalSection(&m_cs);

	if( mapObservers.find(guidMessage) != mapObservers.end() )
	{
		list<MESSAGE_OBSERVER*>& listOb = mapObservers[guidMessage];	
		list<MESSAGE_OBSERVER*>::iterator it = listOb.begin();

		for(;it != listOb.end();it++ )
		{
			if( (*it)->enumMessageObserverPriority >= enumPri )
			{
				listOb.insert(it,pMo);
				break;
			}
		}
		if( it == listOb.end() )
		{
			listOb.push_back(pMo);
		}
	}
	else
	{
		list<MESSAGE_OBSERVER*> listob;
		listob.push_back(pMo);
		mapObservers[guidMessage] = listob;		
	}

LeaveCriticalSection(&m_cs);
}

//反注册一类事件
void CMessageManager::DetachMessage( const DWORD& guidMessage, IMessageObserver* pObserver )
{
EnterCriticalSection(&m_cs);
	if( mapObservers.find(guidMessage) == mapObservers.end() )
	{
		LeaveCriticalSection(&m_cs);
		return;
	}

	list<MESSAGE_OBSERVER*>& listOb = mapObservers[guidMessage];

	for( list<MESSAGE_OBSERVER*>::iterator it = listOb.begin(); it != listOb.end(); it++ )
	{
		if( (*it)->pMessageObserver == pObserver )
		{
			delete (*it);
			listOb.erase(it);
			break;
		}
	}

LeaveCriticalSection(&m_cs);
}

//获得事件注册接口
void CMessageManager::QueryObservers(	const DWORD& guidMessage, std::list<IMessageObserver*>& listOb )
{
//EnterCriticalSection(&m_cs);
	if( mapObservers.find( guidMessage) == mapObservers.end() )
	{
//		LeaveCriticalSection(&m_cs);
		return;
	}

	list<MESSAGE_OBSERVER*>& listOb2 = mapObservers[guidMessage];

	for( list<MESSAGE_OBSERVER*>::iterator it = listOb2.begin(); it != listOb2.end(); it++ )
	{
		listOb.push_back((*it)->pMessageObserver);
	}
//LeaveCriticalSection(&m_cs);
}

//void CMessageManager::EnterCritical()
//{
//	EnterCriticalSection(&m_cs);
//}
//
//void CMessageManager::LeaveCritical()
//{
//	LeaveCriticalSection(&m_cs);
//}

//反注册一类事件

void CMessageManager::AttachMessage(const GUID& guidMessage, IMessageObserver* pObserver,MESSAGE_OBSERVER_PRIORITY enumPri)
{
	AttachMessage( guidMessage,pObserver,enumPri);
}

void CMessageManager::DetachMessage(const GUID& guidMessage,IMessageObserver* pObserver )
{
	DetachMessage( guidMessage,pObserver);
}