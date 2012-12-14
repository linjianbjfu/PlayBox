#ifndef _I_USER_DATA_H
#define _I_USER_DATA_H


#include "IData.h"
#include "IUserDataDef.h"

//用户数据类
class IUserData:
	public IData
{
public:
	virtual CUserInfo* GetUserInfo() = 0;
	virtual BOOL User_IsLogin() = 0;
	virtual void User_SetLogin(BOOL bLogin) = 0;
	virtual void User_Login(LPCSTR pszName,LPCSTR pszPwd) = 0;
	virtual void User_Logout() = 0;
	virtual BOOL User_IfAutoReLogin()=0;
	virtual void User_SetAutoRelogin(BOOL bAuto)=0;
};

#endif


