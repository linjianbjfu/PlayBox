//const.h
#pragma once
#include "globalparas.h"
#include <string>
using namespace std;

#define STR_REG_KOOWO			"SOFTWARE"
#define STR_SOFT_UNINSTALL_KEY  "UNINSTALLED"
#define STR_SOFT_INSTALL_KEY    "INSTALLED"

class CLhcImg
{
public:
	static bool GetHomePath(char szPath[], int pathlen);
	static bool GetUpdateServer(char szSvr[], int svrlen);
	static bool GetLogServer(char szUrl[], int urllen);
	static bool GetUpLogServer(char szUrl[], int urllen);
	static bool GetConfigServer(char szUrl[], int urllen);
	// server list
	static bool GetSoftwareVersion(char szSoftVersion[], int verlen);
	static bool GetUserID(char szID[], int idlen);
	static bool GetInstallSRC(char szSRC[], int srclen);
	static bool GetConfigPath(char szFilePath[], int pathlen);
	static bool GetTempConfigPath(char szFilePath[], int pathlen);
	static bool GetAdPath(char szPath[], int pathlen);
	static bool GetAdConfigPath(char szPath[], int pathlen);
	static bool GetUserFileName(char szFileName[], int namelen);
	static bool GetUserFilePath(char szFilePath[], int pathlen);
	static bool GetLogFilePath(char szFilePath[], int pathlen);
	static void WriteNetID(const char* lpszNetID);
	static void GetUserRegIDAndPass(char szID[],int iIDLen,char szPass[],int iPassLen);
	// 升级提示delay 天数
	static int GetUpdateTipsDelay();
	static bool GetAdDataServer(string& strSvr);
	static bool CheckUpdateADData();
	static bool SetUpdateADData();
};

