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

		sprintf(szGradeName, "%s", "��Ϸ��˿");
		memset(szName,0,sizeof(szName));
		memset(szPWD,0,sizeof(szPWD));
		memset(szLevel,0,sizeof(szLevel));
	};

public:
	DWORD   dwUserID;    //�û�ID
	DWORD   dwSvrScore;  //���������صĻ���
	DWORD   dwSvrLevel;  //���������صĵȼ�
	DWORD	dwSvrRemind; //��������
	DWORD   dwLocalScore;  //���ؼ���Ļ���
	DWORD   dwLocalLevel;  //���ؼ���ĵȼ�
	DWORD	dwLocalRemind; //��������
	DWORD	dwGradeScore;
	DWORD	dwNextGradeScore;
	DWORD	dwInterval;
	DWORD	dwLocalInterval;

	DWORD   dwMsgCount;  //վ��������

	TCHAR   szName[100]; //�û���
	TCHAR   szPWD[100];  //����
	TCHAR   szLevel[1024]; //�ȼ�
	TCHAR	szGradeName[32];
	DWORD   dwLastUserID; //��һ�ε�¼���û�id
};

#endif