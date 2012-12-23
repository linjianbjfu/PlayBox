#ifndef _CONFIG_APP_DEF_H
#define _CONFIG_APP_DEF_H

#include "AfxConfigType.h"

static const char* CONF_APP_MODULE_NAME		= "App";

static const char* CONF_APP_SWF_PATH			= "SwfPath";
static const char* CONF_APP_MAINWND_HOLD		= "MainWndHold";
static const char* CONF_APP_RUN_COUNT			= "RunCount";
static const char* CONF_APP_PLAYED_GAME			= "PlayedGame";
static const char* CONF_APP_PLAYED_PC_GAME		= "playedPcGame";
static const char* CONF_APP_PLAYED_ICON_SIZE	= "PlayedIconSize";

static const char* CONF_APP_PIC_SAVE_PATH		= "PictureSavePath";
static const char* BOTTOM_AD_JSON				= "BottomADJson";


static const char* CONF_APP[] =
{ 
	CONF_APP_SWF_PATH,			TYPE_CONFIG_STR,
	CONF_APP_MAINWND_HOLD,		TYPE_CONFIG_BOOL,
	CONF_APP_RUN_COUNT,			TYPE_CONFIG_INT,
	CONF_APP_PLAYED_GAME,		TYPE_CONFIG_INT,
	CONF_APP_PLAYED_PC_GAME,	TYPE_CONFIG_INT,
	CONF_APP_PLAYED_ICON_SIZE,	TYPE_CONFIG_INT, //玩过的游戏panel显示icon的大小0,1,2
	CONF_APP_PIC_SAVE_PATH,		TYPE_CONFIG_STR,
	BOTTOM_AD_JSON,				TYPE_CONFIG_STR
};

#endif