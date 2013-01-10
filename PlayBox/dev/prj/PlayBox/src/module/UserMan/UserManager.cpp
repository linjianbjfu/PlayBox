#include "stdafx.h"
#include "UserManager.h"
#include "YL_Base64.h"
#include "../../AppConfig/config/ConfigSettingDef.h"
#include "../../DataInterface/IUserMsgObserver.h"

static CUserManager* g_pUserMgr = NULL;

CUserManager::CUserManager()
  : m_hThreadLogIn(NULL),
    m_pUserInfo(NULL)
{
}

CUserManager*	CUserManager::GetInstance()
{
	if(!g_pUserMgr)
		g_pUserMgr = new CUserManager();
	return g_pUserMgr;
}

DWORD WINAPI CUserManager::Thread_LogIn(void*ppar)
{
	YL_CHTTPRequest* httpRequest = new YL_CHTTPRequest;

	char szUserID[256];
	char szPassword[256];
	char szAfterBase64DecodePassword[256];	

	memset(szUserID,0,256);
	memset(szPassword,0,256);
	memset(szAfterBase64DecodePassword,0,256);
	string strValue = "";
	AfxGetUserConfig()->GetConfigStringValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_LOGIN_USER_NAME, strValue );
	//��ʽΪershor
	_snprintf( szUserID, 256 - 1, "%s", strValue.c_str() );

	strValue = "";
	AfxGetUserConfig()->GetConfigStringValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_LOGIN_PASSWORD, strValue );
	_snprintf( szPassword, 256 - 1, "%s", strValue.c_str() );
	//�����������pass��base64����
	//������decode
	YL_Base64Decode( szAfterBase64DecodePassword, szPassword );

	if( strlen(szUserID) ==0 || strlen(szAfterBase64DecodePassword) == 0 )
	{
		list<IMessageObserver*> listOb;
		AfxGetMessageManager()->QueryObservers( KWID_MESSAGE_USER,listOb);
		for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
		{
			IUserMsgObserver* pOb = dynamic_cast<IUserMsgObserver*>(*itOb);
			pOb->UserMsg_LogReshow();
		}

		delete httpRequest;
		return 1;
	}
	CUserManager::Login(&httpRequest,szUserID,szAfterBase64DecodePassword, true);
	delete httpRequest;
	return 0;
}

void CUserManager::User_AppStartUp(BOOL bLogin)
{
	if(bLogin)
		GetInstance()->m_hThreadLogIn = CreateThread(0,0,Thread_LogIn,0,0,0);	
}

void CUserManager::User_Login(LPCSTR pszName,LPCSTR pszPwd)
{
	DWORD dwThreadExitCode = 0;
	if( GetInstance()->m_hThreadLogIn != NULL && 
		GetExitCodeThread(GetInstance()->m_hThreadLogIn,&dwThreadExitCode) && 
		dwThreadExitCode == STILL_ACTIVE )
	{
		TerminateThread(GetInstance()->m_hThreadLogIn,1);
		WaitForSingleObject(GetInstance()->m_hThreadLogIn,1000);
		CloseHandle(GetInstance()->m_hThreadLogIn);
	}
	memset(GetInstance()->m_szTmpName,0,sizeof(GetInstance()->m_szTmpName));
	memset(GetInstance()->m_szTmpPWD,0,sizeof(GetInstance()->m_szTmpPWD));
	strcpy(GetInstance()->m_szTmpName,pszName);
	strcpy(GetInstance()->m_szTmpPWD,pszPwd);
	GetInstance()->m_hThreadLogIn = CreateThread(0,0,Thread_LogInFromFlash,0,0,0);	
}

void CUserManager::User_AppExit()
{
	
	DWORD dwThreadExitCode = 0;
	if( GetInstance()->m_hThreadLogIn != NULL && 
		GetExitCodeThread(GetInstance()->m_hThreadLogIn,&dwThreadExitCode) && 
		dwThreadExitCode == STILL_ACTIVE )
	{
		TerminateThread(GetInstance()->m_hThreadLogIn,1);
		WaitForSingleObject(GetInstance()->m_hThreadLogIn,1000);
		CloseHandle(GetInstance()->m_hThreadLogIn);
	}

	User_Logout();
	DeleteInstance();
}

