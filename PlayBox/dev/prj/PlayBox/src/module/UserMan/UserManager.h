#ifndef USERMANAGER_H
#define USERMANAGER_H
#pragma once
#include "windows.h"

struct UserInfo;

class CUserManager
{
public:	
	CUserManager();
	static CUserManager* GetInstance();
	enum ErrDetail
	{
		SUCCEEDED = 0,
		USER_NOT_EXIST,
		USER_NAME_EMPTY,
		PASS_WORD_EMPTY,
		PASS_WRONG,
		USER_CANCEL,
		SERVER_SAID_LOGIN_FAIL,
		NET_ERROR,
		FAILED, //general fail
	};
	enum State
	{
		NOT_LOGGED_IN = 0, //Î´µÇÂ¼
		HAVE_LANDED,       //ÒÑµÇÂ½
		LOGIN_ON_THE_WAY,  //µÇÂ¼ÖÐ
	};
public:
	void User_AppStartUp();//if config is auto login, then login
	void User_Login(LPCSTR pszName,LPCSTR pszPwd);
	void User_CancelLog();
	void User_Logout();
	void User_AppExit();
	bool User_IsLogin();
	UserInfo* User_GetUserInfo() const;

private:	
	HANDLE	m_hThreadLogIn;
	UserInfo* m_pUserInfo;
	State m_state;
	ErrDetail m_errDetail;
	void UserLoginInternal(LPCSTR pszName,LPCSTR pszPwdMD5);
	static DWORD WINAPI ThreadLogin(void* pPara);
	static void ParseJson(const std::string strJson, bool& bLoginSuc);
	static void SetUserInfo(LPCSTR lpszUserName, LPCSTR lpszPass);
 };

#endif // USERMANAGER_H