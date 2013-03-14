#include "stdafx.h"
#include "OneFlashGameControl.h"


COneFlashGameControl::COneFlashGameControl()
: m_pBrowserIntro(NULL), m_pBrowserRecommand(NULL)
{
}

COneFlashGameControl::~COneFlashGameControl()
{
	
}

void COneFlashGameControl::SetBrowserIntro(CWnd* pWnd)
{
	m_pBrowserIntro = pWnd;
}

void COneFlashGameControl::SetBrowserRecommand(CWnd* pWnd)
{
	m_pBrowserRecommand = pWnd;
}

void COneFlashGameControl::SetTabItem(TAB_ITEM ti)
{

}

void COneFlashGameControl::SetTabItem(TAB_ITEM ti)
{
	m_tabItem = ti;
	std::string strID = CWebManager::GetInstance()->GetValue( ti.strParam, "id" );
	std::string strName = CWebManager::GetInstance()->GetValue( ti.strParam, "name" );
	std::string strPicSvrUrl = CWebManager::GetInstance()->GetValue( ti.strParam, "picurl" );
	std::string strSwfSvrUrl = CWebManager::GetInstance()->GetValue( ti.strParam, "swfurl" );

	NavigateIntro(strID);
	NavigateRecommand(strID);
	m_olg.Clear();
	m_olg.strID = strID;
	m_olg.strName = strName;
	m_olg.strGameSvrPath = strSwfSvrUrl;
	m_olg.strPicSvrPath = strPicSvrUrl;
	m_olg.nGameType = OneGame::FLASH_GAME | OneGame::RECENT_PLAY;
	GLOBAL_GAME->IGameData_AddGame(m_olg);

	std::string strLocalPicPath;
	if (m_olg.GetLocalPicPath(strLocalPicPath) && 
		!YL_FileInfo::IsValid(strLocalPicPath) )
	{
		YL_CHTTPDownFile httpDownFile;
		httpDownFile.DownloadFile( m_olg.strPicSvrPath, strLocalPicPath );
	}

	std::string strLocalSwfPath;
	if (m_olg.GetLocalSwfPath(strLocalSwfPath))
	{
		if (YL_FileInfo::IsValid(strLocalSwfPath))
		{
			m_bDown = false;
			PlayMovie(strLocalSwfPath);
		}else
		{
			m_bDown = true;
			HttpDownCtrl::GetInstance()->StartDown( m_olg.strID, m_olg.strGameSvrPath );
		}
		UpdateAllWnd();
	}
}

void COneFlashGameControl::NavigateIntro(std::string strID)
{
	std::string strFlashGameRightUrl;
	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_CONFIG_FLASH_GAME_RIGHT_URL,strFlashGameRightUrl);
	if (!strFlashGameRightUrl.empty())
		m_pBrowserIntro->Navigate(strFlashGameRightUrl+strID);
}

void COneFlashGameControl::NavigateRecommand(std::string strID)
{
	std::string strFlashGameBottomtUrl;
	AfxGetUserConfig()->GetConfigStringValue(CONF_SETTING_MODULE_NAME, 
		CONF_SETTING_CONFIG_FLASH_GAME_BOTTOM_URL,strFlashGameBottomtUrl);
	if (!strFlashGameBottomtUrl.empty())
		m_pBrowserRecommand->Navigate(strFlashGameBottomtUrl+strID);
}