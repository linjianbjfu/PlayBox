#include "YL_UserId.h"
#include <string>
#include "YL_StringUtil.h"
#include <shlobj.h>
#include "YL_DirInfo.h"

using namespace std;
static LPCSTR g_section = "user";
LPCSTR YL_UserId::USER_ID = "userid";
LPCSTR YL_UserId::INSTALL_SRC = "install_src";
LPCSTR YL_UserId::VERSION = "version";

bool YL_UserId::Set(LPCSTR key, LPCSTR value)
{
	std::string strPath;
	if (!GetUserFilePath(strPath))
		return false;

	WritePrivateProfileString(g_section, key, value, strPath.c_str());
	return true;
}

bool YL_UserId::Get(LPCSTR key, std::string& strValue)
{
	std::string strPath;
	if (!GetUserFilePath(strPath))
		return false;

	char szValue[MAX_PATH] = {0};
	GetPrivateProfileString(g_section, key, "", szValue, MAX_PATH, strPath.c_str());
	return true;
}

bool YL_UserId::GetUserFilePath(std::string& strPath)
{
	char szTempPath[2*MAX_PATH] = {0};
	SHGetSpecialFolderPath(NULL, szTempPath, CSIDL_APPDATA, FALSE);
	if (strlen(szTempPath) == 0)
		return false;
	YL_StringUtil::Format(strPath, "%s\\PP", szTempPath);
	YL_DirInfo::MakeDir(strPath);
	YL_StringUtil::Format(strPath, "%s\\PP\\ky.ini", szTempPath);
	return true;
}