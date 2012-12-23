#include "stdafx.h"
#include "TabBarData.h"
#include "../../core/CDataManager.h"

CTabBarData*	CTabBarData::m_pTabBarData = NULL;
#define MAX_TAB_COUNT	10

IData* CTabBarData::GetInstance()
{
	if(m_pTabBarData == NULL)
		m_pTabBarData = new CTabBarData();
	return m_pTabBarData;
}

void CTabBarData::DelInstance()
{
	if(m_pTabBarData != NULL)
		delete m_pTabBarData;
	m_pTabBarData = NULL;
}

void CTabBarData::DataAppStart()
{
	//�û��ϴιر�ʱ�򿪵�tab������ȡ����
}

void CTabBarData::DataAppExit()
{
	//���浱ǰ�û��򿪵�����tab
}

void CTabBarData::NotifyCreateTab(TAB_ITEM &item)
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_TABBAR,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		ITabBarObserver* pOb = dynamic_cast<ITabBarObserver*>(*itOb);
		pOb->ITabBarOb_CreateNewTab(item);
	}
}

void CTabBarData::NotifyOpenExistTab(TAB_ITEM &item)
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_TABBAR,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		ITabBarObserver* pOb = dynamic_cast<ITabBarObserver*>(*itOb);
		pOb->ITabBarOb_OpenExistTab(item);
	}
}

void CTabBarData::NotifyDelTab(TAB_ITEM &item)
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_TABBAR,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		ITabBarObserver* pOb = dynamic_cast<ITabBarObserver*>(*itOb);
		pOb->ITabBarOb_DelTab(item);
	}
}

void CTabBarData::NotifyOpenTabError(int iErrorCode)
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_TABBAR,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		ITabBarObserver* pOb = dynamic_cast<ITabBarObserver*>(*itOb);
		pOb->ITabBarOb_OpenTabError(iErrorCode);
	}
}

//��ҳʼ���ڵ�һ��tab��
//���½�һ��tabʱ��ʹ��NotifyCreateTab����һ��tab��һ���µ���ҳ��Ҳ�����½�
//���л�һ��tabʱ��ʹ��NotifyOpenExistTab
void CTabBarData::ITabBar_ChangeTab(TAB_ITEM &item)
{
	//�����ǰ��ȫ��ģʽ�����˳�ȫ��
	if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
	{
		GLOBAL_PANELCHANGEDATA->IPanelChange_ExitFullScreen();
	}
	//////////////////////
	vector<TAB_ITEM>::iterator it = m_vecItem.begin();
	for( ; it != m_vecItem.end(); it++ )
	{
		if( it->eumType == item.eumType
			&& it->strName == item.strName
			&& it->strParam == item.strParam)
		{
			break;
		}
	}
	if( it != m_vecItem.end() )
	{
		m_iPos = int(it - m_vecItem.begin());
		NotifyOpenExistTab(item);
	}else
	{
		if( m_vecItem.size() >= MAX_TAB_COUNT )
		{
			//�������ޣ��ڵ�ǰtab��close��ť������tooltips��ʾ�رռ���
			NotifyOpenTabError( 1 );
		}else
		{
			m_vecItem.push_back(item);
			m_iPos = int(m_vecItem.size()-1);
			NotifyCreateTab(item);
		}
	}
}

void CTabBarData::ITabBar_DeleteTab(TAB_ITEM &item)
{
	TAB_ITEM itemexisted;
	vector<TAB_ITEM>::iterator it = m_vecItem.begin();
	for( ; it != m_vecItem.end(); it++ )
	{
		if( it->eumType == item.eumType
			&& it->strName == item.strName
			&& it->strParam == item.strParam)
		{
			break;
		}
	}
	if( it != m_vecItem.end() )
	{
		m_vecItem.erase( it );

		m_iPos--;
		if( m_iPos < 0 )
		{
			m_iPos = 0;
		}
		NotifyDelTab(item);
	}
}

int CTabBarData::ITabBar_GetCurPos()
{
	return m_iPos;
} 

bool CTabBarData::ITabBar_GetCurItem( TAB_ITEM &TabItem )
{
	if( m_iPos >= 0 && m_iPos < m_vecItem.size() )
	{
		TabItem = m_vecItem[m_iPos];
		return true;
	}else
	{
		return false;
	}	
}

void CTabBarData::ITabBar_GetTabBarData( vector<TAB_ITEM>& vec )
{
	vec.clear();
	copy(m_vecItem.begin(), m_vecItem.end(), back_inserter(vec));
}
