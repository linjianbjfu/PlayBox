#pragma once
#pragma warning (disable : 4530)
#include <string>
#include "LhcImg.h"
#include "YL_EncFileReg.h"
#include "YL_RegInfo.h"
#include "tools.h"

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
			str = str.substr(0, str.length()-1);
		YL_RegInfo::WriteString(HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_SOFT_HOME_KEY, str.c_str());
		_snprintf(szPath, pathlen-1, "%s", str.c_str());
	}
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

bool CLhcImg::GetConfigServer(char szUrl[], int urllen)
{
	YL_EncFileReg::GetString( "ConfigServer", "confsvr", szUrl, urllen );
	return true;
}