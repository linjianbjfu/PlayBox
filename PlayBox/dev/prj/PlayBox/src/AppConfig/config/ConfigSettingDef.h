#ifndef _CONFIG_SETTING_DEF_H
#define _CONFIG_SETTING_DEF_H

#include "AfxConfigType.h"

static const char* CONF_SETTING_MODULE_NAME				= "Setting";

static const char* CONF_SETTING_SKIN_IDX				= "skin";
static const char* CONF_SETTING_CONFIG_MAIN_PAGE		= "MainPage";
static const char* CONF_SETTING_CONFIG_PROBLEM_REPORT	= "ProblemReport";
static const char* CONF_SETTING_CONFIG_WEB_GAME_CUSTOM_SERVICE	= "WebGameCustomService";
static const char* CONF_SETTING_CONFIG_PAY				= "PayUrl";
static const char* CONF_SETTING_CONFIG_HOME_PAGE		= "HomePage";
static const char* CONF_SETTING_CONFIG_WEB_GAME_URL		= "WebGameUrl";
static const char* CONF_SETTING_CONFIG_IFASKEXIT		= "askexit";
static const char* CONF_SETTING_CONFIG_EXITCHOICE		= "exitchoice";
static const char* CONF_SETTING_LOGIN_USER_NAME			= "UN";
static const char* CONF_SETTING_LOGIN_PASSWORD			= "PW";
static const char* CONF_SETTING_NETID					= "NetID";
static const char* CONF_SETTING_COLOR					= "ChangeColorValue";
static const char* CONF_SETTING_LIGHT					= "changeColorLight";
static const char* CONF_SETTING_COLOR_BTN_INDEX			= "ColorIndex";
static const char* CONF_SETTING_COLOR_USER				= "ColorUser";
static const char* CONF_SETTING_FIRST_TIME_CHANGESKIN	= "FirstTimeChangeSkin";

static const char* CONF_SETTING_ENABLE_BOSS_KEY			= "UseBossKey";
static const char* CONF_SETTING_BOSS_KEY_VALUE			= "BossKeyValue";
static const char* CONF_SETTING[] =
{
	CONF_SETTING_SKIN_IDX,				TYPE_CONFIG_STR,
	CONF_SETTING_CONFIG_MAIN_PAGE,		TYPE_CONFIG_STR,
	CONF_SETTING_CONFIG_PROBLEM_REPORT,	TYPE_CONFIG_STR,
	CONF_SETTING_CONFIG_WEB_GAME_CUSTOM_SERVICE,	TYPE_CONFIG_STR,
	CONF_SETTING_CONFIG_HOME_PAGE,		TYPE_CONFIG_STR,
	CONF_SETTING_CONFIG_WEB_GAME_URL,	TYPE_CONFIG_STR,
	CONF_SETTING_CONFIG_IFASKEXIT,		TYPE_CONFIG_BOOL,
	CONF_SETTING_CONFIG_EXITCHOICE,		TYPE_CONFIG_BOOL,
	CONF_SETTING_LOGIN_USER_NAME,		TYPE_CONFIG_STR,
	CONF_SETTING_LOGIN_PASSWORD,		TYPE_CONFIG_STR,
	CONF_SETTING_NETID,					TYPE_CONFIG_STR,
	CONF_SETTING_COLOR,					TYPE_CONFIG_INT,
	CONF_SETTING_LIGHT,					TYPE_CONFIG_INT,
	CONF_SETTING_COLOR_BTN_INDEX,		TYPE_CONFIG_INT,
	CONF_SETTING_COLOR_USER,			TYPE_CONFIG_INT,
	CONF_SETTING_FIRST_TIME_CHANGESKIN, TYPE_CONFIG_BOOL,
	CONF_SETTING_ENABLE_BOSS_KEY,		TYPE_CONFIG_BOOL,
	CONF_SETTING_BOSS_KEY_VALUE,		TYPE_CONFIG_INT
};

#define ID_HOTKEY_BOSSKEY		10086

#if !defined CONF_DLG_CTRL_ID
#define CONF_DLG_CTRL_ID

#define	IDC_CONF_BTN_OK				1001
#define IDC_CONF_BTN_CANCEL			1002
#define IDC_CONF_BTN_RESET			1003

#define IDC_CONF_CHK_AUTORUN		1004
#define IDC_CONF_CHK_ENABLE_BOSSKEY	1005
#define IDC_CONF_CHK_PRINTSCREEN	1006
#define IDC_CONF_RAD_MIN			1007
#define IDC_CONF_RAD_EXIT			1008
#define IDC_CONF_CHK_NOASK			1009

#endif // CONF_DLG_CTRL_ID

#endif