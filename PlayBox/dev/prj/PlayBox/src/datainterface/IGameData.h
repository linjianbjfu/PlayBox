#pragma once
#include <string>
#include <vector>
#include <time.h>
#include "IData.h"

struct OneGame
{
	enum Type
	{
		FLASH_GAME  = 1 << 0, 
		WEB_GAME    = 1 << 1,
		COLLECTED   = 1 << 2,
		RECENT_PLAY = 1 << 3
	};
	std::string	strID;	
	std::string	strName;
	std::string	strGamePath;
	std::string	strPicPath;
	std::string	strIntro;
	std::string strSrvID; // srvid (webgame only)
	std::string strAddTime; // Ìí¼ÓÊ±¼ä eg:2013-1-13 12:50
	int nGameType; //Type

	OneGame()	{ clear();}
	void clear()
	{
		strID.clear();
		strName.clear();
		strGamePath.clear();
		strPicPath.clear();
		strIntro.clear();
		nGameType = 0;
	}
};

typedef std::vector<OneGame> GameList;
class IGameData : public IData
{
public:
	virtual bool IGameData_GetGameByID(const std::string& strID, int nGameType, OneGame& og) = 0;
	virtual bool IGameData_AddGame(const OneGame& og) = 0;
	virtual bool IGameData_DelGame(const string& strID , int nGameType) = 0;
	virtual bool IGameData_GetGame(GameList& lgl, int iGameType) = 0;
	virtual unsigned int IGameData_GetGameCount() = 0;
	virtual void IGameData_SetLoginGameList(GameList& lgl) = 0;
	virtual void IGameData_ChangeLoginState(bool bLogin) = 0;
};