#ifndef _GLOBAL_VARIABLE_H
#define _GLOBAL_VARIABLE_H

#include <string>

using std::string;

enum TAB_TYPE
{
	TAB_UNKNOWN,
	TAB_HOME,			//��ҳ
	TAB_WEBGAME,		//web game wnd
	TAB_FLASHGAME,		//flash��Ϸwnd
	TAB_PLAYED_GAME,	//�������Ϸ
	TAB_BROWSER,
};

struct TAB_ITEM
{
	int			id;
	TAB_TYPE	enumType; 
	string		strParam; 
	string		strTitle; //tabbar��ʾ��title
	int			iLPDISPATCHOnlyForBrowser;
	TAB_ITEM() : id(-1), enumType(TAB_UNKNOWN), iLPDISPATCHOnlyForBrowser(0){}
};

#define RUN_MAINEXE		"���κ���.exe"
#define DESCRIP_MAINEXE	"���κ���"
#define SWF_SAVE_PATH   "PlayBoxGame"
#define BOX_DELIMITERS ";;"
#define TAB_BROWSER_DEFAULT_TITLE "�հ�ҳ"
#define TAB_GAMECENTER_TITLE "��Ϸ����"
#define TAB_GAMEINFO_TITLE "��Ϸ��Ѷ"
#define TAB_PLAYED_GAME_TITLE "�ҵĿ���"
#define TAB_WEB_GAME_CUSTOM_SERVICE_TITLE "ҳ�οͷ�"
#define TAB_PAY_TITLE "��ֵ"
#define TAB_OFFICE_SITE "�ٷ���վ"
#define TAB_REPORT_PROBLEM "���ⷴ��"

#endif