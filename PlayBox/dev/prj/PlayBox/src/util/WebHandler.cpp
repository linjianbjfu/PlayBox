#include "stdafx.h"
#include "WebHandler.h"
#include "../DataInterface/ITabBarData.h"
#include "CDataManager.h"
#include "../../../../CommonLib/common/YL_Base64.h"
#include "YL_HTTPRequest.h"
#include "YL_StringUtil.h"
#include "YL_FileInfo.h"
#include "YL_URLEncoder.h"
#include "tools.h"


CWebHandler* CWebHandler::m_pWebHandler = NULL;

CWebHandler::CWebHandler()
{
}
CWebHandler::~CWebHandler()
{
}

CWebHandler* CWebHandler::GetInstance()
{	
	if( m_pWebHandler == NULL )
	{
		m_pWebHandler = new CWebHandler();
	}
	return m_pWebHandler;
}

void CWebHandler::DelInstance()
{
	if( m_pWebHandler != NULL )
	{
		delete m_pWebHandler;
	}
}
//该接口已废弃使用，由WebManager替代
void CWebHandler::OnPlay(const char* tmp )
{
	tmp += 5;
	if(*tmp == '?')
		tmp++;

	string gid		= GetKeyValueStrUnescape(tmp, "gid");
	string strName	= GetKeyValueStrUnescape(tmp, "gname");

	//string strTmp = "街头霸王";
	//string strDst1 = YL_URLEncoder::encode( strTmp );
	//char szDst[MAX_PATH];
	//memset(szDst,0,MAX_PATH);
	//YL_Base64Encode( szDst, strTmp.c_str(), strTmp.length() );
	
	TAB_ITEM tItem;
	tItem.eumType = TAB_GAME;
	tItem.strName = strName;
	tItem.strParam = gid;
	GLOBAL_TABBARDATA->ITabBar_ChangeTab(tItem);
}
//该接口已废弃使用，第一版暂且不支持游戏列表中包含web game
void CWebHandler::OnPlayWebGame(const char* tmp )
{
	tmp += 8;
	if(*tmp == '?')
		tmp++;

	string gid		= GetKeyValueStrUnescape(tmp, "gid");
	string strName	= GetKeyValueStrUnescape(tmp, "gname");

	TAB_ITEM tItem;
	tItem.eumType = TAB_WEBGAME;
	tItem.strName = strName;
	tItem.strParam = gid;
	GLOBAL_TABBARDATA->ITabBar_ChangeTab(tItem);
}
