#include "stdafx.h"
#include "UserManager.h"
#include "YL_Base64.h"
#include "YL_HTTPRequest.h"
#include "YL_StringUtil.h"
#include "UserInfo.h"
#include "../../AppConfig/config/ConfigSettingDef.h"
#include "../../DataInterface/IUserMsgObserver.h"
#include "../../util/md5.h"
#include "IGameData.h"
#include "YL_FileInfo.h"
#include "../json/json.h"
#include "CDataManager.h"

static CUserManager* g_pUserMgr = NULL;
struct gameType2StringEnumPair
{
	char* szGameType;
	int nGameType;
};
static gameType2StringEnumPair g_gameTypePair[] = 
{
	{"collect_game", OneGame::COLLECTED},
	{"recent_played_game", OneGame::RECENT_PLAY} 
};

#define BEGIN_SET_STATE_AND_NOTIFY(s, e) \
	do { \
		g_pUserMgr->m_state = s; \
		g_pUserMgr->m_errDetail = e; \
		list<IMessageObserver*> listOb; \
		AfxGetMessageManager()->QueryObservers( ID_MESSAGE_USER,listOb); \
		for( list<IMessageObserver*>::iterator itOb = listOb.begin(); \
			itOb != listOb.end();itOb++ ) \
		{ \
			IUserMsgObserver* pOb = dynamic_cast<IUserMsgObserver*>(*itOb);

#define END_SET_STATE_AND_NOTIFY() \
		} \
	} while(0);

struct LoginThreadPara
{
	std::string strUserName;
	std::string strPassWord;
};

CUserManager::CUserManager()
	: m_hThreadLogIn(NULL),
	  m_pUserInfo(NULL),
	  m_state(NOT_LOGGED_IN),
	  m_errDetail(SUCCEEDED) {}

CUserManager* CUserManager::GetInstance()
{
	if(!g_pUserMgr)
		g_pUserMgr = new CUserManager();
	return g_pUserMgr;
}

void CUserManager::User_AppStartUp()
{
	bool bAutoLogin = false;
	AfxGetUserConfig()->GetConfigBoolValue(CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_AUTO_LOGIN, bAutoLogin);
	if(!bAutoLogin)
		return;

	//auto login
	std::string strUserName;
	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_LOGIN_USER_NAME, strUserName);

	if (strUserName.empty())
	{
		BEGIN_SET_STATE_AND_NOTIFY(NOT_LOGGED_IN, USER_NAME_EMPTY)
			pOb->UserMsg_LogFaild();
		END_SET_STATE_AND_NOTIFY()
		return;
	}

	std::string strBase64MD5Pass;
	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_LOGIN_PASSWORD, strBase64MD5Pass);
	if (strBase64MD5Pass.empty())
	{
		BEGIN_SET_STATE_AND_NOTIFY(NOT_LOGGED_IN, PASS_WORD_EMPTY)
			pOb->UserMsg_LogFaild();
		END_SET_STATE_AND_NOTIFY()
		return;
	}

	//配置里读出的pass是base64过的
	//将密码decode
	char szMD5Pass[256];
	ZeroMemory(szMD5Pass, 256);
	YL_Base64Decode( szMD5Pass, strBase64MD5Pass.c_str() );
	g_pUserMgr->UserLoginInternal(strUserName.c_str(), szMD5Pass);
}

void CUserManager::User_Login(LPCSTR pszName, LPCSTR pszPwd)
{
	char szPwdMD5[33];
	ZeroMemory(szPwdMD5, 33);

	MD5String(const_cast<char*>(pszPwd), szPwdMD5);
	UserLoginInternal(pszName, szPwdMD5);
}

void CUserManager::UserLoginInternal(LPCSTR pszName, LPCSTR pszPwdMD5)
{
	User_CancelLog();
	LoginThreadPara* pPara = new LoginThreadPara;
	pPara->strUserName = pszName;
	pPara->strPassWord = pszPwdMD5;
	g_pUserMgr->m_hThreadLogIn = CreateThread(0,0,ThreadLogin,pPara,0,0);	
}

void CUserManager::User_CancelLog()
{
	DWORD dwThreadExitCode = 0;
	if( g_pUserMgr->m_hThreadLogIn != NULL && 
		GetExitCodeThread(g_pUserMgr->m_hThreadLogIn,&dwThreadExitCode) && 
		dwThreadExitCode == STILL_ACTIVE )
	{
		TerminateThread(g_pUserMgr->m_hThreadLogIn,1);
		WaitForSingleObject(g_pUserMgr->m_hThreadLogIn,1000);
		CloseHandle(g_pUserMgr->m_hThreadLogIn);
		m_hThreadLogIn = NULL;
		BEGIN_SET_STATE_AND_NOTIFY(NOT_LOGGED_IN, USER_CANCEL)
			pOb->UserMsg_LogFaild();
		END_SET_STATE_AND_NOTIFY()
	}
}

void CUserManager::User_AppExit()
{
	User_CancelLog();
	delete g_pUserMgr;
	g_pUserMgr = NULL;
}

void CUserManager::User_Logout()
{
	GLOBAL_GAME->IGameData_ChangeLoginState(false);
	BEGIN_SET_STATE_AND_NOTIFY(NOT_LOGGED_IN, SUCCEEDED)
		pOb->UserMsg_LogOut();
	END_SET_STATE_AND_NOTIFY()
}

