#ifndef _GLOBAL_VARIABLE_H
#define _GLOBAL_VARIABLE_H

#include <string>

using std::string;

enum TAB_TYPE
{
	TAB_HOME,			//��ҳ
	TAB_WEBGAME,		//web game wnd
	TAB_FLASHGAME,			//flash��Ϸwnd
	TAB_PLAYED_GAME,	//�������Ϸ
	TAB_BROWSER	
};

struct TAB_ITEM
{
	string		strName;
	TAB_TYPE	eumType;
	string		strParam;
};

struct WEB_GAME
{
	string		strID;
	string		strSvrID;
	string		strName;
	string		strPicUrl;
};

struct SWF_GAME
{
	string		strID;
	string		strName;
	string		strPicUrl;
	string		strSwfUrl;
	string		strIntro;
};

#define RUN_MAINEXE		"С������Ϸ��.exe"
#define DESCRIP_MAINEXE	"С������Ϸ��"
#define SWF_SAVE_PATH   "PlayBoxGame"

#endif