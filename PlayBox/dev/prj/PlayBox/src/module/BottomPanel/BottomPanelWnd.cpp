#include "stdafx.h"
#include "resource.h"
#include "BottomPanelWnd.h"
#include "../../gui/util/CBufferDC.h"
#include "AppConfig/config/ConfigSettingDef.h"
#include "YL_HTTPRequest.h"
#include "../json/json.h"
#include "CDataManager.h"
#include "tools.h"

#define WM_BOTTOM_WEB_GAME_IS_OK WM_USER+1

IMPLEMENT_DYNAMIC(CBottomPanelWnd, CBasicWnd)
CBottomPanelWnd::CBottomPanelWnd()
{
}

CBottomPanelWnd::~CBottomPanelWnd()
{
}

BEGIN_MESSAGE_MAP(CBottomPanelWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_MESSAGE(WM_BOTTOM_WEB_GAME_IS_OK, OnBottomWebGameIsOK)
	ON_MESSAGE(WM_STATICCLICKED, OnClickStatic)
END_MESSAGE_MAP()


int CBottomPanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect r(0,0,0,0);
	m_staticWegGame1.Create(NULL,WS_VISIBLE, r,this,ID_STATIC_BOTTOM_PANEL_WEB_GAME1);
	m_staticWegGame2.Create(NULL,WS_VISIBLE, r,this,ID_STATIC_BOTTOM_PANEL_WEB_GAME2);

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();
	pLayoutMgr->RegisterCtrl(this,"bottomWebGame1",&m_staticWegGame1);
	pLayoutMgr->RegisterCtrl(this,"bottomWebGame2",&m_staticWegGame2);
	pLayoutMgr->CreateControlPane( this,"bottompanel","normal");
	pLayoutMgr->CreateBmpPane( this,"bottompanel","normal" );

	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME,
		CONF_SETTING_CONFIG_BOTTOM_RECOMMAND_WEB_GAME_URL, m_strRecommandWebGameUrl);
	CreateThread(0,0,ThreadGetRecommandWebGame,this,0,0);
	return 0;
}

DWORD CBottomPanelWnd::ThreadGetRecommandWebGame(void* pPara)
{
	CBottomPanelWnd* p = (CBottomPanelWnd*)pPara;	
	YL_CHTTPRequest http;
	if(!http.Request(p->m_strRecommandWebGameUrl, YL_CHTTPRequest::REQUEST_GET, 20*1000 ))
		return 0;

	BYTE *pbyIndex = NULL;
	long size = 0;
	pbyIndex = http.GetContent(size);
	if(pbyIndex && size != 0)
	{
		std::string strContent = std::string((char*)pbyIndex, size);

		//{"recommend_webgame":[{"id":"115","server_id":"3","game_name":"\u4e8c\u6218\u524d\u7ebf","thumbnail_url":"http:\/\/www.najiuwan.com"},{"id":"114","server_id":"13","game_name":"\u5927\u4fa0\u4f20","thumbnail_url":"http:\/\/www.najiuwan.com"}]}
		Json::Reader reader;
		Json::Value root;  
		if (reader.parse(strContent, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素  
		{
			Json::Value jsonGame = root["recommend_webgame"];
			if (jsonGame.isArray())
			{
				for (int i=0; i<jsonGame.size(); i++)
				{
					string strID = jsonGame[i]["id"].asString();
					string strSvrID = jsonGame[i]["server_id"].asString();
					string strGameName = UTF8ToGB(jsonGame[i]["game_name"].asString().c_str());
					string strPicUrl = jsonGame[i]["thumbnail_url"].asString();

					if(strID.empty() || strSvrID.empty() || strGameName.empty() || strPicUrl.empty())
						continue;

					if(i == 0)
					{
						p->m_webGame1.enumType = TAB_WEBGAME;
						p->m_webGame1.strTitle = strGameName;
						p->m_webGame1.strParam = string("method=webgame") + BOX_DELIMITERS
							+ "id=" + strID + BOX_DELIMITERS
							+ "svrid=" + strSvrID + BOX_DELIMITERS
							+ "name=" + strGameName + BOX_DELIMITERS
							+ "picurl=" + strPicUrl + BOX_DELIMITERS;
					} else
					if (i == 1)
					{
						p->m_webGame2.enumType = TAB_WEBGAME;
						p->m_webGame2.strTitle = strGameName;
						p->m_webGame2.strParam = string("method=webgame") + BOX_DELIMITERS
							+ "id=" + strID + BOX_DELIMITERS
							+ "svrid=" + strSvrID + BOX_DELIMITERS
							+ "name=" + strGameName + BOX_DELIMITERS
							+ "picurl=" + strPicUrl + BOX_DELIMITERS;
					}else
						break;
				}
			}
		}
	}
	::PostMessage(p->GetSafeHwnd(), WM_BOTTOM_WEB_GAME_IS_OK, 0, 0);
	return 0;
}

void CBottomPanelWnd::LoadSkin()
{
}

LRESULT	CBottomPanelWnd::OnBottomWebGameIsOK(WPARAM wParam, LPARAM lParam)
{
	if(m_webGame1.enumType == TAB_UNKNOWN)
		m_staticWegGame1.ShowWindow(SW_HIDE);
	else
		m_staticWegGame1.SetWindowText(m_webGame1.strTitle.c_str());

	if(m_webGame2.enumType == TAB_UNKNOWN)
		m_staticWegGame2.ShowWindow(SW_HIDE);
	else
		m_staticWegGame2.SetWindowText(m_webGame2.strTitle.c_str());

	return 0L;
}

LRESULT	CBottomPanelWnd::OnClickStatic(WPARAM wParam, LPARAM lParam)
{
	if (wParam == ID_STATIC_BOTTOM_PANEL_WEB_GAME1)
	{
		GLOBAL_TABBARDATA->ITabBar_ChangeTab(m_webGame1);
	} else
	if (wParam == ID_STATIC_BOTTOM_PANEL_WEB_GAME2)
	{
		GLOBAL_TABBARDATA->ITabBar_ChangeTab(m_webGame2);
	}
	return 0L;
}