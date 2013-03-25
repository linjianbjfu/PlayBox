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

HANDLE         g_hUpdateMutex = NULL;
char           g_szModule[VERSION_LENGTH];
HWND           g_hClientWnd;
int			   g_httpControl = 0;

void GenUserID()
{
	string strID;
	if(!YL_UserId::Get(YL_UserId::USER_ID, strID) || strID.empty())
	{
		char szMacAddr[MAX_PATH] = {0};
		if(MyMusic::GetMacAddress(szMacAddr))
			YL_UserId::Set(YL_UserId::USER_ID, szMacAddr);
	}
}

void Update()
{
	//创建日志文件目录
	char szPath[MAX_PATH];
	if(!CLhcImg::GetHomePath(szPath, MAX_PATH))
		return;
	
	YL_DirInfo::MakeDir(string(szPath) + "\\log");
	//初始化日志消息服务接口
	LogInit(g_szModule);
	//申请ID
	GenUserID();
	//执行当前命令 获取升级文件前会wait 45秒,上传客户端日志wait 10分钟
	ExecCurrentCmd();
	//任务执行完以后解锁
	if(g_hUpdateMutex != NULL)
	{
		CloseHandle(g_hUpdateMutex);
		g_hUpdateMutex = NULL;
	}
	return;
}

void __stdcall BeginUpdate2(const char* szSoft, const char* szModule, HWND hWnd)
{
	//如果锁存在,直接返回
	if( g_hUpdateMutex != NULL )
		return;

	SECURITY_DESCRIPTOR sd;
	InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);
	SECURITY_ATTRIBUTES sa = { sizeof sa, &sd, FALSE };

	char szMutexName[SRC_LENGTH];
	memset(szMutexName, 0, SRC_LENGTH);
	_snprintf(szMutexName, SRC_LENGTH-1, "%s_%s", "KY_Update_2013-03-23", szSoft);
	g_hUpdateMutex = CreateMutex(&sa, FALSE, szMutexName);
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(g_hUpdateMutex);
		g_hUpdateMutex = NULL;
		return;
	}
	memset(g_szModule, 0, VERSION_LENGTH);
	_snprintf(g_szModule, VERSION_LENGTH, "%s", szModule);
	g_hClientWnd = hWnd;
	Update();
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