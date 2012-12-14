#ifndef _GLOBAL_VARIABLE_H
#define _GLOBAL_VARIABLE_H

#include <string>

using std::string;

enum TAB_TYPE
{
	TAB_HOME,			//主页
	TAB_WEBGAME,		//web game wnd
	TAB_WEB,			//主页,搜索之外的网页：用户中心
	//以上都是html展现
	TAB_GAME,			//flash游戏wnd
	TAB_PLAYED_GAME		//玩过的游戏
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
	string		strName;
	string		strGameUrl;
};

struct SWF_GAME
{
	string		strID;
	string		strName;
	string		strPicUrl;
	string		strSwfUrl;
	string		strIntro;
};

#define RUN_MAINEXE		"小宝贝游戏盒.exe"
#define DESCRIP_MAINEXE	"小宝贝游戏盒"
#define SWF_SAVE_PATH   "PlayBoxGame"

#endif