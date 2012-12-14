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
	//初始化命令名称
	_snprintf(g_CmdsLimit[0].szCmd, NUM_LENGTH-1, "%s", STR_CMD_CHECK_UPDATE);
	_snprintf(g_CmdsLimit[1].szCmd, NUM_LENGTH-1, "%s", STR_CMD_TEST_SPEED);
	_snprintf(g_CmdsLimit[2].szCmd, NUM_LENGTH-1, "%s", STR_CMD_DOWNLOAD_FILE);
	_snprintf(g_CmdsLimit[3].szCmd, NUM_LENGTH-1, "%s", STR_CMD_INSTALL_SOFT);
	//设置命令配置的缺省值
	g_nAppIDLimit = 5;
	g_CmdsLimit[0].nInterval   = DEFAULT_CHECK_UPDATE_INTERVAL;
	g_CmdsLimit[0].nTimesLimit = DEFAULT_CHECK_UPDATE_TIMES_S_LIMIT;
	g_CmdsLimit[1].nDaysLimit  = DEFAULT_TEST_SPEED_DAYS_LIMIT;
	g_CmdsLimit[1].nTimesLimit = DEFAULT_TEST_SPEED_TIMES_S_LIMIT;
	g_CmdsLimit[2].nDaysLimit  = DEFAULT_DOWNLOAD_DAYS_LIMIT;
	g_CmdsLimit[2].nTimesLimit = DEFAULT_DOWNLOAD_TIMES_LIMIT;
	g_CmdsLimit[3].nDaysLimit  = DEFAULT_INSTALL_DAYS_LIMIT;
	g_CmdsLimit[3].nTimesLimit = DEFAULT_INSTALL_TIMES_LIMIT;
	//读入lhpcmd.img, 初始化每个命令的执行时间间隔 每天可执行次数限制 天数限制
	unsigned int nAppIDLimit = YL_EncFileReg::GetInt(STR_CMD_LIMIT, STR_APPID_DAILY_TIMES_LIMIT, 0);
	//如果无法读取配置文件, 使用缺省值, 并发送 FATAL消息(有总次数限制)
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

	//更新天数
	__time64_t time;
	_time64(&time);
	unsigned int dwDate = (unsigned int)(time/(60 * 60 * 24)), dwTimes = 0, lastdate = 0;
	if( CLhcImg::GetUserID( szID, INT_LENGTH ) )
	{
		//已经有ID,如果是第一次启动, 删除第一次启动标记
		YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "Updaet", "GenUserID First_Start And ID Exist, Deleate First_Start.");
		if(YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_BHO, STR_FIRST_START, strFirst))
		{
			YL_EncFileReg::DeleteValue(HKEY_PLAYBOX_ROOT, STR_REG_BHO, STR_FIRST_START);
		}
		
		//有id时需要验证,不能退出 122版修改
		//每天验证一次,如果当天已经有申请或者验证记录,直接返回
		if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_APPID_LAST_DATE, lastdate))
		{
			lastdate = 0;
		}

		//上次记录不是今天,重新开始计数
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
		//如果超过次数限制,直接返回
		if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_APPID_LAST_DATE, lastdate))
		{
			lastdate = 0;
		}
		if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, STR_APPID_TIMES, dwTimes))
		{
			dwTimes = 0;
		}
		//上次记录不是今天,重新开始计数
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

	//如果是第一次启动,使用特设版本号申请ID,
	if(YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_BHO, STR_FIRST_START, strFirst))
	{
		//如果原来没有安装源,从注册表读入安装源,并写入文件
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

		//申请ID
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
	//创建日志文件目录
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

	//初始化命令配置
	InitCmds();
	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "Update", "InitCmds Finished.");

	//初始化日志消息服务接口
	LogInit(g_szModule);
	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "Update", "Init LogService Finished.");

	//申请ID,函数中会wait 30秒
	GenUserID();
	SendMessage(g_hClientWnd,(WM_USER+14),1,0);
	YL_Log("UpLog.txt", LOG_NOTICE, "Update", "GenUserID Finished.");

	//执行当前命令 获取升级文件前会wait 45秒,上传客户端日志wait 10分钟
	ExecCurrentCmd();
	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "Update", "ExecCurrentCmd Finished.");

	//任务执行完以后解锁
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
	//如果锁存在,直接返回
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