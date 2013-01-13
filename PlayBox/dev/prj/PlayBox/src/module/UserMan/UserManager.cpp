#include "stdafx.h"
#include "UserManager.h"
#include "YL_Base64.h"
#include "YL_HTTPRequest.h"
#include "YL_StringUtil.h"
#include "UserInfo.h"
#include "../../AppConfig/config/ConfigSettingDef.h"
#include "../../DataInterface/IUserMsgObserver.h"
#include "../../util/md5.h"

static CUserManager* g_pUserMgr = NULL;

#define NOTIFY(method) \
{ \
	list<IMessageObserver*> listOb; \
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_USER,listOb); \
	for( list<IMessageObserver*>::iterator itOb = listOb.begin(); \
		itOb != listOb.end();itOb++ ) \
	{ \
		IUserMsgObserver* pOb = dynamic_cast<IUserMsgObserver*>(*itOb); \
		pOb->method; \
	} \
}

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
		g_pUserMgr->m_errDetail = USER_NAME_EMPTY;
		NOTIFY(IUserMsgObserver::UserMsg_LogFaild());
		return;
	}

	std::string strBase64MD5Pass;
	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_LOGIN_PASSWORD, strBase64MD5Pass);
	if (strBase64MD5Pass.empty())
	{
		g_pUserMgr->m_errDetail = PASS_WORD_EMPTY;
		NOTIFY(IUserMsgObserver::UserMsg_LogFaild());
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
		m_state = NOT_LOGGED_IN;
		m_errDetail = USER_CANCEL;
		NOTIFY(IUserMsgObserver::UserMsg_LogFaild());
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
	m_state = NOT_LOGGED_IN;
	m_errDetail = SUCCEEDED;

	NOTIFY(IUserMsgObserver::UserMsg_LogOut());
}

bool CUserManager::User_IsLogin()
{
	return m_state == HAVE_LANDED;
}

DWORD CUserManager::ThreadLogin(void* pPara)
{
	LoginThreadPara* threadPara = (LoginThreadPara*)pPara;
	NOTIFY(IUserMsgObserver::UserMsg_BeginLogin());

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
		NOTIFY(IUserMsgObserver::UserMsg_LogFaild());
	}
	//登陆成功
	BYTE *pbyIndex = NULL;
	long size = 0;
	pbyIndex = http.GetContent(size);

	if(pbyIndex && size != 0)
	{
		//解析pbyIndex
		//不符合格式，或明确说登陆失败
		if (!g_pUserMgr->m_pUserInfo)
			g_pUserMgr->m_pUserInfo = new UserInfo;

		g_pUserMgr->m_pUserInfo->strName = threadPara->strUserName;
		g_pUserMgr->m_pUserInfo->strPassMD5 = threadPara->strPassWord;
		g_pUserMgr->m_state = HAVE_LANDED;
		g_pUserMgr->m_errDetail = SUCCEEDED;

		std::string strContent = std::string((char*)pbyIndex, size);
		//解析json

		g_pUserMgr->m_state = HAVE_LANDED;

		AfxGetUserConfig()->SetConfigStringValue( CONF_SETTING_MODULE_NAME, 
			CONF_SETTING_LOGIN_USER_NAME, g_pUserMgr->m_pUserInfo->strName, true );
		
		char szBase64MD5[256];
		ZeroMemory(szBase64MD5, 256);
		YL_Base64Encode( szBase64MD5, g_pUserMgr->m_pUserInfo->strPassMD5.c_str(),
			g_pUserMgr->m_pUserInfo->strPassMD5.length());
		AfxGetUserConfig()->SetConfigStringValue( CONF_SETTING_MODULE_NAME, 
			CONF_SETTING_LOGIN_PASSWORD, szBase64MD5, true );

		NOTIFY(IUserMsgObserver::UserMsg_Login());
	}else
	{
		g_pUserMgr->m_state = NOT_LOGGED_IN;
		g_pUserMgr->m_errDetail = FAILED;
		NOTIFY(IUserMsgObserver::UserMsg_LogFaild());
	}
	return 0;
}