#include "stdafx.h"
#include "tools.h"
#include "LhcImg.h"
#include "YL_FileInfo.h"
#include "YL_DirInfo.h"
#include "YL_EncFileReg.h"
#include "../../../../CommonLib/prj/Update/Update.h"
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
	static bool haveIn = false;//checkUpdateֻ�������һ��
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
@brief	����Ƿ�����ʾ������
*/
void checkPopUpUpdate(HWND *phWnd)
{
	YL_Log( STR_LOG_FILE ,LOG_NOTICE,"PlayBoxDlg","checkPopUpUpdate==IN" );
	static bool haveInCheckPopUpUpdate = false;//checkPopUpUpdateֻ�������һ��
	if(!haveInCheckPopUpUpdate)
	{
		haveInCheckPopUpUpdate = true;	
		_beginthread(runPopUpUpdate, 0, (void*)phWnd);
	}
	YL_Log( STR_LOG_FILE ,LOG_NOTICE,"PlayBoxDlg","checkPopUpUpdate==OUT" );
}

/*
@brief	����Ƿ�����ʾ���������������а�װ�������˳���Ϸ��
*/
void runPopUpUpdate(LPVOID pparam)
{
	YL_Log(UPDATE_LOG, LOG_NOTICE, "runPopUpUpdate", "IN==");
	//����Ƿ�Ӧ�ó���������ʾ
	//û�湻10����Ϸ������������
	int nListen = 0;
	AfxGetUserConfig()->GetConfigIntValue( CONF_APP_MODULE_NAME, CONF_APP_PLAYED_GAME, nListen );
	if(nListen < 4)
	{
		YL_Log(UPDATE_LOG, LOG_NOTICE, "runPopUpUpdate", "OUT0==");
		return;
	}
	unsigned int dwDate = 0, dwInstDate = 0, dwCurDate = 0;
	time_t timer;
	time(&timer);
	dwCurDate = (DWORD)(timer/(60 * 60 * 24));
	//��鰲װʱ��
	if(YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, "INSTALL_DATE", dwInstDate))
	{
		int delay = CLhcImg::GetUpdateTipsDelay();
		if(dwCurDate - dwInstDate < delay)
		{
			YL_Log(UPDATE_LOG, LOG_NOTICE, "runPopUpUpdate", "OUT1==");
			return;
		}
	}
	//�ϴ�ִ��ʱ��
	if(YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, "UPTIP_DATE", dwDate))
	{
		//�Ƚ�ʱ����
		if(dwCurDate - dwDate < 3)
		{
			YL_Log(UPDATE_LOG, LOG_NOTICE, "runPopUpUpdate", "OUT2==");
			return;
		}
	}
	
	char cfname[MAX_PATH];
	GetModuleFileName(NULL, cfname, MAX_PATH);	
	string playerName;
	if(strstr(strupr(cfname), "PLAYBOX") != NULL)
	{
		playerName = "PLAYBOX";
	}
	else
	{
		YL_Log(UPDATE_LOG, LOG_NOTICE, "runPopUpUpdate", "OUT3==");
		return;
	}
	//��ע����ж��Ƿ��������°汾
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
		
		//��Ĭ��װ����������������bho����
		if(strstr(strupr(setupexe), "_SILENT") != NULL)
		{
			YL_Log(UPDATE_LOG, LOG_NOTICE, "runPopUpUpdate", "OUT4==");
			return;
		}		
		int forceUpdate;
		if(strstr(strupr(setupexe), "_FORCE") != NULL)
			forceUpdate = MB_OK;
		else
			forceUpdate = MB_OKCANCEL;

		HWND hMain = NULL;

		CUpdateTipDlg dlg;
		dlg.SetForceUpdate(forceUpdate);
		Sleep(6000); // wait for main window shown
		//��¼����ִ������
		YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, "UPTIP_DATE", dwCurDate);
		if(dlg.DoModal() == IDOK)
		{
			//����û�ȷ�ϣ����а�װ����
			YL_EncFileReg::WriteString( HKEY_PLAYBOX_ROOT, STR_REG_SOFT, "PLAYERCMD", GetCommandLine() );
			
			char uppath[MAX_PATH];
			memset(uppath, 0, MAX_PATH);
			_snprintf(uppath, MAX_PATH-1, "%s\\update", dir);
			win32Showexec(setupexe, uppath, false);
			//�ر���Ϸ��
			YL_Log(UPDATE_LOG, LOG_NOTICE, "runPopUpUpdate", "SendMessage to Close Player %s.", playerName.c_str());
			if(playerName == "PLAYBOX")
			{
				PostMessage(*((HWND*)pparam), WM_CLOSE, 0,0);
			}
			else
			{
				YL_Log(UPDATE_LOG, LOG_NOTICE, "runPopUpUpdate", "OUT5==");
				exit(0);
			}
		}
	}
	YL_Log(UPDATE_LOG, LOG_NOTICE, "runPopUpUpdate", "OUT6==");
}
/*
@brief	����������·������б������������ȹ���
*/
typedef void (__stdcall *LPCREATERECOGNISE)(const char* szSoft, const char* szModule, HWND hWnd);
void realUpdate(LPVOID pparam)
{
	YL_Log(UPDATE_LOG, LOG_NOTICE, "realUpdate", "IN==");

	char szsysdir[MAX_PATH], szUpdate[MAX_PATH], szPlayer[64];
	memset(szsysdir, 0 , MAX_PATH);
	memset(szUpdate, 0, MAX_PATH);
	memset(szPlayer, 0, 64);
	_snprintf(szPlayer, 64-1, "%s", (char*)pparam);

	if(!CLhcImg::GetHomePath(szsysdir, MAX_PATH))
	{
		return;
	}
	_snprintf(szUpdate, MAX_PATH-1, "%s\\Update.dll", szsysdir);

	HMODULE hLib = LoadLibrary(szUpdate);
	if(hLib != NULL)
	{
		LPCREATERECOGNISE lpfn = (LPCREATERECOGNISE)GetProcAddress(hLib, "BeginUpdate2");	
		if(lpfn != NULL)
		{
			YL_Log(UPDATE_LOG, LOG_NOTICE, "realUpdate", "Call BeginUpdate2.");
			lpfn(g_szSoftName, szPlayer, g_hSharedWnd);
		}
		FreeLibrary(hLib);
	}
	YL_Log(UPDATE_LOG, LOG_NOTICE, "realUpdate", "OUT==");
}