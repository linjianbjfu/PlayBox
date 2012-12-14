#pragma once
#pragma warning (disable : 4530)
#include "LhFlashImg.h"
#include "YL_Ini.h"
#include "YL_RegInfo.h"
#include "YL_EncFileReg.h"
#include "shellapi.h"
#include <string>
#include "shfolder.h"
#include "tools.h"
#include <io.h>
#include <time.h>
#include "globalparas.h"
#include "LhcImg.h"

using namespace std;
#define STR_FLASH_SECTION "FlashMessage"
#define STR_FLASH_GROUP(x) ("FlashGroup" + uint2str(x)).c_str()

bool CLhFlashImg::GetHomePath(char szPath[], int pathlen)
{
	memset(szPath, 0, pathlen);	
	YL_RegInfo::ReadString(HKEY_PLAYBOX_ROOT, STR_REG_SOFT, STR_SOFT_HOME_KEY, szPath, pathlen);
	if(strlen(szPath) <= 0)
		return false;
	szPath[pathlen-1] = '\0';
	return true;
}

bool CLhFlashImg::GetSoftwareVersion(string& strVer)
{
	char path[MAX_PATH], szSoftVersion[INT_LENGTH];
	memset(szSoftVersion,0,INT_LENGTH);
	if(!GetHomePath(path, MAX_PATH))
		return false;
	string strConfPath(path); 
	strConfPath = strConfPath + "\\res\\pic\\lhc.img";
	if( GetPrivateProfileString("Config", "Version", NULL, szSoftVersion, INT_LENGTH-1, strConfPath.c_str()) == 0 )
	{
		return false;
	}
	szSoftVersion[INT_LENGTH-1] = '\0';
	strVer = szSoftVersion;
	return true;
}

bool CLhFlashImg::GetAdiniPath(string &strFile)
{
	char szConfigPath[MAX_PATH];
	if( GetHomePath(szConfigPath, MAX_PATH) )
	{
		strcat(szConfigPath, "\\res\\ad\\tips.ini");
		strFile = szConfigPath;
		return true;
	}
	return false;
}

bool CLhFlashImg::CheckValidIndex(unsigned int nGroup, unsigned int nIndex)
{
	unsigned int num = GetFlashMessageNum();
	if(nGroup >= num || nGroup < 0)
		return false;
	num = GetFlashCandidateNum(nGroup);
	if(nIndex >= num || nIndex < 0)
		return false;
	return true;
}

// flash µ¯¿ò¹ã¸æ
unsigned int CLhFlashImg::GetFlashMessageNum()
{
	unsigned int nRet = 0;
	char szConfigPath[MAX_PATH], szNum[INT_LENGTH];
	memset( szNum, 0, INT_LENGTH );
	if( GetHomePath(szConfigPath, MAX_PATH) )
	{
		strcat(szConfigPath, "\\res\\ad\\tips.ini");
		if(GetPrivateProfileString(STR_FLASH_SECTION, "num", NULL, szNum, INT_LENGTH, szConfigPath) != 0)
		{
			nRet = (unsigned int)atoi(szNum);
			if( nRet < 0 )
				nRet = 0;
		}
	}

	return nRet;
}

unsigned int CLhFlashImg::GetStartTimer()
{
	unsigned int nRet = 0;
	char szNum[INT_LENGTH];
	memset( szNum, 0, INT_LENGTH );

	string szConfigPath;
	if( GetAdiniPath(szConfigPath) )
	{
		if(GetPrivateProfileString(STR_FLASH_SECTION, "starttimer", NULL, szNum, INT_LENGTH, szConfigPath.c_str()) != 0)
		{
			nRet = (unsigned int)atoi(szNum);
			if( nRet < 0 )
				nRet = 0;
		}
	}

	return nRet * 60 * 1000;
}

