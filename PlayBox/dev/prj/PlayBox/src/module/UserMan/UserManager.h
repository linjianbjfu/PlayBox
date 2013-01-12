#ifndef  USERMANAGER_H
#define USERMANAGER_H
#pragma once

class YL_CHTTPRequest;
class UserInfo;

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
	void User_Login();
	void User_Logout();
	void User_AppExit();
	BOOL User_IsLogin();
	void User_LoginFaild();
	static DWORD WINAPI Thread_LogIn(void*ppar);
	static DWORD WINAPI Thread_LogInFromFlash(void* ppar);
	void User_Login(LPCSTR pszName,LPCSTR pszPwd);
	static LOGIN_STATUS	 Login(YL_CHTTPRequest** pHTTP,const char* lpszRegID,const char* lpszRegPass, bool bAuto=false);
	void ClearUserInfo();
	void SetUserInfo(int iID, const std::string& strName, const std::string& strPassMD5);
	void SetLogonState(bool bIsLogon);

private:	
	HANDLE	m_hThreadLogIn;
	UserInfo* m_pUserInfo;
	CUserManager();
	char  m_szTmpName[100]; //线程传参使用
	char  m_szTmpPWD[100];	//线程传参使用
	bool m_bLogIn;
};

#endif // USERMANAGER_H