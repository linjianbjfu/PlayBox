#pragma once
#include "../../DataInterface/IGameData.h"

class CGameDataImp : public IGameData
{
public:
	static		CGameDataImp*	m_pData;
	static		IData*			GetInstance();
	static		void			DelInstance();
	
	void		DataAppStart();
	void		DataAppExit();

	bool IGameData_GetGameByID(const std::string& strID, int nGameType, OneGame& og);
	/*游戏信息和图片在该函数中获取*/
	bool IGameData_AddGame(const OneGame& og);
	bool IGameData_DelGame(const string& strID, int nGameType );
	bool IGameData_GetGame(GameList& lgl, int iGameType);
	unsigned int IGameData_GetGameCount();
	void IGameData_SetLoginGameList(GameList& lgl);
	void IGameData_ChangeLoginState(bool bLogin);
private:
	bool m_bLogin;
	GameList&	m_vecGame;
	GameList	m_vecLocalGame;
	GameList	m_vecLoginGame;
	string			m_strDataFilePath;	//本地游戏数据data文件路径
	void		LoadGameData();
	void		UnLoadGameData();
	CGameDataImp();
};