unsigned int CLhFlashImg::GetFlashCandidateNum(unsigned int nGroup)
{
	unsigned int num = GetFlashMessageNum();
	if( nGroup >= num || nGroup < 0 )
		return 0;

	unsigned int nRet = 0;
	string strFile("");
	GetAdiniPath(strFile);
	if(strFile == "")
		return 0;

	char szNum[NUM_LENGTH];
	memset(szNum, 0, NUM_LENGTH);
	if(GetPrivateProfileString(STR_FLASH_GROUP(nGroup), "num", NULL, szNum, NUM_LENGTH, strFile.c_str()) != 0)
	{
		nRet = (unsigned int)atoi(szNum);
	}
	return nRet;
}

unsigned int CLhFlashImg::GetFlashMessageTimer(unsigned int nGroup, unsigned int nIndex)
{
	unsigned int nRet = 30;
	if(!CheckValidIndex(nGroup, nIndex))
		return nRet;
	char szConfigPath[MAX_PATH], szKey[NUM_LENGTH], szNum[NUM_LENGTH];
	memset(szKey, 0, NUM_LENGTH);
	memset(szNum, 0, NUM_LENGTH);
	if( !GetHomePath(szConfigPath, MAX_PATH) )
		return 0;

	strcat(szConfigPath, "\\loading\\ad\\tips.ini");
	_snprintf(szKey, NUM_LENGTH-1, "timer%u", nIndex);
	if(GetPrivateProfileString(STR_FLASH_GROUP(nGroup), szKey, NULL, szNum, NUM_LENGTH, szConfigPath) != 0)
	{
		nRet = (unsigned int)atoi(szNum);
	}
	return nRet * 60 * 1000;
}

bool CLhFlashImg::GetFlashMessageFile(unsigned int nGroup, unsigned int nIndex, string& strFile)
{
	if(!CheckValidIndex(nGroup, nIndex))
		return false;

	char szURL[MAX_PATH];
	memset( szURL, 0, MAX_PATH );
	char szConfigPath[MAX_PATH], szKey[NUM_LENGTH], szFile[MAX_PATH];
	memset(szKey, 0, NUM_LENGTH);
	memset(szFile, 0, MAX_PATH);
	if( !GetHomePath(szConfigPath, MAX_PATH) )
		return false;

	strcat(szConfigPath, "\\res\\ad\\tips.ini");
	_snprintf(szKey, NUM_LENGTH-1, "file%d", nIndex );
	if(GetPrivateProfileString(STR_FLASH_GROUP(nGroup), szKey, NULL, szFile, MAX_PATH, szConfigPath) == 0)
	{
		return false;
	}

	GetHomePath(szConfigPath, MAX_PATH);
	_snprintf(szURL, MAX_PATH-1, "%s\\res\\ad\\%s", szConfigPath, szFile);
	FILE *f = fopen(szURL, "r");
	if(f == NULL)
		return false;

	fclose(f);
	string strVer;
	GetSoftwareVersion(strVer);
	strFile = szURL;
	strFile = strFile + "?ver=" + strVer;
	return true;
}

bool CLhFlashImg::GetFlashMessageSize(unsigned int nGroup, unsigned int nIndex, int& nWidth, int& nHeight)
{
	if(!CheckValidIndex(nGroup, nIndex))
		return false;

	unsigned int nRet = 0;
	char szConfigPath[MAX_PATH], szNum[INT_LENGTH];
	memset(szNum, 0, INT_LENGTH);
	if( !GetHomePath(szConfigPath, MAX_PATH) )
		return false;
	strcat(szConfigPath, "\\res\\ad\\tips.ini");

	char szKey[NUM_LENGTH];
	_snprintf(szKey, NUM_LENGTH-1, "width%u", nIndex);
	if(GetPrivateProfileString(STR_FLASH_GROUP(nGroup), szKey, NULL, szNum, INT_LENGTH, szConfigPath) != 0)
	{
		nWidth = (unsigned int)atoi(szNum);
		memset(szNum, 0, INT_LENGTH);
		_snprintf(szKey, NUM_LENGTH-1, "height%u", nIndex);
		if(GetPrivateProfileString(STR_FLASH_GROUP(nGroup), szKey, NULL, szNum, INT_LENGTH, szConfigPath) != 0)
		{
			nHeight = (unsigned int)atoi(szNum);
			return true;
		}
	}
	return false;
}

