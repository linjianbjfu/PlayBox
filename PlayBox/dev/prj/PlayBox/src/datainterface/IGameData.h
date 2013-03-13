#pragma once
#include <string>
#include <vector>
#include <time.h>
#include "IData.h"
#include "YL_FileInfo.h"
#include "Global\GlobalSwfPath.h"
#include "YL_StringUtil.h"

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
	std::string	strGameSvrPath;
	std::string	strPicSvrPath;
	std::string strSrvID; // srvid (webgame only)
	std::string strIntro;
	std::string strAddTime;
	int nGameType; //Type

	OneGame() : nGameType(0) { }
	void Clear()
	{
		strID.clear();	
		strName.clear();
		strGameSvrPath.clear();
		strPicSvrPath.clear();
		strSrvID.clear();
		strIntro.clear();
		strAddTime.clear();
		nGameType = 0;
	}
	bool GetLocalPicPath(std::string& strPath)
	{
		strPath.clear();
		if (!strID.empty() && !strPicSvrPath.empty())
		{
			std::string strPicFormat;
			YL_FileInfo::GetFileNameSuffix(strPicSvrPath, strPicFormat);

			string strSwfSavePath;
			GlobalSwfPath::GetConfigSwfPath(strSwfSavePath);
			if (strSwfSavePath.empty())
				return false;
			if (strSwfSavePath.at(strSwfSavePath.length()-1) != '\\')
				strSwfSavePath += "\\";

			if (nGameType & FLASH_GAME)
			{
				YL_StringUtil::Format( strPath, "%s%s.%s", 
					strSwfSavePath.c_str(), 
					strID.c_str(), 
					strPicFormat.c_str() );
			}else if (nGameType & WEB_GAME)
			{
				YL_StringUtil::Format( strPath, "%s%s_%s.%s", 
					strSwfSavePath.c_str(), 
					strID.c_str(),
					strSrvID.c_str(),
					strPicFormat.c_str() );
			}
		}
		return !strPath.empty();
	}

	bool GetLocalSwfPath(std::string& strPath)
	{
		strPath.clear();
		if (!strID.empty() && (nGameType & FLASH_GAME))
		{
			string strSwfSavePath;
			GlobalSwfPath::GetConfigSwfPath(strSwfSavePath);
			if (strSwfSavePath.empty())
				return false;
			if (strSwfSavePath.at(strSwfSavePath.length()-1) != '\\')
				strSwfSavePath += "\\";

			YL_StringUtil::Format( strPath, "%s%s.swf", 
				strSwfSavePath.c_str(), 
				strID.c_str());
		}
		return !strPath.empty();
	}
};

typedef std::vector<OneGame> GameList;

struct GameKey
{
	int nGameType;
	string strGameID;
	string strSvrID;
};

class IGameData : public IData
{
public:
	virtual bool IGameData_GetGameByID(int nGameType, const std::string& strGameID, 
		const std::string& strSvrID, OneGame& og) = 0;
	virtual bool IGameData_AddGame(const OneGame& og) = 0;
	virtual bool IGameData_DelGame(const std::vector<GameKey>& vec) = 0;
	virtual bool IGameData_GetGame(GameList& lgl, int iGameType) = 0;
	virtual unsigned int IGameData_GetGameCount() = 0;
	virtual void IGameData_SetLoginGameList(GameList& lgl) = 0;
	virtual void IGameData_ChangeLoginState(bool bLogin) = 0;
};