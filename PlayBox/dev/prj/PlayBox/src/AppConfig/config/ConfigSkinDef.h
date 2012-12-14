#ifndef _CONFIG_SKIN_DEF_H
#define _CONFIG_SKIN_DEF_H

#include "AfxConfigType.h"

static const char* CONF_SKINSUB_MODULE_NAME	= "SkinSub";
static const char* CONF_SKINSUB_CURRSUB		= "CurSub";
static const char* CONF_SKINSUB_COLOR		= "Color";

static const char* CONF_SKINSUB[] =
{ 
	CONF_SKINSUB_CURRSUB,				TYPE_CONFIG_STR,	
	CONF_SKINSUB_COLOR,					TYPE_CONFIG_STR
};

#endif