unsigned int CLhFlashImg::GetFlashMessageWeight(unsigned int nGroup, unsigned int nIndex)
{
	if(!CheckValidIndex(nGroup, nIndex))
		return 0;
	unsigned int nRet = 0;
	string strFile("");
	GetAdiniPath(strFile);
	if(strFile == "")
		return 0;
	char szNum[INT_LENGTH];
	memset(szNum, 0, INT_LENGTH);
	if(GetPrivateProfileString(STR_FLASH_GROUP(nGroup), ("weight"+uint2str(nIndex)).c_str(), NULL, szNum, INT_LENGTH, strFile.c_str()) != 0)
	{
		nRet = (unsigned int)atoi(szNum);
	}
	return nRet;
}

bool CLhFlashImg::GetFlashMessageId(unsigned int nGroup, unsigned int nIndex, string &strId)
{
	if(!CheckValidIndex(nGroup, nIndex))
		return false;

	string strFile("");
	GetAdiniPath(strFile);
	if(strFile == "")
		return false;
	char szId[MAX_PATH];
	memset(szId, 0, MAX_PATH);
	if(GetPrivateProfileString(STR_FLASH_GROUP(nGroup), ("id"+uint2str(nIndex)).c_str(), NULL, szId, MAX_PATH, strFile.c_str()) != 0)
	{
		strId = szId;
	}
	return true;
}

bool CLhFlashImg::GetFlashMsgInfo(unsigned int nUserId, unsigned int nGroup, int &top, int &left, int &nWidth, int &nHeight, unsigned int &nTimer, string &strId, string & strFile)
{
	unsigned int nIndex = 0;
	if(!SelectCandidate(nUserId, nGroup, nIndex))
		return false;
	nWidth = nHeight = 0;
	GetFlashMessageSize(nGroup, nIndex, nWidth, nHeight);

	RECT rectDesktopWithoutTaskbar;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rectDesktopWithoutTaskbar, 0);
	left  = rectDesktopWithoutTaskbar.right - nWidth;
	top = rectDesktopWithoutTaskbar.bottom - nHeight;

	nTimer = GetFlashMessageTimer(nGroup, nIndex);
	GetFlashMessageFile(nGroup, nIndex, strFile);
	GetFlashMessageId(nGroup, nIndex, strId);
	return true;
}

bool CLhFlashImg::SelectCandidate(unsigned int nUserId, unsigned int nGroup, unsigned int &nIndex)
{
	if(GetFlashCandidateNum(nGroup) <= 0)
		return false;
	unsigned int nWeights = 0;
	for(nIndex = 0; nIndex < GetFlashCandidateNum(nGroup); nIndex++)
	{
		nWeights += GetFlashMessageWeight(nGroup, nIndex);
	}
	if(nWeights <= 0)
		return false;
	unsigned int nOrder = (nUserId + GetCurrentDate()) % nWeights;
	nIndex = 0;
	nWeights = GetFlashMessageWeight(nGroup, nIndex);
	while(nOrder >= nWeights && nIndex < GetFlashCandidateNum(nGroup))
	{
		nIndex++;
		nWeights += GetFlashMessageWeight(nGroup, nIndex);
	}
	return true;
}

int CLhFlashImg::GetFlashPopDate()
{
	unsigned int nDate = 0;
	if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, "FLASH_AD_DATE", nDate))
	{
		return 0;
	}
	return nDate;
}

bool CLhFlashImg::SetFlashPopDate(unsigned int nDate)
{
	return YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, "FLASH_AD_DATE", nDate);
}

int CLhFlashImg::GetFlashPopGroupIndex()
{
	unsigned int nIndex = 0;
	if(!YL_EncFileReg::ReadDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, "FLASH_AD_Group", nIndex))
	{
		return 0;
	}
	return nIndex;
}

bool CLhFlashImg::SetFlashPopGroupIndex(unsigned int nIndex)
{
	return YL_EncFileReg::WriteDWORD(HKEY_PLAYBOX_ROOT, STR_REG_UPDATE, "FLASH_AD_Group", nIndex);
}