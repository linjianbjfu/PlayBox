#include "stdafx.h"
#include "Update.h"
#include "UserReg.h"
#include "../../common/YL_RegInfo.h"
#include "../../common/YL_EncFileReg.h"
#include "../../common/YL_Ini.h"
#include "../../common/YL_FileInfo.h"
#include "../../common/tools.h"
#include "../../common/LhcImg.h"
#include "../../common/YL_DirInfo.h"
#include "../../common/YL_UserId.h"
#include <process.h>
#include <time.h>

extern bool ExecCurrentCmd();
extern bool CheckInstallState();

CmdLimit       g_CmdsLimit[4];
unsigned int   g_nAppIDLimit;
HANDLE         g_hUpdateMutex = NULL;
char           g_szModule[VERSION_LENGTH];
HWND           g_hClientWnd;
int			   g_httpControl = 0;

HANDLE		   g_EventRegID;
HANDLE		   g_EventGetConfig;
HANDLE		   g_EventUpConfigLog;

bool InitCmds()
{
	for(int n = 0; n<4; n++)
	{
		memset(&g_CmdsLimit[n], 0, sizeof(CmdLimit));
	}
	//��ʼ����������
	_snprintf(g_CmdsLimit[0].szCmd, NUM_LENGTH-1, "%s", STR_CMD_CHECK_UPDATE);
	_snprintf(g_CmdsLimit[1].szCmd, NUM_LENGTH-1, "%s", STR_CMD_TEST_SPEED);
	_snprintf(g_CmdsLimit[2].szCmd, NUM_LENGTH-1, "%s", STR_CMD_DOWNLOAD_FILE);
	_snprintf(g_CmdsLimit[3].szCmd, NUM_LENGTH-1, "%s", STR_CMD_INSTALL_SOFT);
	//�����������õ�ȱʡֵ
	g_nAppIDLimit = 5;
	g_CmdsLimit[0].nInterval   = DEFAULT_CHECK_UPDATE_INTERVAL;
	g_CmdsLimit[0].nTimesLimit = DEFAULT_CHECK_UPDATE_TIMES_S_LIMIT;
	g_CmdsLimit[1].nDaysLimit  = DEFAULT_TEST_SPEED_DAYS_LIMIT;
	g_CmdsLimit[1].nTimesLimit = DEFAULT_TEST_SPEED_TIMES_S_LIMIT;
	g_CmdsLimit[2].nDaysLimit  = DEFAULT_DOWNLOAD_DAYS_LIMIT;
	g_CmdsLimit[2].nTimesLimit = DEFAULT_DOWNLOAD_TIMES_LIMIT;
	g_CmdsLimit[3].nDaysLimit  = DEFAULT_INSTALL_DAYS_LIMIT;
	g_CmdsLimit[3].nTimesLimit = DEFAULT_INSTALL_TIMES_LIMIT;
	//����lhpcmd.img, ��ʼ��ÿ�������ִ��ʱ���� ÿ���ִ�д������� ��������
	unsigned int nAppIDLimit = YL_EncFileReg::GetInt(STR_CMD_LIMIT, STR_APPID_DAILY_TIMES_LIMIT, 0);
	//����޷���ȡ�����ļ�, ʹ��ȱʡֵ, ������ FATAL��Ϣ(���ܴ�������)
	if(nAppIDLimit <= 0)
	{
		return false;
	}
	else
	{
		g_nAppIDLimit = nAppIDLimit;
		g_CmdsLimit[0].nInterval   = YL_EncFileReg::GetInt(STR_CMD_LIMIT, STR_CHECK_UPDATE_INTERVAL,  DEFAULT_CHECK_UPDATE_INTERVAL);
		g_CmdsLimit[0].nTimesLimit = YL_EncFileReg::GetInt(STR_CMD_LIMIT, STR_CHECK_UPDATE_TIMES_LIMIT, DEFAULT_CHECK_UPDATE_TIMES_S_LIMIT);
		g_CmdsLimit[1].nDaysLimit  = YL_EncFileReg::GetInt(STR_CMD_LIMIT, STR_TEST_SPEED_DAYS_LIMIT,  DEFAULT_TEST_SPEED_DAYS_LIMIT);
		g_CmdsLimit[1].nTimesLimit = YL_EncFileReg::GetInt(STR_CMD_LIMIT, STR_TEST_SPEED_TIMES_LIMIT, DEFAULT_TEST_SPEED_TIMES_S_LIMIT);
		g_CmdsLimit[2].nDaysLimit  = YL_EncFileReg::GetInt(STR_CMD_LIMIT, STR_DOWNLOAD_DAYS_LIMIT,    DEFAULT_DOWNLOAD_DAYS_LIMIT);
		g_CmdsLimit[2].nTimesLimit = YL_EncFileReg::GetInt(STR_CMD_LIMIT, STR_DOWNLOAD_TIMES_LIMIT,   DEFAULT_DOWNLOAD_TIMES_LIMIT);
		g_CmdsLimit[3].nDaysLimit  = YL_EncFileReg::GetInt(STR_CMD_LIMIT, STR_INSTALL_DAYS_LIMIT,     DEFAULT_INSTALL_DAYS_LIMIT);
		g_CmdsLimit[3].nTimesLimit = YL_EncFileReg::GetInt(STR_CMD_LIMIT, STR_INSTALL_TIMES_LIMIT,    DEFAULT_INSTALL_TIMES_LIMIT);
	}

	return true;
}

