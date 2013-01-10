#pragma once

#include <string>
#include <vector>
#include <time.h>
using std::string;
using std::vector;

#include "IData.h"

struct OneLocalGame
{
	enum {TYPE_FLASH_GAME=1, TYPE_WEB_GAME};
	string	strID;	
	string	strName;
	string	strGamePath;
	string	strPicPath;
	string	strIntro;
	unsigned int uiFileSize;
	int nGameType; // 1:Flash	2:Web
	string strSrvID; // srvid (webgame only)
	

	OneLocalGame()
	{
		clear();
	}
	void clear()
	{
		strID.clear();
		strName.clear();
		strGamePath.clear();
		strPicPath.clear();
		strIntro.clear();
		uiFileSize    = 0;
		nGameType = 0;
	}
};

typedef vector<OneLocalGame> LocalGameList;

class ILocalGameData : public IData
{
public:
	virtual bool ILocalGameData_GetGameByID( string strID, OneLocalGame& og ) = 0;
	virtual bool ILocalGameData_AddGame( OneLocalGame og, string strMD5 = "") = 0;
	virtual bool ILocalGameData_DelGame( string strID ) = 0;
	virtual bool ILocalGameData_GetAllGame( LocalGameList& lgl ) = 0;
	virtual unsigned int ILocalGameData_GetGameCount() = 0;

	virtual int ILocalGameData_GetWebGame(LocalGameList& lgl) = 0;
	virtual int ILocalGameData_GetFlashGame(LocalGameList& lgl) = 0;
};

