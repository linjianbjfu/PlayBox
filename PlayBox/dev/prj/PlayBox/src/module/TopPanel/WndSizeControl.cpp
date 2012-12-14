#include "StdAfx.h"
#include "WndSizeControl.h"

WndSizeControl* WndSizeControl::m_pCtr = NULL;

WndSizeControl::WndSizeControl(void)
{
}

WndSizeControl::~WndSizeControl(void)
{
}

WndSizeControl* WndSizeControl::GetInstance()
{
	if( m_pCtr == NULL )
	{
		m_pCtr = new WndSizeControl;
	}
	return m_pCtr;
}

void WndSizeControl::DelInstance()
{
	if( m_pCtr != NULL )
	{
		delete m_pCtr;
		m_pCtr = NULL;
	}
}

void WndSizeControl::AddObserver( IWndSizeObserver* pOb )
{
	if( m_ob.find(pOb) == m_ob.end() )
	{
		m_ob.insert( pOb );
	}
}

void WndSizeControl::LeaveObserver( IWndSizeObserver* pOb )
{
	m_ob.erase(pOb);
}

void WndSizeControl::Max()
{
	for( set<IWndSizeObserver*>::iterator iter = m_ob.begin(); iter != m_ob.end(); iter++ )
	{
		(*iter)->IWndSizeObserver_Max();
	}
}

void WndSizeControl::Min()
{
	for( set<IWndSizeObserver*>::iterator iter = m_ob.begin(); iter != m_ob.end(); iter++ )
	{
		(*iter)->IWndSizeObserver_Min();
	}
}

void WndSizeControl::Restore()
{
	for( set<IWndSizeObserver*>::iterator iter = m_ob.begin(); iter != m_ob.end(); iter++ )
	{
		(*iter)->IWndSizeObserver_Restore();
	}
}

void WndSizeControl::Close()
{
	for( set<IWndSizeObserver*>::iterator iter = m_ob.begin(); iter != m_ob.end(); iter++ )
	{
		(*iter)->IWndSizeObserver_Close();
	}
}