void GenUserID()
{
	char szID[INT_LENGTH];
	string strFirst;

	//��������
	__time64_t time;
	_time64(&time);
	unsigned int dwDate = (unsigned int)(time/(60 * 60 * 24)), dwTimes = 0, lastdate = 0;
	if( CLhcImg::GetUserID( szID, INT_LENGTH ) )
	{
		//�Ѿ���ID,����ǵ�һ������, ɾ����һ���������
		YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "Updaet", "GenUserID First_Start And ID Exist, Deleate First_Start.");
		if(YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_BHO, STR_FIRST_START, strFirst))
		{
			YL_EncFileReg::DeleteValue(HKEY_PLAYBOX_ROOT, STR_REG_BHO, STR_FIRST_START);
		}
		
		//��idʱ��Ҫ��֤,�����˳� 122���޸�
		//ÿ����֤һ��,��������Ѿ������������֤��¼,ֱ�ӷ���
		if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_APPID_LAST_DATE, lastdate))
		{
			lastdate = 0;
		}

		//�ϴμ�¼���ǽ���,���¿�ʼ����
		if(dwDate != lastdate)
		{
			YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_APPID_LAST_DATE, dwDate);
			dwTimes = 0;
			YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_APPID_TIMES, dwTimes);
		}
		else
			return;
	}
	else
	{
		//���������������,ֱ�ӷ���
		if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_APPID_LAST_DATE, lastdate))
		{
			lastdate = 0;
		}
		if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_APPID_TIMES, dwTimes))
		{
			dwTimes = 0;
		}
		//�ϴμ�¼���ǽ���,���¿�ʼ����
		if(dwDate != lastdate)
		{
			YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_APPID_LAST_DATE, dwDate);
			dwTimes = 0;
			YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_APPID_TIMES, dwTimes);
		}

		if(dwTimes < g_nAppIDLimit)
		{
			if( !YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_APPID_TIMES, ++dwTimes) )
			{
				return;
			}
		}
		else
		{
			YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "Updaet", "GenUserID Times Limit.");
			return;
		}
	}

	char szUserFilePath[MAX_PATH], szSource[SRC_LENGTH];
	if(!YL_UserId::GetUserFilePath(szUserFilePath, MAX_PATH)) //g7kk.dat
	{
		YL_Log(UPDATE_LOG_FILE, LOG_FATAL, "Update", "GenUserID Call GetUserFilePath Error!");
		return;
	}

	YL_Log("UpLog.txt", LOG_NOTICE, "Update", "Wait RegID Event");
	WaitForSingleObject( g_EventRegID,30*1000);	
	YL_Log("UpLog.txt", LOG_NOTICE, "Update", "Wait RegID Event--DONE");

	//����ǵ�һ������,ʹ������汾������ID,
	if(YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_BHO, STR_FIRST_START, strFirst))
	{
		//���ԭ��û�а�װԴ,��ע�����밲װԴ,��д���ļ�
		if( !CLhcImg::GetInstallSRC( szSource, SRC_LENGTH ) )
		{
			string::size_type nTok = strFirst.find_last_of("\\/");
			if(nTok != string::npos)
			{
				string strSrc = strFirst.substr(nTok+1, strFirst.length()-nTok-1);
				_snprintf(szSource, SRC_LENGTH-1, "%s", strSrc.c_str());
			}
			else{
				_snprintf(szSource, SRC_LENGTH-1, "%s", strFirst.c_str());
			}
			YL_UserId::SetInstallSRC(szSource);					
		}

		//����ID
		YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "Update", "GenUserID First_Start And No ID.");
		if(MyMusic::ApplyIDFromServer(szUserFilePath, szSource, true))
		{
			YL_EncFileReg::DeleteValue(HKEY_PLAYBOX_ROOT, STR_REG_BHO, STR_FIRST_START);
		}
	}
	else
	{
		YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "Update", "GenUserID Without First_Start And No ID.");
		CLhcImg::GetInstallSRC(szSource, SRC_LENGTH);
		MyMusic::ApplyIDFromServer(szUserFilePath, szSource, false);
	}
}

