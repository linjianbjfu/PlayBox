#include "stdafx.h"
#include "GameDataImp.h"
#include "YL_StringUtil.h"
#include "YL_DirInfo.h"
#include "YL_FileInfo.h"
#include "tools.h"
#include "../../util/MarkupArchive.h"
#include "../PlayedGamePanel/LMCStructs.h"
#include <assert.h>
#include "../UserMan/UserManager.h"
#include "../UserMan/UserInfo.h"
#include "AppConfig/config/ConfigSettingDef.h"
#include "YL_HTTPRequest.h"
#include "Global/GlobalSwfPath.h"

CGameDataImp*	CGameDataImp::m_pData = NULL;
#define MAX_TAB_COUNT	3

CGameDataImp::CGameDataImp() : m_pVecGame(&m_vecLocalGame){}

IData* CGameDataImp::GetInstance()
{
	if(m_pData == NULL)
		m_pData = new CGameDataImp;
	return m_pData;
}

void CGameDataImp::DelInstance()
{
	if(m_pData != NULL)
		delete m_pData;
	m_pData = NULL;
}

void CGameDataImp::DataAppStart()
{
	std::string strSwfPath;
	GlobalSwfPath::GetConfigSwfPath(strSwfPath);
	if (strSwfPath[strSwfPath.length()-1] != '\\')
		strSwfPath += '\\';
	YL_StringUtil::Format( m_strDataFilePath, "%s\\usergame.xml", strSwfPath.c_str() );
	LoadGameData();
}

void CGameDataImp::DataAppExit()
{
	UnLoadGameData();
}

static void MergeXml (CString strMaster, CString strUpdate)
{
	CMarkupArchive xmlMaster;
	CMarkupArchive xmlUpdate;

	// safe check
	if (!xmlMaster.Open (strMaster.GetBuffer ()))
		return;
	if (!xmlUpdate.Open (strUpdate.GetBuffer ()))
		return;

	xmlMaster.ResetPos();
	xmlMaster.FindElem();
	xmlMaster.IntoElem();

	xmlUpdate.ResetPos ();
	xmlUpdate.FindElem ();
	xmlUpdate.IntoElem ();

	while (xmlUpdate.FindElem ())
	{
		CString strElemName = xmlUpdate.GetTagName ();
		xmlMaster.AddElem (strElemName);

		CString strID    = xmlUpdate.GetAttrib( "id" );
		CString strName  = xmlUpdate.GetAttrib( "name" );
		CString strIntro  = xmlUpdate.GetAttrib( "intro" );
		CString strPicPath = xmlUpdate.GetAttrib( "pic" );
		CString strGamePath = xmlUpdate.GetAttrib( "swf" );
		CString strLastPlayTime = xmlUpdate.GetAttrib( "lastplaytime" );
		CString strPlayCount = xmlUpdate.GetAttrib( "playcount" );
		CString strGameType = xmlUpdate.GetAttrib ("type");
		CString strMD5 = xmlUpdate.GetAttrib ("MD5");

		xmlMaster.SetAttrib ("id", strID);
		xmlMaster.SetAttrib ("name", strName);
		xmlMaster.SetAttrib ("intro", strIntro);
		xmlMaster.SetAttrib ("pic", strPicPath);
		xmlMaster.SetAttrib ("swf", strGamePath);
		xmlMaster.SetAttrib ("lastplaytime", strLastPlayTime);
		xmlMaster.SetAttrib ("playcount", strPlayCount);
		xmlMaster.SetAttrib ("type", strGameType);
		xmlMaster.SetAttrib ("MD5", strMD5);
	}
	xmlMaster.Save ();
	xmlMaster.Close ();
	xmlUpdate.Close ();
}


void CGameDataImp::LoadGameData()
{
	m_pVecGame->clear();
	YL_Log( "CGameDataImp.txt", LOG_DEBUG, "LoadGameData","m_strDataFilePath:%s", m_strDataFilePath.c_str() );

	CMarkupArchive xml;
	if( !xml.Open(m_strDataFilePath.c_str()) )
		return;

	YL_Log( "CGameDataImp.txt", LOG_DEBUG, "LoadGameData","CMarkupArchive::Open success" );

	char szHomePath[MAX_PATH];
	CLhcImg::GetHomePath( szHomePath, MAX_PATH );

	xml.ResetPos();
	xml.FindElem();
	xml.IntoElem();

	while( xml.FindChildElem() )
	{
		OneGame olg;
		olg.strID    = xml.GetAttrib( "Id" );
		olg.strName  = xml.GetAttrib( "Name" );
		olg.strIntro  = xml.GetAttrib( "Intro" );
		//YL_Log( "CGameDataImp.txt", LOG_DEBUG, "LoadGameData","id:%s|name:%s|intro:%s",
		//	olg.strID.c_str(), olg.strName.c_str(), olg.strIntro.c_str() );
		olg.strPicPath = xml.GetAttrib( "Img" );
		olg.strGamePath = xml.GetAttrib( "AppPath" );
		olg.strSrvID = xml.GetAttrib("srvid");
		olg.strAddTime = xml.GetAttrib("add_time");
		CString strGameType = xml.GetAttrib( _T("GameType") );
		olg.nGameType = atoi(strGameType.GetBuffer(0) );
		strGameType.ReleaseBuffer();

		if( YL_FileInfo::IsValid( olg.strGamePath ) )
			m_pVecGame->push_back( olg );
		else if ( 0 == strnicmp(olg.strGamePath.c_str(), "http://", strlen("http://")) )
			m_pVecGame->push_back( olg );
	}
	xml.Close();
}

