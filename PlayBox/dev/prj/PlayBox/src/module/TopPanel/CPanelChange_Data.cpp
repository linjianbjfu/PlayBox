#include "stdafx.h"
#include "CPanelChange_Data.h"
#include "../../DataInterface/IPanelChangeObserver.h"

CPanelChange* CPanelChange::m_pPanelChange = NULL;

IData* CPanelChange::GetInstance()
{
	if( m_pPanelChange == NULL )
	{
		m_pPanelChange = new CPanelChange();		
	}
	return m_pPanelChange;
}

void CPanelChange::DataAppStart()
{
	m_bFullScreen = false;
	pWndFullScreen = NULL;
}

void CPanelChange::DataAppExit()
{
	delete this;
}

void CPanelChange::IPanelChange_ToFullScreen( CWnd* pWnd )
{
	AfxGetMainWindow()->ShowWindow( SW_SHOW );
	m_bFullScreen = true;
	pWndFullScreen = pWnd;

	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_PANEL_CHANGE,listOb);
	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IPanelChangeObserver * pOb = dynamic_cast<IPanelChangeObserver*>(*itOb);
		pOb->IPanelChangeOb_ToFullScreen( pWndFullScreen );
	}
}

bool CPanelChange::IPanelChange_IsFullScreen()
{
	return m_bFullScreen;
}

void CPanelChange::IPanelChange_ExitFullScreen()
{
	m_bFullScreen = false;

	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_PANEL_CHANGE,listOb);
	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IPanelChangeObserver * pOb = dynamic_cast<IPanelChangeObserver*>(*itOb);
		pOb->IPanelChangeOb_ExitFullScreen( pWndFullScreen );
	}
	pWndFullScreen = NULL;
}

bool CPanelChange::IPanelChange_Max()
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_PANEL_CHANGE,listOb);
	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IPanelChangeObserver * pOb = dynamic_cast<IPanelChangeObserver*>(*itOb);
		pOb->IPanelChangeOb_WindowMax();
	}
	return true;
}

bool CPanelChange::IPanelChange_Min()
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_PANEL_CHANGE,listOb);
	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IPanelChangeObserver * pOb = dynamic_cast<IPanelChangeObserver*>(*itOb);
		pOb->IPanelChangeOb_WindowMin();
	}
	return true;
}

bool CPanelChange::IPanelChange_Restore()
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_PANEL_CHANGE,listOb);
	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IPanelChangeObserver * pOb = dynamic_cast<IPanelChangeObserver*>(*itOb);
		pOb->IPanelChangeOb_WindowRestore();
	}
	return true;
}

void CPanelChange::IPanelChange_Close()
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_PANEL_CHANGE,listOb);
	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IPanelChangeObserver * pOb = dynamic_cast<IPanelChangeObserver*>(*itOb);
		pOb->IPanelChangeOb_WindowClose();
	}
}