bool CUserManager::User_IsLogin()
{
	return m_state == HAVE_LANDED;
}

DWORD CUserManager::ThreadLogin(void* pPara)
{
	LoginThreadPara* threadPara = (LoginThreadPara*)pPara;
	BEGIN_SET_STATE_AND_NOTIFY(LOGIN_ON_THE_WAY, SUCCEEDED)
		pOb->UserMsg_BeginLogin();
	END_SET_STATE_AND_NOTIFY()

	std::string strUserSvr;
	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME,
		CONF_SETTING_CONFIG_USER_SVR, strUserSvr);
	std::string strUrl;
	YL_StringUtil::Format(strUrl, "%susername/%s/pass/%s",
		strUserSvr.c_str(), threadPara->strUserName.c_str(),
		threadPara->strPassWord.c_str());

	YL_CHTTPRequest http;
	if(!http.Request( strUrl, YL_CHTTPRequest::REQUEST_GET, 20*1000 ))
	{
		delete threadPara;
		BEGIN_SET_STATE_AND_NOTIFY(LOGIN_ON_THE_WAY, NET_ERROR)
			pOb->UserMsg_LogFaild();
		END_SET_STATE_AND_NOTIFY()
	}
	//登陆成功
	BYTE *pbyIndex = NULL;
	long size = 0;
	pbyIndex = http.GetContent(size);

	if(pbyIndex && size != 0)
	{
		std::string strContent = std::string((char*)pbyIndex, size);
		//解析json
		strContent = YL_FileInfo::GetFileContent("D:\\temp.json");
		bool bLoginSuc = false;
		ParseJson(strContent, bLoginSuc);
		if (bLoginSuc)
		{
			SetUserInfo(threadPara->strUserName.c_str(), threadPara->strPassWord.c_str());
			AfxGetUserConfig()->SetConfigStringValue( CONF_SETTING_MODULE_NAME, 
				CONF_SETTING_LOGIN_USER_NAME, g_pUserMgr->m_pUserInfo->strName, true );

			char szBase64MD5[256];
			ZeroMemory(szBase64MD5, 256);
			YL_Base64Encode( szBase64MD5, g_pUserMgr->m_pUserInfo->strPassMD5.c_str(),
				g_pUserMgr->m_pUserInfo->strPassMD5.length());
			AfxGetUserConfig()->SetConfigStringValue( CONF_SETTING_MODULE_NAME, 
				CONF_SETTING_LOGIN_PASSWORD, szBase64MD5, true );

			BEGIN_SET_STATE_AND_NOTIFY(HAVE_LANDED, SUCCEEDED)
				pOb->UserMsg_Login();
			END_SET_STATE_AND_NOTIFY()
		} else
		{
			BEGIN_SET_STATE_AND_NOTIFY(NOT_LOGGED_IN, SERVER_SAID_LOGIN_FAIL)
				pOb->UserMsg_LogFaild();
			END_SET_STATE_AND_NOTIFY()
		}
	}else
	{
		BEGIN_SET_STATE_AND_NOTIFY(NOT_LOGGED_IN, NET_ERROR)
			pOb->UserMsg_LogFaild();
		END_SET_STATE_AND_NOTIFY()
	}
	return 0;
}

void CUserManager::SetUserInfo(LPCSTR lpszUserName, LPCSTR lpszPass)
{
	if (!g_pUserMgr->m_pUserInfo)
		g_pUserMgr->m_pUserInfo = new UserInfo;

	g_pUserMgr->m_pUserInfo->strName = lpszUserName;
	g_pUserMgr->m_pUserInfo->strPassMD5 = lpszPass;
}
//parse the content received from server, and set bLoginSuc whether decided by server content
void CUserManager::ParseJson(const std::string strJson, bool& bLoginSuc)
{
	bLoginSuc = false;
	Json::Reader reader;
	Json::Value root;  
	if (reader.parse(strJson, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素  
	{
		bLoginSuc = root["login_success"].asBool();
		if (!bLoginSuc)
			return;
		
		GameList lgl;
		for (int j=0; j<sizeof(g_gameTypePair)/sizeof(gameType2StringEnumPair); j++)
		{
			Json::Value jsonGame = root[g_gameTypePair[j].szGameType];
			if (jsonGame.isArray())
			{
				for (int i=0; i<jsonGame.size(); i++)
				{
					OneGame olg;
					std::string strGameType = jsonGame[i]["type"].asString();
					olg.nGameType = g_gameTypePair[j].nGameType;
					if (strGameType == "flashgame")
					{
						olg.nGameType |= OneGame::FLASH_GAME;
						olg.strGamePath = jsonGame[i]["swf_url"].asString();
					} else 
					if (strGameType == "webgame")
					{
						olg.nGameType |= OneGame::WEB_GAME;
						olg.strSrvID = jsonGame[i]["serverid"].asString();
					} else 
						continue;

					olg.strName = jsonGame[i]["name"].asString();
					olg.strPicPath = jsonGame[i]["thumbnail_url"].asString();					
					olg.strID = jsonGame[i]["id"].asString();
					lgl.push_back(olg);
				}
			}		
		}
		if (!lgl.empty())
			GLOBAL_GAME->IGameData_SetLoginGameList(lgl);
		GLOBAL_GAME->IGameData_ChangeLoginState(true);
	}
}

UserInfo* CUserManager::User_GetUserInfo() const
{
	return m_pUserInfo;
}