void CGameDataImp::UnLoadGameData()
{
	string strTmp = m_strDataFilePath + ".1";
	string strContent = "<?xml version=\"1.0\" encoding=\"GBK\"?><root/>";

	string strFileDir;
	YL_FileInfo::GetFileDirPath( strTmp, strFileDir );
	YL_DirInfo::MakeDir( strFileDir );

	YL_FileInfo::SetFileContent( strTmp, strContent );
	
	CMarkupArchive xml;
	if( !xml.Open(strTmp.c_str()) )
		return;

	xml.ResetPos();
	xml.FindElem();

	for( GameList::iterator it1 = m_pVecGame->begin(); it1 != m_pVecGame->end(); it1++ )
	{
		xml.AddChildElem( "app" );		
		xml.IntoElem();
		xml.SetAttrib( "Id", it1->strID.c_str() );
		xml.SetAttrib( "Name", it1->strName.c_str() );
		xml.SetAttrib( "Intro", it1->strIntro.c_str() );
		xml.SetAttrib( "Img", it1->strPicPath.c_str() );
		xml.SetAttrib( "AppPath", it1->strGamePath.c_str() );
		xml.SetAttrib( "srvid", it1->strSrvID.c_str());
		xml.SetAttrib( "add_time", it1->strAddTime.c_str());

		CString str;
		str.Format(_T("%d"), it1->nGameType);
		xml.SetAttrib( _T("GameType"), str.GetBuffer(0));
		str.ReleaseBuffer();
		xml.OutOfElem();
	}
	xml.Close();
	xml.Save();

	CopyFile( strTmp.c_str(), m_strDataFilePath.c_str(), FALSE );
}

bool CGameDataImp::IGameData_GetGameByID(const std::string& strID, int nGameType, OneGame& og)
{
	assert(!((nGameType & OneGame::WEB_GAME) && (nGameType & OneGame::FLASH_GAME)));
	GameList::iterator it1 = m_pVecGame->begin();
	for( ; it1 != m_pVecGame->end(); it1 ++ )
	{
		if((nGameType & it1->nGameType) && strID == it1->strID)
			break;
	}
	if( it1 != m_pVecGame->end() )
	{
		if (it1->nGameType & OneGame::WEB_GAME)
		{
			og = *it1;
			return true;
		}
		else if (it1->nGameType & OneGame::FLASH_GAME)
		{
			if( YL_FileInfo::IsValid( it1->strGamePath ) )
			{
				og = *it1;
				return true;
			}else
			{			
				m_pVecGame->erase( it1 );
				og.clear();
				return false;
			}
		}
	}else
	{
		og.clear();
		return false;
	}
}

bool CGameDataImp::IGameData_AddGame(const OneGame& og)
{
	assert((og.nGameType & OneGame::FLASH_GAME) || (og.nGameType & OneGame::WEB_GAME));
	int iFlashOrWeb = (og.nGameType & OneGame::FLASH_GAME) ? OneGame::FLASH_GAME : OneGame::WEB_GAME;

	//删除原有的信息
	GameList::iterator it1 = m_pVecGame->begin();
	for(; it1 != m_pVecGame->end(); it1 ++ )
		if((iFlashOrWeb & it1->nGameType) && og.strID == it1->strID)
			break;

	if( it1 != m_pVecGame->end() )
		return false;

	OneGame olg = og;
	// 获取系统日期
	struct tm* tmLocal;
	time_t t = time(NULL);;
	tmLocal = localtime(&t);

	YL_StringUtil::Format(olg.strAddTime, "%d-%d-%d %d:%d",
		tmLocal->tm_year+1900, tmLocal->tm_mon+1, tmLocal->tm_mday, // date
		tmLocal->tm_hour, tmLocal->tm_min);// time
	
	//加入本地游戏数据
	m_pVecGame->push_back( olg );
	//玩过的游戏告知server
	//这里添加的必定是flashgame
	if ((olg.nGameType & OneGame::FLASH_GAME) ||
		(olg.nGameType & OneGame::WEB_GAME) )
	{
		GameType gt = FLASH;
		if (olg.nGameType & OneGame::FLASH_GAME)
			gt = FLASH;
		else if (olg.nGameType & OneGame::WEB_GAME)
			gt = WEB;
		IGameData_UserGameChangeInfoToSvr(gt, ADD, PLAYED, olg.strID, olg.strSrvID);
	}
	return true;
}

