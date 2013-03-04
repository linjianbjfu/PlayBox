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

	bool IGameData_GetGameByID(const std::string& strID, int nGameType, OneGame& og);
	/*��Ϸ��Ϣ��ͼƬ�ڸú����л�ȡ*/
	bool IGameData_AddGame(const OneGame& og);
	bool IGameData_DelGame(const string& strID, int nGameType );
	bool IGameData_DelGame(const std::vector<std::string>& vecID, 
		std::vector<int> vecGameType);
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
	bool DelGameInternal(const string& strID, int nGameType);
	static DWORD WINAPI ThreadDownloadPic(void* pPara);
	void AddDownloadPicPara(std::vector<DownloadPicPara>* para, GameList& gl);
	void DownloadPics();
	HANDLE	m_hThread;

	CGameDataImp();
};