// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <string>
#include "atlstr.h"

using namespace std;

//日志上传命令
#define STR_UPLOAD_LOG_KEY               "Upload_Time"                   

#define STR_MSG_CONFIG                   "LogMsg"
//命令配置文件
#define STR_CMD_LIMIT                    "CommandLimit"
#define STR_APPID_DAILY_TIMES_LIMIT      "AppIDDailyLimit"
#define STR_CHECK_UPDATE_INTERVAL        "CheckUpdateInterval"
#define STR_CHECK_UPDATE_TIMES_LIMIT     "CheckUpdateTimesLimit"

#define STR_TEST_SPEED_TIMES_LIMIT       "TestSpeedTimesLimit"
#define STR_TEST_SPEED_DAYS_LIMIT        "TestSpeedDaysLimit"

#define STR_DOWNLOAD_TIMES_LIMIT         "DownloadTimesLimit"
#define STR_DOWNLOAD_DAYS_LIMIT          "DownloadDaysLimit"

#define STR_INSTALL_TIMES_LIMIT          "InstallTimesLimit"
#define STR_INSTALL_DAYS_LIMIT           "InstallDaysLimit"

//注册表记录的当前命令状态
#define STR_CURRENT_CMD                  "CURRENT_CMD"
#define STR_CMD_LAST_DATE_KEY            "CMD_LAST_DATE"
#define STR_CMD_LAST_TIME_KEY            "CMD_LAST_TIME"
#define STR_CMD_EXEC_TIMES_KEY           "CMD_EXEC_TIMES"
#define STR_CMD_EXEC_DAYS_KEY            "CMD_EXEC_DAYS"
#define STR_APPID_TIMES                  "AppID_Times"
#define STR_APPID_LAST_DATE              "AppID_Last_Date"
#define STR_LAST_VERSION                 "Last_Version"

#define STR_CMD_CHECK_UPDATE             "Check_Update"
#define STR_CMD_TEST_SPEED               "Test_Speed"
#define STR_CMD_DOWNLOAD_FILE            "Download_File"
#define STR_CMD_INSTALL_SOFT             "Install_Soft"

#define STR_CHECK_UPDATE_LAST_DATE_KEY   "Check_Update_Last_Date"
#define STR_CHECK_UPDATE_TIMES_KEY       "Check_Update_Times"

//BHO安装标记
#define STR_FIRST_START                  "First_Start"

#define UPDATE_LOG_FILE                  "update.log"