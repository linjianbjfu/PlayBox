#ifndef _I_USER_DATA_DEF_H
#define _I_USER_DATA_DEF_H

class CUserInfo
{
public:
	CUserInfo()
	{
		Clear();
		dwLastUserID = 0;
	};

	void Clear()
	{	
		dwLastUserID = dwUserID;
		dwUserID = 0;
		dwSvrScore = 0;
		dwSvrLevel = 0;
		dwLocalScore = 0;
		dwLocalLevel = 0;
		dwMsgCount = 0;
		dwSvrRemind = 0;
		dwLocalRemind = 0;
		dwGradeScore = 0;
		dwNextGradeScore = 0;
		dwInterval = 10;
		dwLocalInterval = 0;

		sprintf(szGradeName, "%s", "游戏粉丝");
		memset(szName,0,sizeof(szName));
		memset(szPWD,0,sizeof(szPWD));
		memset(szLevel,0,sizeof(szLevel));
	};

public:
	DWORD   dwUserID;    //用户ID
	DWORD   dwSvrScore;  //服务器返回的积分
	DWORD   dwSvrLevel;  //服务器返回的等级
	DWORD	dwSvrRemind; //本地提醒
	DWORD   dwLocalScore;  //本地计算的积分
	DWORD   dwLocalLevel;  //本地计算的等级
	DWORD	dwLocalRemind; //本地提醒
	DWORD	dwGradeScore;
	DWORD	dwNextGradeScore;
	DWORD	dwInterval;
	DWORD	dwLocalInterval;

	DWORD   dwMsgCount;  //站内信数量

	TCHAR   szName[100]; //用户名
	TCHAR   szPWD[100];  //密码
	TCHAR   szLevel[1024]; //等级
	TCHAR	szGradeName[32];
	DWORD   dwLastUserID; //上一次登录的用户id
};

#endif