bool CGameDataImp::IGameData_DelGame(const string& strID, int nGameType)
{
	assert((nGameType & OneGame::FLASH_GAME) || (nGameType & OneGame::WEB_GAME));
	int iFlashOrWeb = (nGameType & OneGame::FLASH_GAME) ? OneGame::FLASH_GAME : OneGame::WEB_GAME;

	GameList::iterator it1 = m_pVecGame->begin();
	for( ; it1 != m_pVecGame->end(); it1 ++ )
		if((iFlashOrWeb & it1->nGameType) && strID == it1->strID)
			break;

	if( it1 != m_pVecGame->end() )
	{
		//删除图片
		::DeleteFile( it1->strPicPath.c_str() );
		//删除swf文件
		::DeleteFile( it1->strGamePath.c_str() );

		m_pVecGame->erase( it1 );

		//删除游戏告知server
		if ((it1->nGameType | OneGame::FLASH_GAME) ||
			(it1->nGameType | OneGame::WEB_GAME))
		{
			GameType gt = FLASH;
			if (it1->nGameType | OneGame::FLASH_GAME)
				gt = FLASH;
			else if (it1->nGameType | OneGame::WEB_GAME)
				gt = WEB;
			IGameData_UserGameChangeInfoToSvr(gt, ADD, PLAYED, it1->strID, it1->strSrvID);
		}		
		return true;
	}
	return false;
}

bool CGameDataImp::IGameData_GetGame(GameList& lgl, int iGameType)
{
	lgl.clear();
	assert(!((iGameType & OneGame::COLLECTED) && (iGameType & OneGame::RECENT_PLAY)));

	GameList::iterator it = m_pVecGame->begin();
	for (; it != m_pVecGame->end(); it++)
	{
		if (iGameType & OneGame::FLASH_GAME && 
			it->nGameType & OneGame::FLASH_GAME)
		{
			lgl.push_back(*it);
			continue;
		}
		if (iGameType & OneGame::WEB_GAME && 
			it->nGameType & OneGame::WEB_GAME)
		{
			lgl.push_back(*it);
			continue;
		}
		if (iGameType & OneGame::COLLECTED && 
			it->nGameType & OneGame::COLLECTED)
		{
			lgl.push_back(*it);
			continue;
		}
		if (iGameType & OneGame::RECENT_PLAY && 
			it->nGameType & OneGame::RECENT_PLAY)
		{
			lgl.push_back(*it);
			continue;
		}
	}
	return true;
}

unsigned int CGameDataImp::IGameData_GetGameCount()
{
	return (unsigned int)(m_pVecGame->size());
}

void CGameDataImp::IGameData_SetLoginGameList(GameList& lgl)
{
	m_vecLoginGame.clear();
	std::copy(lgl.begin(), lgl.end(), back_inserter(m_vecLoginGame));
}

void CGameDataImp::IGameData_ChangeLoginState(bool bLogin)
{
	m_bLogin = bLogin;
	m_pVecGame = m_bLogin ? &m_vecLoginGame : &m_vecLocalGame;
}

/*
http://box.najiuwan.com/index.php?s=/Gamelist/user_game/
username/yinshaohua7777/pass/11111/type/flashgame/action/add/
gameid/3/status/played

username:yinshaohua
pass:md5(密码)
type：flashgame，webgame
action:add,del
status:played(玩过),collect(收藏)
gameid:107svrid:1
返回成功：{"success":1}，失败{"success":0}
*/
void CGameDataImp::IGameData_UserGameChangeInfoToSvr(GameType gt, 
	Action action, Status s, const std::string& strGameID, const std::string& strSvrID) 
{
	if (!CUserManager::GetInstance()->User_IsLogin())
		return;
	
	std::string strSvr;
	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME,
		CONF_SETTING_CONFIG_COLLECTE_GAME_ACTION_REPORT_URL, strSvr);
	if (strSvr.empty())
		return;

	std::string strType;
	switch (gt)
	{
	case FLASH:
		strType = "flashgame";
		break;
	case WEB:
		strType = "webgame";
		break;
	}
	std::string strAction;
	switch (action)
	{
	case ADD:
		strAction = "add";
		break;
	case DEL:
		strAction = "del";
		break;
	}
	std::string strStatus;
	switch (s)
	{
	case PLAYED:
		strStatus = "played";
		break;
	case COLLECT:
		strStatus = "collect";
		break;
	}
	
	std::string strUrl;
	YL_StringUtil::Format(strUrl, 
		"%ss=/Gamelist/user_game/username/%s/pass/%s/type/%s/action/%s/gameid/%s/svrid/%s/status/%s",
		strSvr.c_str(),
		CUserManager::GetInstance()->User_GetUserInfo()->strName.c_str(),
		CUserManager::GetInstance()->User_GetUserInfo()->strPassMD5.c_str(),
		strType.c_str(),
		strAction.c_str(),
		strGameID.c_str(),
		strSvrID.c_str(),
		strStatus.c_str()
		);
	YL_CHTTPRequest http;
	bool bAsync = false;
	if (bAsync)
	{
		http.SendRequest(strUrl, YL_CHTTPRequest::REQUEST_GET);
	}else
	if (http.Request(strUrl, YL_CHTTPRequest::REQUEST_GET))
	{
		BYTE *pbyIndex = NULL;
		long size = 0;
		pbyIndex = http.GetContent(size);
		TRACE("OK");
	}
}