void CUserManager::User_Logout()
{
    list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_USER,listOb);
	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IUserMsgObserver* pOb = dynamic_cast<IUserMsgObserver*>(*itOb);
		pOb->UserMsg_LogOut();
	}
}

BOOL CUserManager::User_IsLogin()
{
	return GLOBAL_USERDATA->User_IsLogin();
}

void CUserManager::User_LoginFaild()
{
	CUserManager* pUserMan = CUserManager::GetInstance();
	if( pUserMan == NULL )
	{
		return ;
	}
	GLOBAL_USERDATA->User_SetLogin(false);
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( KWID_MESSAGE_USER,listOb);
	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IUserMsgObserver* pOb = dynamic_cast<IUserMsgObserver*>(*itOb);
		pOb->UserMsg_LogFaild();
	}
}

CUserManager::LOGIN_STATUS CUserManager::Login(YL_CHTTPRequest** ppHTTP,const char* lpszRegID,const char* lpszRegPass, bool bAutoLogin)
{
	if( lpszRegID == NULL || lpszRegPass == NULL 
		|| strlen(lpszRegID) == 0 || strlen(lpszRegPass) == 0 )
	{
		list<IMessageObserver*> listOb;
		AfxGetMessageManager()->QueryObservers( KWID_MESSAGE_USER,listOb);
		for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
		{
			IUserMsgObserver* pOb = dynamic_cast<IUserMsgObserver*>(*itOb);
			pOb->UserMsg_LogReshow();
		}
		return CUserManager::LOG_FAILED_OTHER;
	}
	
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( KWID_MESSAGE_USER,listOb);
	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IUserMsgObserver* pOb = dynamic_cast<IUserMsgObserver*>(*itOb);
		pOb->UserMsg_AutoLogin(bAutoLogin);
		pOb->UserMsg_BeginLogin();
	}

	CUserManager* pUserMan = CUserManager::GetInstance();

	char szVersion[128];
	memset(szVersion,0,128);
	CKwcImg::GetSoftwareVersion(szVersion,128);

	int iLen = (int)strlen(lpszRegID);

	char* szEncoderID = new char[ BASE64_LENGTH(iLen) + 1 ];
	YL_Base64Encode(szEncoderID,lpszRegID,(int)strlen(lpszRegID));

	iLen = (int)strlen(lpszRegPass);
	char* szEncoderPass = new char[ BASE64_LENGTH(iLen) + 1 ];

	YL_Base64Encode(szEncoderPass,lpszRegPass,(int)strlen(lpszRegPass));

	iLen = (int)strlen( szVersion );
	char* szEncoderVersion= new char[ BASE64_LENGTH(iLen) + 1 ];
	YL_Base64Encode(szEncoderVersion,szVersion,(int)strlen(szVersion));

	OneSubject *pCur = AfxGetUIManager()->UIGetSkinMgr()->GetCurSubject();
	CString strLog, strFormat;
	strFormat = "skinname=%s";
	if(pCur != NULL)
	{
		if(pCur->nID != 0)
		{
			strLog.Format("%d", pCur->nID);
			strFormat = "skinid=%s";
		}
		else
			strLog.Format("%s", pCur->strShowName.c_str());
	}
	else
	{
		strLog = AfxGetUIManager()->UIGetSkinMgr()->GetSkinPath();
		int pos = strLog.ReverseFind('\\');
		strLog = strLog.Mid(pos + 1);
	}

	string strUserSvr;
	CKwcImg::GetUserServer(strUserSvr);
	char szURL[512];
	memset(szURL,0,512);
	if(bAutoLogin)
		_snprintf(szURL,511,"%s/US/Login?uname=%s&pwd=%s&ver=%s&auto=1&" + strFormat,strUserSvr.c_str(),szEncoderID,szEncoderPass,szEncoderVersion,strLog);
	else
		_snprintf(szURL,511,"%s/US/Login?uname=%s&pwd=%s&ver=%s&" + strFormat ,strUserSvr.c_str(),szEncoderID,szEncoderPass,szEncoderVersion,strLog);

	delete[] szEncoderID;	
	delete[] szEncoderVersion;

	if( (*ppHTTP)->Request( szURL, YL_CHTTPRequest::REQUEST_GET, 20*1000 ) != TRUE)
	{
		delete[] szEncoderPass;
		User_LoginFaild();
		return CUserManager::LOG_FAILED_OTHER;
	}

	BYTE *pbyIndex = NULL;
	long size = 0;
	pbyIndex = (*ppHTTP)->GetContent(size);


	GLOBAL_USERDATA->GetUserInfo()->Clear();
	if( pbyIndex != NULL && *pbyIndex == '1' )
	{
		CUserManager* pUserMan = CUserManager::GetInstance();
		if( pUserMan == NULL )
		{
			delete[] szEncoderPass;
			User_LoginFaild();
			return CUserManager::LOG_FAILED_OTHER;
		}
	
		char szReBuffer[512];
		memset(szReBuffer,0,512);

		if( size >= 512 )
		{	
			size = 511;
		}

		memcpy(szReBuffer,pbyIndex,size);
		szReBuffer[size] = '\0';

		char szNetID[256];
		char* pStart	= NULL;
		char* pEnd		= NULL;
		pStart = strstr(szReBuffer,"\r\n");
		if( pStart == NULL )
		{
			User_LoginFaild();
			goto FAILED;
		}

		pStart += 2;

		strcpy(szNetID,pStart);
		
		GLOBAL_USERDATA->GetUserInfo()->dwUserID = atol(szNetID);
		_tcscpy(GLOBAL_USERDATA->GetUserInfo()->szName,lpszRegID);   //��¼�û���
		_tcscpy(GLOBAL_USERDATA->GetUserInfo()->szPWD,lpszRegPass);  //��¼����
		GLOBAL_USERDATA->User_SetLogin(true);

		string strValue = string( lpszRegID );
		AfxGetUserConfig()->SetConfigStringValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_LOGIN_USER_NAME, strValue, true );
		
		strValue = string( szEncoderPass );
		AfxGetUserConfig()->SetConfigStringValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_LOGIN_PASSWORD, strValue, true );

		strValue = string( szNetID );
		AfxGetUserConfig()->SetConfigStringValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_NETID, strValue, true );

		list<IMessageObserver*> listOb;
		AfxGetMessageManager()->QueryObservers( KWID_MESSAGE_USER,listOb);
		for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
		{
			IUserMsgObserver* pOb = dynamic_cast<IUserMsgObserver*>(*itOb);
			pOb->UserMsg_Login();
		}

		delete[] szEncoderPass;
		return CUserManager::LOG_SUCCEEDED;
	}
	else if( pbyIndex != NULL && *pbyIndex == '0' )
	{
		char szReBuffer[512];
		memset(szReBuffer,0,512);
		
		if( size >= 512 )
		{	
			size = 511;
		}

		memcpy(szReBuffer,pbyIndex,size);
		szReBuffer[size] = '\0';
		delete[] szEncoderPass;

		User_LoginFaild();
		if( strstr(szReBuffer,"USER_NOT_EXIST") != NULL )
		{
			return CUserManager::LOG_FAILED_USER_NOT_EXIST;
		}
		else if( strstr(szReBuffer,"PASSWORD_ERROR") != NULL )
		{
			return CUserManager::LOG_FAILED_PASS_WRONG;
		}
		else
		{
			return CUserManager::LOG_FAILED_OTHER;
		}
	}

FAILED:
	delete[] szEncoderPass;
	return CUserManager::LOG_FAILED_OTHER;
}