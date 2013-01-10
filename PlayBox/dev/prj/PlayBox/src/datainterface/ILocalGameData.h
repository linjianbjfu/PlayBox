#pragma once

#include <string>
#include <vector>
#include <time.h>
using std::string;
using std::vector;

#include "IData.h"

struct OneLocalGame
{
	string	strID;	
	string	strName;
	string	strGamePath;
	string	strPicPath;
	string	strIntro;
	unsigned int uiFileSize;
	int nGameType; // 1:Flash	2:Web
	

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
	virtual bool ILocalGameData_AddGame( string strID, string strName, string strPicPath, string strSwfPath, string strIntro, unsigned int type, int nGameType, string strMD5 = "") = 0;
	virtual bool ILocalGameData_DelGame( string strID ) = 0;
	virtual bool ILocalGameData_GetAllGame( LocalGameList& lgl ) = 0;
	virtual unsigned int ILocalGameData_GetGameCount() = 0;
};

