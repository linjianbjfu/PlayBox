#ifndef _GLOBAL_VARIABLE_H
#define _GLOBAL_VARIABLE_H

#include <string>

using std::string;

enum TAB_TYPE
{
	TAB_UNKNOWN,
	TAB_HOME,			//主页
	TAB_WEBGAME,		//web game wnd
	TAB_FLASHGAME,		//flash游戏wnd
	TAB_PLAYED_GAME,	//玩过的游戏
	TAB_BROWSER,
};

struct TAB_ITEM
{
	int			id;
	TAB_TYPE	enumType; 
	string		strParam; 
	string		strTitle; //tabbar显示的title
	int			iLPDISPATCHOnlyForBrowser;
	TAB_ITEM() : id(-1), enumType(TAB_UNKNOWN), iLPDISPATCHOnlyForBrowser(0){}
};

#define RUN_MAINEXE		"酷游盒子.exe"
#define DESCRIP_MAINEXE	"酷游盒子"
#define SWF_SAVE_PATH   "PlayBoxGame"
#define BOX_DELIMITERS ";;"
#define TAB_BROWSER_DEFAULT_TITLE "空白页"
#define TAB_GAMECENTER_TITLE "游戏中心"
#define TAB_GAMEINFO_TITLE "游戏资讯"
#define TAB_PLAYED_GAME_TITLE "我的酷游"
#define TAB_WEB_GAME_CUSTOM_SERVICE_TITLE "页游客服"
#define TAB_PAY_TITLE "充值"
#define TAB_OFFICE_SITE "官方网站"
#define TAB_REPORT_PROBLEM "问题反馈"

#endif