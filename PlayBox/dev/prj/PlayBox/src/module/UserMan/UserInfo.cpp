#include "stdafx.h"
#include "UserMan.h"
#include "YL_Base64.h"
#include "../../AppConfig/config/ConfigSettingDef.h"
#include "../../DataInterface/IUserMsgObserver.h"
#include "../../DataInterface/IUserData.h"

UserMan* UserMan::m_pSingleUserMan = NULL;
extern BOOL g_bWWStarted;

UserMan::UserMan():m_hThreadLogIn(NULL),m_dwLogIn(0),m_dwLogOut(0)
{	
	memset(m_szTmpName,0,sizeof(m_szTmpName));
	memset(m_szTmpPWD,0,sizeof(m_szTmpPWD));
	
}

void UserMan::CreateInstance()
{
	if( m_pSingleUserMan == NULL )
	{
		m_pSingleUserMan = new UserMan;
	}
}

void UserMan::DeleteInstance()
{
	if( m_pSingleUserMan != NULL )
	{
		delete m_pSingleUserMan;
	}
}

UserMan*	UserMan::GetInstance()
{
	if( m_pSingleUserMan == NULL )
	{
		CreateInstance();
	}
	return m_pSingleUserMan;
}
BOOL UserMan::IsCreateInstance()
{
	return m_pSingleUserMan != NULL;
}

DWORD WINAPI UserMan::Thread_LogInFromFlash(void*ppar)
{
	YL_CHTTPRequest* httpRequest = new YL_CHTTPRequest;
	if( strlen(GetInstance()->m_szTmpName) ==0 || strlen(GetInstance()->m_szTmpPWD) == 0 )
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
	UserMan::Login(&httpRequest,GetInstance()->m_szTmpName,GetInstance()->m_szTmpPWD);
	delete httpRequest;
	return 0;
}


DWORD WINAPI UserMan::Thread_LogIn(void*ppar)
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
	//格式为ershor
	_snprintf( szUserID, 256 - 1, "%s", strValue.c_str() );

	strValue = "";
	AfxGetUserConfig()->GetConfigStringValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_LOGIN_PASSWORD, strValue );
	_snprintf( szPassword, 256 - 1, "%s", strValue.c_str() );
	//配置里读出的pass是base64过的
	//将密码decode
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
	UserMan::Login(&httpRequest,szUserID,szAfterBase64DecodePassword, true);
	delete httpRequest;
	return 0;
}

void UserMan::User_AppStartUp(BOOL bLogin)
{
	bool bValue = false;
	AfxGetUserConfig()->GetConfigBoolValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_AUTO_LOG, bValue );
	if( (bValue && !g_bWWStarted) || bLogin)
	{
		GetInstance()->m_hThreadLogIn = CreateThread(0,0,Thread_LogIn,0,0,0);	
		return;
	}
}

void UserMan::User_Login(LPCSTR pszName,LPCSTR pszPwd)
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

void UserMan::User_PrepareExit()
{
	HWND hWnd = ::FindWindow(NULL,_T("酷我提示"));
	if(hWnd)
		::PostMessage(hWnd,WM_CLOSE,0,0);
}

void UserMan::User_AppExit()
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

void UserMan::User_Login()
{
	if (s_bIsLoginDlgOpened)
	{
		return;
	}
	s_bIsLoginDlgOpened=TRUE;
	CDlgLogin dlgLogin(AfxGetMainWindow());
	bool bValue = false;
	AfxGetUserConfig()->GetConfigBoolValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_AUTO_LOG, bValue );
	dlgLogin.m_bAutoLog = bValue ? TRUE : FALSE;
	
	if(dlgLogin.DoModal() == IDOK)
	{
		bValue = ( dlgLogin.m_bAutoLog == TRUE );
		AfxGetUserConfig()->SetConfigBoolValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_AUTO_LOG, bValue, true );

		bValue = ( dlgLogin.m_bDontRemPsw == TRUE );
		AfxGetUserConfig()->SetConfigBoolValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_DONT_REM_PASSWORD, bValue,true );
	}

	s_bIsLoginDlgOpened=FALSE;
}

