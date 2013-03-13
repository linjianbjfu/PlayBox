#ifndef _WEB_HANDLER_H
#define _WEB_HANDLER_H

#include "WebPage.h"

struct CallJSParam
{
	UINT nParam;
	string strFunc;
	string strParam1;
	string strParam2;
	string strParam3;
};

class CWebManager
{
public:
	CWebManager();
	~CWebManager();
	static CWebManager*		m_pSelf;
	static CWebManager*		GetInstance();
	static void				DelInstance();

	void	OnPlay(const char* );
	void	OnPlayWebGame(const char* );
	void	CallGBoxFromWeb(const string& strCommand,string& strRes);
	void    CallWebFromGBox(CHtmlView* pHTMLView, CallJSParam * pParam/*const string& strCommand*/);

	string	GetValue( string& strContent, string strKey );

private:
	CWebPage	m_WebPage;
	void	_string_help(const CString& strInput,CString& strLeft,CString& strRight);
	string	_command_playswfgame(string& strContent);
	string	_command_playwebgame(string& strContent);
	string	_command_refresh( string& strContent );
	string	_command_playexegame (string & strContent);
	string  _command_openBrowser (string & strContent);
	string  _command_login (string & strContent);
	string  _command_collect_game (string & strContent);
	string  _command_close_find_pass_and_open_browser(string & strContent);
	void	NotifyWebRefresh( const  char* psz);
};

#endif