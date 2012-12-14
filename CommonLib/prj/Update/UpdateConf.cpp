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

//szSectionName下面的data字段的value不相同，则将tempconfig里的szSectionName里的内容更新到config文件
void UpdateDateSectionConfig(const char* szSectionName)
{
	char szHomePath[YL_MAX_PATH], szTempFilePath[YL_MAX_PATH], szConfigPath[YL_MAX_PATH];
	if((!CLhcImg::GetHomePath(szHomePath, YL_MAX_PATH)) || (!CLhcImg::GetConfigPath(szConfigPath, YL_MAX_PATH)) || ((!CLhcImg::GetTempConfigPath(szTempFilePath, YL_MAX_PATH))))
	{
		return;
	}

	YL_Ini ini(szConfigPath), tmpini(szTempFilePath);
	if( tmpini.GetUInt(szSectionName, "date", 0) != ini.GetUInt(szSectionName, "date", 0))
	{
		UpdateSectionConfig(szSectionName, szConfigPath, szTempFilePath);
	}
}

void UpdateConfigFile(const char* szDesFilePath)
{
	char szConfigPath[MAX_PATH];
	if( CLhcImg::GetConfigPath(szConfigPath, MAX_PATH) )
	{
		//更新升级服务器列表
		UpdateSectionConfig("UpdateServerList", szConfigPath, szDesFilePath);
	}

	//char szMsgSetPath[MAX_PATH], szUpdateSetPath[MAX_PATH];
	//memset( szMsgSetPath, 0, MAX_PATH );
	//memset( szUpdateSetPath, 0, MAX_PATH );
	//更新升级命令配置
	//if(!CLhcImg::GetUpdateSetPath(szUpdateSetPath, MAX_PATH)) //lhpcmd.img
	//{
	//	YL_Log(UPDATE_LOG_FILE, LOG_WARNING, "update", "Get update config path error.");
	//}else
	//{
	//	UpdateSectionConfig(STR_CMD_LIMIT, szUpdateSetPath, szDesFilePath);
	//	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "update", "Get update config path success.");
	//}

	//更新实时消息配置LogMsg Config
	//if(!CLhcImg::GetMsgSetPath(szMsgSetPath, MAX_PATH)) //lhplog.img
	//{
	//	YL_Log(UPDATE_LOG_FILE, LOG_WARNING, "update", "Get msg config path error.");
	//}else
	//{
	//	UpdateSectionConfig(STR_MSG_CONFIG, szMsgSetPath, szDesFilePath);
	//	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "update", "Get msg config path success.");
	//}

	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "update", "update 0.");
	UpdateConfig();
	YL_Log(UPDATE_LOG_FILE, LOG_NOTICE, "update", "update 1.");
}