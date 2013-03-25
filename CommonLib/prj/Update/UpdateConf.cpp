#include "stdafx.h"
#include "../Log/KwLogSvr.h"
#include "UpdateConf.h"
#include "../../common/YL_RegInfo.h"
#include "../../common/YL_FileInfo.h"
#include "../../common/YL_DirInfo.h"
#include "../../common/YL_Ini.h"
#include "../../common/tools.h"
#include "../../common/MacroStr.h"
#include "../../common/LHcImg.h"

//把文件szDesFilePath中szSectionName部分的key-value内容复制到文件szConfigPath
void UpdateSectionConfig(const char* szSectionName, const char* szConfigPath, const char* szDesFilePath)
{
	char szCmdConfig[INI_SECTION_BUFFER_LENGTH];
	memset(szCmdConfig, 0, INI_SECTION_BUFFER_LENGTH);
	if( GetPrivateProfileSection(szSectionName, szCmdConfig, INI_SECTION_BUFFER_LENGTH-1, szDesFilePath) > 0 )
	{
		WritePrivateProfileSection(szSectionName, szCmdConfig, szConfigPath);
	}
}
