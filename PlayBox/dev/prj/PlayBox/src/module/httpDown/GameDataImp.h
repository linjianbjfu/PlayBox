#pragma once
#include "../../DataInterface/IGameData.h"

class CGameDataImp : public IGameData
{
public:
	enum GameType {	FLASH, WEB };
	enum Action { ADD, DEL };
	enum Status { PLAYED, COLLECT };
	static		CGameDataImp*	m_pData;
	static		IData*			GetInstance();
	static		void			DelInstance();
	
	void		DataAppStart();
	void		DataAppExit();

	bool IGameData_GetGameByID(int nGameType, const std::string& strGameID, 
		const std::string& strSvrID, OneGame& og);
	/*��Ϸ��Ϣ��ͼƬ�ڸú����л�ȡ*/
	bool IGameData_AddGame(const OneGame& og);
	bool IGameData_DelGame(const std::vector<GameKey>& vec);
	bool IGameData_GetGame(GameList& lgl, int iGameType);
	unsigned int IGameData_GetGameCount();
	void IGameData_SetLoginGameList(GameList& lgl);
	void IGameData_ChangeLoginState(bool bLogin);
	void IGameData_UserGameChangeInfoToSvr(GameType gt, Action action,
		Status s, const std::string& strGameID, const std::string& strSvrID); //�û��ղأ��������Ϸ��Ϣ����server
private:
	struct DownloadPicPara
	{
		std::string strPicSvrUrl;
		std::string strPicLocalPath;
	};

	bool m_bLogin;
	GameList*	m_pVecGame;
	GameList	m_vecLocalGame;
	GameList	m_vecLoginGame;
	string		m_strDataFilePath;	//������Ϸ����data�ļ�·��
	void		LoadGameData();
	void		UnLoadGameData();
	static void NotifyGameDataChanged();
	bool DelGameInternal(int nGameType, const string& strID, const string& strSvrID);
	static DWORD WINAPI ThreadDownloadPic(void* pPara);
	void AddDownloadPicPara(std::vector<DownloadPicPara>* para, GameList& gl);
	void DownloadPics();
	HANDLE	m_hThread;

	CGameDataImp();
};