#ifndef _CONFIG_LAYOUT_DEF_H
#define _CONFIG_LAYOUT_DEF_H

#include "AfxConfigType.h"

static const char* CONF_LAYOUT_MODULE_NAME	= "Layout";
static const char* CONF_LAYOUT_NORMAL		= "Normal";
static const char* CONF_LAYOUT_X_POS		= "xPos";
static const char* CONF_LAYOUT_Y_POS		= "yPos";
static const char* CONF_LAYOUT_WIDTH		= "Width";
static const char* CONF_LAYOUT_HEIGHT		= "Height";
static const char* CONF_LAYOUT_MIN_WIDTH	= "mincx";
static const char* CONF_LAYOUT_MIN_HEIGHT	= "mincy";
static const char* CONF_LAYOUT_ESC_ALL		= "EscFullAllValue";
static const char* CONF_LAYOUT_HAS_CLICK_GAME_INFO	= "HasClickGameInfo";

static const char* CONF_LAYOUT[] =
{ 
	CONF_LAYOUT_NORMAL,		TYPE_CONFIG_BOOL,
	CONF_LAYOUT_X_POS,		TYPE_CONFIG_INT,
	CONF_LAYOUT_Y_POS,		TYPE_CONFIG_INT,
	CONF_LAYOUT_WIDTH,		TYPE_CONFIG_INT,
	CONF_LAYOUT_HEIGHT,		TYPE_CONFIG_INT,
	CONF_LAYOUT_MIN_WIDTH,  TYPE_CONFIG_INT,
	CONF_LAYOUT_MIN_HEIGHT, TYPE_CONFIG_INT,
	CONF_LAYOUT_ESC_ALL,	TYPE_CONFIG_INT,
	CONF_LAYOUT_HAS_CLICK_GAME_INFO, TYPE_CONFIG_BOOL
};

#endif