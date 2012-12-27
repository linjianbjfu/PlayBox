#include "stdafx.h"
#include "tabpagecontrol.h"
#include "CDataManager.h"
#include "TabWndFactory.h"
#include "YL_StringUtil.h"
#include "YL_URLEncoder.h"
#include "tools.h"
#include "../GamePanel/GamePanelWnd.h"
#include "../GamePanel/WebGamePanelWnd.h"
#include "../WebInteract/MyWebBrowserWnd.h"
#include "../WebInteract/WebManager.h"
#include "../PlayedGamePanel/PlayedGameWnd.h"
#include "../../Core/CDataManager.h"
#include "../../AppConfig/config/ConfigSettingDef.h"
#include "YL_DirInfo.h"
#include "YL_FileInfo.h"
#include "YL_HTTPDownFile.h"

#include <algorithm>
#include "../BrowserPanel/BrowserPanelWnd.h"
#include "../GameCenterPanel/GameCenterPanelWnd.h"

CTabPageControl * CTabPageControl::m_pTabPageControl = NULL;

CTabPageControl* CTabPageControl::GetInstance()
{
	if( m_pTabPageControl == NULL )
		m_pTabPageControl = new CTabPageControl;
	return m_pTabPageControl;
}

void CTabPageControl::DelInstance()
{
	if( m_pTabPageControl != NULL )
	{
		delete m_pTabPageControl;
		m_pTabPageControl = NULL;
	}
}

CTabPageControl::CTabPageControl()
{
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_TABBAR,(ITabBarObserver*) this);
}

CTabPageControl::~CTabPageControl()
{
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_TABBAR,(ITabBarObserver*) this);
}

void CTabPageControl::SetParentWnd(CWnd* pWnd)
{
	m_pWndParent = pWnd;
	TabWndFactory::GetInstance()->SetParent( pWnd );
}

//展现游戏列表时，第一个tab打开
//否则，后续增加tab
void CTabPageControl::ITabBarOb_CreateNewTab(TAB_ITEM & item)
{
	CRect rc;
	m_pWndParent->GetClientRect(&rc);

	CWnd* pWndTmp = NULL;
	if( item.eumType == TAB_HOME )
	{
		GameCenterPanelWnd* pWnd = TabWndFactory::GetInstance()->CreateWndGameCenterPanel();
		pWnd->Navigate();
		pWndTmp = pWnd;
	}else
	if( item.eumType == TAB_FLASHGAME )
	{
		GamePanelWnd *pWnd = TabWndFactory::GetInstance()->CreateWndGamePanel();
		pWnd->SetTabItem( item );
		pWndTmp = pWnd;
	}else
	if( item.eumType == TAB_WEBGAME )
	{
		WebGamePanelWnd *pWnd = TabWndFactory::GetInstance()->CreateWndWebGamePanel();
		pWnd->SetTabItem( item );
		pWndTmp = pWnd;
	}else
	if( item.eumType == TAB_BROWSER )
	{
		BrowserPanelWnd *pWnd = TabWndFactory::GetInstance()->CreateWndBrowserPanel();
		string strUrl = CWebManager::GetInstance()->GetValue( item.strParam, "url" );
		if (strUrl.empty())
		{
			strUrl = "about:blank";
		}
		pWnd->Navigate( strUrl );
		pWndTmp = pWnd;
	}else if( item.eumType == TAB_PLAYED_GAME )
	{
		PlayedGameWnd* pWnd = TabWndFactory::GetInstance()->CreateWndPlayedGame();
		pWndTmp = pWnd;
	}
	if( pWndTmp != NULL )
	{
		ONE_TAB ot;
		ot.first = item;
		ot.second = pWndTmp;
		m_mapTab.push_back( ot );
		ITabBarOb_OpenExistTab( item );
	}
}

