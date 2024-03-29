#include "stdafx.h"
#include "tools.h"
#include "LhcImg.h"
#include "YL_FileInfo.h"
#include "YL_DirInfo.h"
#include "YL_EncFileReg.h"
#include "./src/AppConfig/config/ConfigAppDef.h"
#include "./src/AppConfig/config/ConfigSettingDef.h"
#include "./src/core/AfxGlobals.h"

#include "Update.h"
#include "UpdateTipDlg.h"
#include <time.h>

#define UPDATE_LOG       "update.txt"
#define STR_NEW_LYRIC_VERSION     "NEW_LYRIC"
#define STR_MB_NEW_LYRIC_VER	  "MB_NEW_LYRIC"

extern HWND g_hSharedWnd;

HANDLE g_hCheckUpdate = NULL;
void checkUpdate(const char* player)
{
	static bool haveIn = false;//checkUpdate只允许进入一次
	if(!haveIn)
	{
		haveIn = true;
		g_hCheckUpdate = (HANDLE)_beginthread(realUpdate, 0, (void*)(player));
	}
}

void stopCheckUpdate()
{
	DWORD dwThreadid = 0;
	if( g_hCheckUpdate && GetExitCodeThread(g_hCheckUpdate,&dwThreadid) && dwThreadid == STILL_ACTIVE)
	{
		TerminateThread(g_hCheckUpdate,1);
		WaitForSingleObject(g_hCheckUpdate,INFINITE);
		CloseHandle(g_hCheckUpdate);
		g_hCheckUpdate = NULL;
	}
}
/*
@brief	检测是否有明示升级包
*/
void checkPopUpUpdate(HWND *phWnd)
{
	static bool haveInCheckPopUpUpdate = false;//checkPopUpUpdate只允许进入一次
	if(!haveInCheckPopUpUpdate)
	{
		haveInCheckPopUpUpdate = true;	
		_beginthread(runPopUpUpdate, 0, (void*)phWnd);
	}
}

/*
@brief	检查是否有明示升级包，有则运行安装包，并退出游戏盒
*/
void runPopUpUpdate(LPVOID pparam)
{
	//检测是否应该出现升级提示
	//没玩够10个游戏，不进行升级
	int nListen = 0;
	AfxGetUserConfig()->GetConfigIntValue( CONF_APP_MODULE_NAME, CONF_APP_PLAYED_GAME, nListen );
	if(nListen < 4)
		return;

	unsigned int dwDate = 0, dwInstDate = 0, dwCurDate = 0;
	time_t timer;
	time(&timer);
	dwCurDate = (DWORD)(timer/(60 * 60 * 24));
	
	char cfname[MAX_PATH];
	GetModuleFileName(NULL, cfname, MAX_PATH);	
	string playerName;
	if(strstr(strupr(cfname), "PLAYBOX"))
		playerName = "PLAYBOX";
	else
		return;
	//从注册表判断是否下载了新版本
	char pszNewVer[128], pszMbNewVer[128];
	memset(pszNewVer, 0, 128);
	memset(pszMbNewVer, 0, 128);
	YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_NEW_LYRIC_VERSION, pszNewVer, 128);
	YL_EncFileReg::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_MB_NEW_LYRIC_VER, pszMbNewVer, 128);	
	if(strlen(pszNewVer) > 0 || (strlen(pszMbNewVer) > 0 ))
	{
		char dir[MAX_PATH];
		memset(dir, 0, MAX_PATH);
		if(!CLhcImg::GetHomePath(dir, MAX_PATH))
			return;

		char setupexe[128];
		if(strlen(pszNewVer) > 0)
			strcpy(setupexe, pszNewVer);
		else
			strcpy(setupexe, pszMbNewVer);
		
		//静默安装的升级包不处理，由bho处理
		if(strstr(strupr(setupexe), "_SILENT"))
			return;

		int forceUpdate;
		if(strstr(strupr(setupexe), "_FORCE"))
			forceUpdate = MB_OK;
		else
			forceUpdate = MB_OKCANCEL;

		HWND hMain = NULL;
		CUpdateTipDlg dlg;
		dlg.SetForceUpdate(forceUpdate);
		Sleep(6000); // wait for main window shown
		//记录当天执行日期
		YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, "UPTIP_DATE", dwCurDate);
		if(dlg.DoModal() == IDOK)
		{
			//如果用户确认，运行安装程序
			YL_EncFileReg::WriteString( HKEY_PLAYBOX_ROOT, STR_REG_SOFT, "PLAYERCMD", GetCommandLine() );
			
			char uppath[MAX_PATH];
			memset(uppath, 0, MAX_PATH);
			_snprintf(uppath, MAX_PATH-1, "%s\\update", dir);
			win32Showexec(setupexe, uppath, false);
			//关闭游戏盒
			if(playerName == "PLAYBOX")
				PostMessage(*((HWND*)pparam), WM_CLOSE, 0,0);
			else
				exit(0);
		}
	}
}
/*
@brief	具体操作更新服务器列表，下载升级包等工作
*/
typedef void (__stdcall *LPCREATERECOGNISE)(const char* szSoft, const char* szModule, HWND hWnd);
void realUpdate(LPVOID pparam)
{
	char szsysdir[MAX_PATH] = {0};
	char szUpdate[MAX_PATH] = {0};
	char szPlayer[64] = {0};
	_snprintf(szPlayer, 64-1, "%s", (char*)pparam);

	if(!CLhcImg::GetHomePath(szsysdir, MAX_PATH))
		return;
	_snprintf(szUpdate, MAX_PATH-1, "%s\\Update.dll", szsysdir);

	HMODULE hLib = LoadLibrary(szUpdate);
	if(hLib != NULL)
	{
		LPCREATERECOGNISE lpfn = (LPCREATERECOGNISE)GetProcAddress(hLib, "BeginUpdate2");	
		if(lpfn)
			lpfn(g_szSoftName, szPlayer, g_hSharedWnd);

		FreeLibrary(hLib);
	}
}