void Update(void *param)
{
	//������־�ļ�Ŀ¼
	char szPath[MAX_PATH];
	if(CLhcImg::GetHomePath(szPath, MAX_PATH))
	{
		YL_DirInfo::MakeDir(string(szPath) + "\\log");
	}
	else
	{
		//Fatal Error!
		return;
	}

	//��ʼ����������
	InitCmds();
	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "Update", "InitCmds Finished.");

	//��ʼ����־��Ϣ����ӿ�
	LogInit(g_szModule);
	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "Update", "Init LogService Finished.");

	//����ID,�����л�wait 30��
	GenUserID();
	SendMessage(g_hClientWnd,(WM_USER+14),1,0);
	YL_Log("UpLog.txt", LOG_NOTICE, "Update", "GenUserID Finished.");

	//ִ�е�ǰ���� ��ȡ�����ļ�ǰ��wait 45��,�ϴ��ͻ�����־wait 10����
	ExecCurrentCmd();
	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "Update", "ExecCurrentCmd Finished.");

	//����ִ�����Ժ����
	if(g_hUpdateMutex != NULL)
	{
		CloseHandle(g_hUpdateMutex);
		g_hUpdateMutex = NULL;
		YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "Update", "ExecCurrentCmd Finished And Release Mutex.");
	}
	return;
}

void __stdcall SetHTTPControl(int iControl)
{
	g_httpControl++;
	if( iControl == 1 )
	{		
		YL_Log("UpLog.txt", LOG_NOTICE, "Update", "SetRegIdEvent");
		SetEvent(g_EventRegID);
	}	
	else if( iControl ==2 )
	{
		YL_Log("UpLog.txt", LOG_NOTICE, "Update", "SetGetConfigEvent");
		SetEvent(g_EventGetConfig);
	}
	else //iControl == 3
	{
		YL_Log("UpLog.txt", LOG_NOTICE, "Update", "SetUpConfigLogEvent");
		SetEvent(g_EventUpConfigLog);
	}
}

void __stdcall BeginUpdate2(const char* szSoft, const char* szModule, HWND hWnd)
{
	//���������,ֱ�ӷ���
	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "Update", "Enter BeginUpdate2.");
	if( g_hUpdateMutex != NULL )
		return;

	g_EventGetConfig	= CreateEvent(NULL,TRUE,FALSE,NULL);
	g_EventUpConfigLog	= CreateEvent(NULL,TRUE,FALSE,NULL);
	g_EventRegID		= CreateEvent(NULL,TRUE,FALSE,NULL);

	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);
	SECURITY_ATTRIBUTES sa = { sizeof sa, &sd, FALSE };

	char szMutexName[SRC_LENGTH];
	memset(szMutexName, 0, SRC_LENGTH);
	_snprintf(szMutexName, SRC_LENGTH-1, "%s_%s", "LANGHUA_Update_2010-07-19", szSoft);
	g_hUpdateMutex = CreateMutex(&sa, FALSE, szMutexName);
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "Update", "Mutex Existes!");
		CloseHandle(g_hUpdateMutex);
		g_hUpdateMutex = NULL;
		return;
	}

	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "Update", "No Mutex .");
	memset(g_szModule, 0, VERSION_LENGTH);
	_snprintf(g_szModule, VERSION_LENGTH, "%s", szModule);
	g_hClientWnd = hWnd;

	Update(NULL);
	return;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if(ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		if(g_hUpdateMutex != NULL)
		{
			CloseHandle(g_hUpdateMutex);
			g_hUpdateMutex = NULL;
		}
	}
    return TRUE;
}