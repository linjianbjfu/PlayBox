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
	//用户上次关闭时打开的tab，都读取出来
}

void CTabBarData::DataAppExit()
{
	//保存当前用户打开的所有tab
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

//网页始终在第一个tab打开
//当新建一个tab时，使用NotifyCreateTab，第一个tab打开一个新的网页，也属于新建
//当切换一个tab时，使用NotifyOpenExistTab
void CTabBarData::ITabBar_ChangeTab(TAB_ITEM &item)
{
	//如果当前是全屏模式，就退出全屏
	if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
		GLOBAL_PANELCHANGEDATA->IPanelChange_ExitFullScreen();

	//item有id是打开已有tab，否则新建
	vector<TAB_ITEM>::iterator it = m_vecItem.begin();
	for( ; it != m_vecItem.end(); it++ )
	{
		if(it->id == item.id)
			break;
	}
	if( it != m_vecItem.end() )
	{
		//if open new webgamepanel, should check whether login
		if (item.enumType == TAB_WEBGAME &&
			item.id == 0 &&
			CUserManager::GetInstance()->User_GetUserInfo() == NULL)
		{
			CDlgLogin dlgLogin;
			dlgLogin.DoModal();
			return;
		}
		else
		{
			m_iPos = int(it - m_vecItem.begin());
			m_vecItem[m_iPos] = item;
			NotifyOpenExistTab(item);
		}
	}else
	{
		if( m_vecItem.size() >= MAX_TAB_COUNT )
			//超过上限，在当前tab的close按钮处出现tooltips提示关闭几个
			NotifyOpenTabError( 1 );
		else
		{
			//新建id
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
		it->strTitle = tabItem.strTitle;
}