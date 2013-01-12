#pragma once
#pragma warning (disable : 4530)
#include "YL_UserId.h"
#include "LhcImg.h"
#include "YL_Base64.h"
#include "YL_RegInfo.h"
#include "YL_EncFileReg.h"
#include "YL_Ini.h"
#include "shellapi.h"
#include <string>
#include "shfolder.h"
#include "tools.h"
#include <io.h>
#include <time.h>
#include "globalparas.h"
#include  "..\prj\Log\KwLogSvr.h"
#include <atltime.h>

using namespace std;
#pragma   data_seg("SHARED")
#pragma   data_seg()
#pragma   comment(linker, "/section:SHARED,RWS")

bool CLhcImg::GetHomePath(char szPath[], int pathlen)
{
	memset(szPath, 0, pathlen);	
	YL_RegInfo::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_SOFT_HOME_KEY, szPath, pathlen);
	if(strlen(szPath) <= 0)
	{
		string str = GetExePath();
		if(str.length() <= 0)
			return false;
		else if(str[str.length()-1] == '\\')
		{
			str = str.substr(0, str.length()-1);
		}
		YL_RegInfo::WriteString(HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_SOFT_HOME_KEY, str.c_str());
		_snprintf(szPath, pathlen-1, "%s", str.c_str());
	}
	szPath[pathlen-1] = '\0';
	return true;
}

bool CLhcImg::GetConfigPath(char szFilePath[], int pathlen)
{
	string strPath;
	YL_EncFileReg::GetEncFilePath( strPath );
	_snprintf( szFilePath, pathlen, "%s", strPath.c_str() );
	return true;
}

bool CLhcImg::GetAdPath(char szPath[], int pathlen)
{
	char szBuffer[MAX_PATH];
	if( !GetHomePath( szBuffer,MAX_PATH ) )
		return false;
	_snprintf(szPath, pathlen-1, "%s\\Formats\\", szBuffer);
	return true;
}

bool CLhcImg::GetAdConfigPath(char szPath[], int pathlen)
{
	char szBuffer[MAX_PATH];
	if( !GetAdPath( szBuffer,MAX_PATH ) )
		return false;
	_snprintf(szPath, pathlen-1, "%stips.ini", szBuffer);
	return true;
}

bool CLhcImg::GetTempConfigPath(char szFilePath[], int pathlen)
{
	char dir[MAX_PATH];
	memset(szFilePath, 0, pathlen);
	if(!GetHomePath(dir, MAX_PATH))
		return false;
	_snprintf(szFilePath, pathlen-1, "%s%s", dir, "\\skin\\svr.img");
	return true;
}

bool CLhcImg::GetUpdateServer(char szSvr[], int svrlen)
{
	YL_EncFileReg::GetString( "UpdateServer", "updatesvr", szSvr, svrlen );
	return true;
}

bool CLhcImg::GetLogServer(char szUrl[], int urllen)
{
	YL_EncFileReg::GetString( "LogMsg", "reallogserver", szUrl, urllen );
	return true;
}

bool CLhcImg::GetUpLogServer(char szUrl[], int urllen)
{
	YL_EncFileReg::GetString( "LogServer", "uplogsvr", szUrl, urllen );
	return true;
}

bool CLhcImg::GetConfigServer(char szUrl[], int urllen)
{
	YL_EncFileReg::GetString( "ConfigServer", "confsvr", szUrl, urllen );
	return true;
}

bool CLhcImg::GetSoftwareVersion(char szSoftVersion[], int verlen)
{
	YL_EncFileReg::GetString( "Config", "Version", szSoftVersion, verlen );
	return true;
}

bool CLhcImg::GetLogFilePath(char szFilePath[], int pathlen)
{
	char dir[MAX_PATH];
	memset(szFilePath, 0, pathlen);
	if(!GetHomePath(dir, MAX_PATH))
	{
		return false;
	}
	_snprintf(szFilePath, pathlen-1, "%s%s", dir, "\\log\\act.log");
	return true;
}

bool CLhcImg::GetUserID( char szID[], int idlen)
{
	return YL_UserId::GetUserID(szID, idlen);
}

bool CLhcImg::GetInstallSRC(char szSRC[], int srclen)
{
	return YL_UserId::GetInstallSRC(szSRC, srclen);
}

bool CLhcImg::GetAdDataServer(string& strSvr)
{
	char szAdDataSvr[MAX_PATH];
	memset(szAdDataSvr, 0, MAX_PATH);
	YL_EncFileReg::GetString( "ADDataSvr", "ADDataServer", szAdDataSvr, MAX_PATH );
	strSvr = szAdDataSvr;
	return true;
}

bool CLhcImg::CheckUpdateADData()
{
	unsigned int dwDate, dwCurDate;
	if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, "AD_DATE", dwDate))
		return true;

	CTime tm;
	tm=CTime::GetCurrentTime();
	dwCurDate = tm.GetYear()*10000+tm.GetMonth()*100+tm.GetDay();
	if (dwCurDate-dwDate!=0)
		return true;
	return false;
}

bool CLhcImg::SetUpdateADData()
{
	//获取系统时间
	CTime tm;
	tm=CTime::GetCurrentTime();
	DWORD dwCurDate = tm.GetYear()*10000+tm.GetMonth()*100+tm.GetDay();

	return YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, "AD_DATE", dwCurDate);
}

int CLhcImg::GetUpdateTipsDelay()
{
	char szText[MAX_PATH];
	memset(szText, 0, MAX_PATH);
	YL_EncFileReg::GetString( "UpdateTips", "delay", szText, MAX_PATH );
	int nRet = (int)atoi(szText);
	return nRet;
}
