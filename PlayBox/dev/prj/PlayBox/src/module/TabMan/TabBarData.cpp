#include "stdafx.h"
#include "TabBarData.h"
#include "../../core/CDataManager.h"
#include "../UserMan/UserManager.h"
#include "../UserMan/DlgLogin.h"

CTabBarData*	CTabBarData::m_pTabBarData = NULL;
#define MAX_TAB_COUNT	10

CTabBarData::CTabBarData() : m_idCounter(0) {}

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

#define NOTIFY_TABBAR_OB_BEGIN() \
	list<IMessageObserver*> listOb; \
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_TABBAR,listOb); \
	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ ) \
	{ \
		ITabBarObserver* pOb = dynamic_cast<ITabBarObserver*>(*itOb);

#define NOTIFY_TABBAR_OB_END() \
	}

void CTabBarData::NotifyCreateTab(TAB_ITEM &item)
{
	NOTIFY_TABBAR_OB_BEGIN()
		pOb->ITabBarOb_CreateNewTab(item);
	NOTIFY_TABBAR_OB_END()
}

void CTabBarData::NotifyOpenExistTab(TAB_ITEM &item)
{
	NOTIFY_TABBAR_OB_BEGIN()
		pOb->ITabBarOb_OpenExistTab(item);
	NOTIFY_TABBAR_OB_END()
}

void CTabBarData::NotifyDelTab(TAB_ITEM &item)
{
	NOTIFY_TABBAR_OB_BEGIN()
		pOb->ITabBarOb_DelTab(item);
	NOTIFY_TABBAR_OB_END()
}

void CTabBarData::NotifyOpenTabError(int iErrorCode)
{
	NOTIFY_TABBAR_OB_BEGIN()
		pOb->ITabBarOb_OpenTabError(iErrorCode);
	NOTIFY_TABBAR_OB_END()
}

void CTabBarData::NotifyTabItemDataChanged(TAB_ITEM &item)
{
	NOTIFY_TABBAR_OB_BEGIN()
		pOb->ITabBarOb_TabItemDataChanged(item);
	NOTIFY_TABBAR_OB_END()
}

//��ҳʼ���ڵ�һ��tab��
//���½�һ��tabʱ��ʹ��NotifyCreateTab����һ��tab��һ���µ���ҳ��Ҳ�����½�
//���л�һ��tabʱ��ʹ��NotifyOpenExistTab
void CTabBarData::ITabBar_ChangeTab(TAB_ITEM &item)
{
	//�����ǰ��ȫ��ģʽ�����˳�ȫ��
	if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
		GLOBAL_PANELCHANGEDATA->IPanelChange_ExitFullScreen();

	//1 item��id�Ǵ�����tab
	bool bFound = false;
	for(vector<TAB_ITEM>::iterator it = m_vecItem.begin();
		it != m_vecItem.end(); it++ )
	{
		if (!bFound && 
			item.id != -1 && 
			it->id == item.id)
			 bFound = true;

		if (!bFound &&
			(item.enumType == TAB_HOME || 
			item.enumType == TAB_PLAYED_GAME || 
			item.enumType == TAB_GAME_INFO_HOME) &&
			it->enumType == item.enumType)
			bFound = true;

		if (!bFound &&
			(item.enumType == TAB_WEBGAME || item.enumType == TAB_FLASHGAME) &&
			it->strParam == item.strParam)
			bFound = true;

		if (bFound)
		{
			m_iPos = int(it - m_vecItem.begin());
			NotifyOpenExistTab(m_vecItem[m_iPos]);
			return;
		}
	}

	if( m_vecItem.size() >= MAX_TAB_COUNT )
		//�������ޣ��ڵ�ǰtab��close��ť������tooltips��ʾ�رռ���
		NotifyOpenTabError( 1 );
	else
	{
		//if open new webgamepanel, should check whether login
		if (item.enumType == TAB_WEBGAME &&
			item.id == -1 &&
			CUserManager::GetInstance()->User_GetUserInfo() == NULL)
		{
			CDlgLogin dlgLogin;
			dlgLogin.AddTask(item);
			dlgLogin.DoModal();
		}
		else
		{
			//�½�id
			item.id = m_idCounter++;
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
		if(it->id == item.id)
			break;
	}
	if( it != m_vecItem.end() )
	{
		m_vecItem.erase( it );
		m_iPos--;
		if( m_iPos < 0 )
			m_iPos = 0;
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
		return false;
}

void CTabBarData::ITabBar_GetTabBarData( vector<TAB_ITEM>& vec )
{
	vec.clear();
	copy(m_vecItem.begin(), m_vecItem.end(), back_inserter(vec));
}

void CTabBarData::ITabBar_SetBarData(TAB_ITEM &tabItem)
{
	TAB_ITEM itemexisted;
	vector<TAB_ITEM>::iterator it = m_vecItem.begin();
	for( ; it != m_vecItem.end(); it++ )
	{
		if(it->id == tabItem.id)
			break;
	}
	if( it != m_vecItem.end() )
	{
		it->strTitle = tabItem.strTitle;
		NotifyTabItemDataChanged(tabItem);
	}
}