static void _string_help(const CString& strInput,CString& strLeft,CString& strRight)
{
	strLeft = strRight = "";

	int iPosEq = strInput.Find('=');
	if( iPosEq == -1 )
		return;

	strLeft		= strInput.Left(iPosEq);
	strRight	= strInput.Mid(iPosEq+1,strInput.GetLength()-iPosEq-1);
}

static string GetValue( string& strContent, string strKey )
{
	string strRes;
	CString str(strContent.c_str());
	CString strOneItem;
	int curPos = 0;
	strOneItem= str.Tokenize("\n",curPos);
	while(strOneItem != "")
	{		
		CString strLeft,strRight;
		_string_help( strOneItem,strLeft,strRight );
		if( strLeft.Compare(strKey.c_str()) == 0 )
		{
			strRes = strRight;
			break;
		}
		strOneItem = str.Tokenize("\n", curPos);
	};
	return strRes;
}

void CTabPageControl::ITabBarOb_OpenExistTab(TAB_ITEM & item)
{
	for( vector<ONE_TAB>::iterator it = m_mapTab.begin();
		it != m_mapTab.end(); it++ )
	{
		bool bShow = (it->first.strName == item.strName && 
			it->first.eumType == item.eumType && 
			it->first.strParam == item.strParam);
		it->second->ShowWindow( bShow ? SW_SHOW : SW_HIDE );
	}
}

void CTabPageControl::ITabBarOb_DelTab(TAB_ITEM & item)
{
	vector<ONE_TAB>::iterator it = m_mapTab.begin();
	for( ; it != m_mapTab.end(); it++ )
	{
		if( it->first.strName == item.strName
			&& it->first.eumType == item.eumType
			&& it->first.strParam == item.strParam)
		{
			break;
		}
	}
	if( it != m_mapTab.end() )
	{
		TabWndFactory::GetInstance()->Recycle( it->second );
		m_mapTab.erase( it );
	}
	//显示上一个tab内容
	TAB_ITEM ti;
	GLOBAL_TABBARDATA->ITabBar_GetCurItem( ti );
	{
		ITabBarOb_OpenExistTab( ti );
	}
}

void CTabPageControl::ITabBarOb_OpenTabError(int iErrorCode)
{
}

void CTabPageControl::ResizePage()
{
	if( m_mapTab.size() <= 0 )
		return;
	//当前显示的tabitem
	TAB_ITEM ti;
	GLOBAL_TABBARDATA->ITabBar_GetCurItem( ti );
	//父窗口大小
	CRect rc;
	m_pWndParent->GetClientRect(&rc);

	for( vector<ONE_TAB>::iterator it = m_mapTab.begin();
		it != m_mapTab.end(); it++ )
	{
		it->second->MoveWindow (rc, FALSE);
		bool bShow = (it->first.eumType == ti.eumType
			&& it->first.strName == ti.strName
			&& it->first.strParam == ti.strParam);
		it->second->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	}
}

void CTabPageControl::OpenHomePage()
{
	TAB_ITEM ti;
	ti.strName = "我的游戏";
	ti.strParam = "";
	ti.eumType  = TAB_PLAYED_GAME;
	GLOBAL_TABBARDATA->ITabBar_ChangeTab( ti );

	{
		TAB_ITEM ti;
		ti.strName = "游戏大厅";
		ti.eumType  = TAB_HOME;
		GLOBAL_TABBARDATA->ITabBar_ChangeTab( ti );
	}
}

void CTabPageControl::CallJS (LPVOID lpVoid)
{
	for( vector<ONE_TAB>::iterator it = m_mapTab.begin();
		it != m_mapTab.end(); it++ )
	{
		MyWebBrowserWnd * pBrowser = (MyWebBrowserWnd *)it->second;
		if (pBrowser != NULL && ::IsWindow (pBrowser->m_hWnd))
		{
			pBrowser->OnCallJavaScript ((WPARAM)pBrowser, (LPARAM)lpVoid);
		}
	}
}
