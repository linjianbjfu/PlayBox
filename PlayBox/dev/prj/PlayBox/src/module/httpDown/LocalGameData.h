#pragma once
#include "StdAfx.h"
#include "../../DataInterface/ILocalGameData.h"

class LocalGameData : public ILocalGameData
{
private:
	LocalGameList	m_vecGame;	
	string			m_strDataFilePath;	//������Ϸ����data�ļ�·��
public:
	static		LocalGameData*	m_pData;
	static		IData*			GetInstance();
	static		void			DelInstance();
	
	void		DataAppStart();
	void		DataAppExit();
private:
	void		LoadGameData();
	void		UnLoadGameData();
	void		NotifyQQItemCountChange();
public:
	virtual bool ILocalGameData_GetGameByID( string strID, OneLocalGame& og );
	/*��Ϸ��Ϣ��ͼƬ�ڸú����л�ȡ*/
	virtual bool ILocalGameData_AddGame( string strID, string strName, 
		string strPicPath, string strSwfPath, string strIntro, unsigned int type, string strMD5 = "");
	virtual bool ILocalGameData_DelGame( string strID );
	virtual bool ILocalGameData_GetAllGame( LocalGameList& lgl );
	virtual unsigned int ILocalGameData_GetGameCount();
};