void UserMan::User_Logout()
{
	UserMan* pUserMan = UserMan::GetInstance();
	//if( !pUserMan->m_pUserData->User_IsLogin() )
	//{
	//	return;
	//}

	GLOBAL_USERDATA->GetUserInfo()->Clear();
	GLOBAL_USERDATA->User_SetLogin(false);
	pUserMan->m_dwLogOut = GetTickCount();

	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( KWID_MESSAGE_USER,listOb);
	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IUserMsgObserver* pOb = dynamic_cast<IUserMsgObserver*>(*itOb);
		pOb->UserMsg_LogOut();
	}

	if( !GLOBAL_USERDATA->User_IsLogin() )
	{
		Log2Server::Log_LogTime( (pUserMan->m_dwLogOut - pUserMan->m_dwLogIn) / 1000 );
	}
}

BOOL UserMan::User_IsLogin()
{
	return GLOBAL_USERDATA->User_IsLogin();
}

void UserMan::User_LoginFaild()
{
	UserMan* pUserMan = UserMan::GetInstance();
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

UserMan::LOGIN_STATUS UserMan::Login(YL_CHTTPRequest** ppHTTP,const char* lpszRegID,const char* lpszRegPass, bool bAutoLogin)
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
		return UserMan::LOG_FAILED_OTHER;
	}
	
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( KWID_MESSAGE_USER,listOb);
	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IUserMsgObserver* pOb = dynamic_cast<IUserMsgObserver*>(*itOb);
		pOb->UserMsg_AutoLogin(bAutoLogin);
		pOb->UserMsg_BeginLogin();
	}

	UserMan* pUserMan = UserMan::GetInstance();

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
		//Log( UserMan::LOG_FILE, LOG_NOTICE, "Login", "---网络连接失败");
		delete[] szEncoderPass;
		User_LoginFaild();
		return UserMan::LOG_FAILED_OTHER;
	}

	BYTE *pbyIndex = NULL;
	long size = 0;
	pbyIndex = (*ppHTTP)->GetContent(size);


	GLOBAL_USERDATA->GetUserInfo()->Clear();
	if( pbyIndex != NULL && *pbyIndex == '1' )
	{
		
		//Log( UserMan::LOG_FILE, LOG_NOTICE, "Login", "登录成功");

		UserMan* pUserMan = UserMan::GetInstance();
		if( pUserMan == NULL )
		{
			delete[] szEncoderPass;
			User_LoginFaild();
			return UserMan::LOG_FAILED_OTHER;
		}
	
		
		pUserMan->m_dwLogIn = GetTickCount();

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
//		pEnd = strstr(pStart+1,"\r\n");
//		if( pEnd == NULL )
//		{
//			goto FAILED;
//		}

//		*pEnd = '\0';
		strcpy(szNetID,pStart);
		
		GLOBAL_USERDATA->GetUserInfo()->dwUserID = atol(szNetID);
		_tcscpy(GLOBAL_USERDATA->GetUserInfo()->szName,lpszRegID);   //记录用户名
		_tcscpy(GLOBAL_USERDATA->GetUserInfo()->szPWD,lpszRegPass);  //记录密码
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
		return UserMan::LOG_SUCCEEDED;
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
			return UserMan::LOG_FAILED_USER_NOT_EXIST;
		}
		else if( strstr(szReBuffer,"PASSWORD_ERROR") != NULL )
		{
			return UserMan::LOG_FAILED_PASS_WRONG;
		}
		else
		{
			return UserMan::LOG_FAILED_OTHER;
		}
	}

FAILED:
	delete[] szEncoderPass;
	return UserMan::LOG_FAILED_OTHER;
}

void UserMan::User_Register()
{
	bool bSepMode;

	bSepMode = AfxGetUIManager()->UIGetLayoutMgr()->GetDisplayMode() == DISPLAY_SEPERATE_MODE;	

	CUserRegister dlg(bSepMode ? ::CHostWndMgr::GetInstance()->GetHostWnd("sepControl"): AfxGetMainWindow());

	YL_Log("AddDlg.txt",LOG_DEBUG,"Add","UserRegister");

	AfxGetUIManager()->UIAddDialog( &dlg );
	if(dlg.DoModal() == IDOK)
	{
		//注册成功后默认自动登录
		AfxGetUserConfig()->SetConfigBoolValue( CONF_SETTING_MODULE_NAME, CONF_SETTING_AUTO_LOG, true );
		
		CUserFaceDlg dlg;
		dlg.DoModal();
		UserMan::User_Logout();
		UserMan::User_AppStartUp();
	}

	YL_Log("AddDlg.txt",LOG_DEBUG,"Remove","UserRegister");
	AfxGetUIManager()->UIRemoveDialog( &dlg );


}

BOOL UserMan::s_bIsLoginDlgOpened=FALSE;
