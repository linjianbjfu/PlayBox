#ifndef  USERMANAGER_H
#define USERMANAGER_H
#pragma once

class YL_CHTTPRequest;

class CUserManager
{
public:	
	static void CreateInstance();
	static CUserManager*	GetInstance();
	static void	DeleteInstance();
	
	enum LOGIN_STATUS
	{
		LOG_SUCCEEDED = 0,
		LOG_FAILED_USER_NOT_EXIST,
		LOG_FAILED_PASS_WRONG,
		LOG_FAILED_OTHER
	};
public:
	void User_AppStartUp(BOOL bLogin = false);
	static void User_Login();
	static void User_Logout();
	static void User_AppExit();
	static BOOL User_IsLogin();
	static void User_LoginFaild();
	static DWORD WINAPI Thread_LogIn(void*ppar);
	static void User_Login(LPCSTR pszName,LPCSTR pszPwd);
	static LOGIN_STATUS	 Login(YL_CHTTPRequest** pHTTP,const char* lpszRegID,const char* lpszRegPass, bool bAuto=false);

private:	
	HANDLE	m_hThreadLogIn;
	UserInfo* m_pUserInfo;
	CUserManager();
};

#endif // USERMANAGER_H