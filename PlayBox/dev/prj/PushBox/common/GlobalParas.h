#pragma once
#include "MacroInt.h"

#define REG_PBOX_KEY     "SOFTWARE\\YPLAYBOX"
#define REG_PBOX_HOME    "path"

#define STR_LOG_FILE							"main.txt"
#define STR_TEMP_MEDIA_FOLDER					"PlayBox"
#define INT_AD_LINK_URL_LENGTH					2000
#define STR_MAIN_EXE_PATH						"PlayBox.exe"

#define HKEY_PLAYBOX_ROOT HKEY_LOCAL_MACHINE
static const char* g_szSoftName				= "YPLAYBOX";
static const char* STR_REG_SOFT				= REG_PBOX_KEY;
static const wchar_t* STR_WCHAR_REG_SOFT	= L"SOFTWARE\\YPLAYBOX";
static const char* STR_REG_SOFT_MSG			= "SOFTWARE\\YPLAYBOX\\LOGMSG";
static const char* STR_REG_UPDATE			= "SOFTWARE\\YPLAYBOX\\UPDATE";
static const char* STR_REG_BHO				= "SOFTWARE\\YPLAYBOX\\BHO";
static const char* STR_SOFT_HOME_KEY		= REG_PBOX_HOME;