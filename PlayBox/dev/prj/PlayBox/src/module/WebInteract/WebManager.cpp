#include "stdafx.h"
#include "WebManager.h"
#include "YL_StringUtil.h"
#include "tools.h"
#include "../../datainterface/ITabBarData.h"
#include "../../datainterface/IWebRefreshObserver.h"
#include "../../Core/CDataManager.h"
#include "YL_DirInfo.h"
#include "MyWebBrowserWnd.h"
#include "../../AppConfig/config/ConfigSettingDef.h"
#include "../../AppConfig/config/ConfigAppDef.h"
#include "../TabMan/TabPageControl.h"
#include "../UserMan/UserManager.h"

CWebManager* CWebManager::m_pSelf = NULL;

CWebManager::CWebManager()
{
}
CWebManager::~CWebManager()
{
}

CWebManager* CWebManager::GetInstance()
{	
	if( m_pSelf == NULL )
	{
		m_pSelf = new CWebManager();
	}
	return m_pSelf;
}

void CWebManager::DelInstance()
{
	if( m_pSelf != NULL )
	{
		delete m_pSelf;
	}
}

void CWebManager::CallGBoxFromWeb(const string& strCommand,string& strRes)
{
	string strHeader;
	string strContent;

	size_t iHeader = strCommand.find(BOX_DELIMITERS);
	if( iHeader == string::npos)
		return;
	
	strHeader	= strCommand.substr(0,iHeader);
	strContent	= strCommand.substr(iHeader+strlen(BOX_DELIMITERS));
	//header
	CString str(strHeader.c_str());
	CString strLeft,strRight;
	_string_help( str,strLeft,strRight );

	if( strLeft == "method" )
	{
		if( strRight == "flashgame")//点击玩flash游戏
			strRes = _command_playswfgame( strContent );
		else
		if( strRight == "webgame")//点击玩web游戏（也可玩嵌在网页里的flash游戏）
			strRes = _command_playwebgame( strContent );
		else
		if( strRight == "refresh" )
			strRes = _command_refresh( strContent );
		else
		if (strRight == "browser")
			strRes = _command_openBrowser(strContent);
		else
		if (strRight == "login")
			strRes = _command_login(strContent);
		else
		if (strRight == "checkdlstatus")
			strRes = _command_checkdlstatus (strContent);
	}
}

void CWebManager::NotifyWebRefresh( const  char* psz)
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_REFRESH,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IWebRefreshObserver* pOb = dynamic_cast<IWebRefreshObserver*>(*itOb);
		pOb->IWebRefreshOb_Refresh( psz);
	}
}
string CWebManager::_command_refresh( string& strContent )
{
	string strUrl   = GetValue( strContent, "url" );
	if( strUrl.length() != 0 )
	{
		NotifyWebRefresh( strUrl.c_str() );
		return "res=ok";
	}	
	return "res=err";
}

string CWebManager::_command_playswfgame(string& strContent)
{	
	string strID   = GetValue( strContent, "id" );
	string strName = GetValue( strContent, "name" );
	string strUrl  = GetValue( strContent, "swfurl" );

	//判断id是否OK
	bool bIDOK = false;
	if( strID.length() > 0 && IsNumber( strID ) ) //ID必须是数字
		bIDOK = true;

	//判断name是否OK
	bool bNameOK = false;
	if( strName.length() != 0 )
		bNameOK = true;

	//判断url是否OK
	bool bUrlOK = false;
	CString cstrUrl = CString( strUrl.c_str() );
	if( cstrUrl.Right(4).MakeLower() == ".swf" ) //url末4位必须是.swf
		bUrlOK = true;
	
	if( bIDOK && bNameOK && bUrlOK )
	{
		TAB_ITEM tItem;
		tItem.enumType = TAB_FLASHGAME;
		tItem.strTitle = strName;
		tItem.strParam = strContent;
		GLOBAL_TABBARDATA->ITabBar_ChangeTab(tItem);
		return "res=ok";
	}else
		return "res=err";
}

string CWebManager::_command_playwebgame(string& strContent)
{	
	string strID   = GetValue( strContent, "id" );
	string strName = GetValue( strContent, "name" );
	
	if( strID.length() > 0 && IsNumber( strID ) && !strName.empty() )	
	{
		TAB_ITEM tItem;
		tItem.enumType = TAB_WEBGAME;
		tItem.strTitle = strName;
		tItem.strParam = strContent;
		GLOBAL_TABBARDATA->ITabBar_ChangeTab(tItem);
		return "res=ok";
	}	
	return "res=err";
}

string CWebManager::_command_openBrowser (string & strContent)
{
	TAB_ITEM tItem;
	tItem.enumType = TAB_BROWSER;
	tItem.strTitle = TAB_BROWSER_DEFAULT_TITLE;
	tItem.strParam = strContent;
	GLOBAL_TABBARDATA->ITabBar_ChangeTab(tItem);
	return "";
}

string CWebManager::_command_login(string & strContent)
{
	string strName = GetValue (strContent, "username");
	string strMD5Pass = GetValue (strContent, "pass");
	if (!strName.empty() && !strMD5Pass.empty())
	{
		CUserManager::GetInstance()->User_Login(strName.c_str(), strMD5Pass.c_str(), true);
		return "res=ok";
	}else
		return "res=err";
}

string CWebManager::_command_checkdlstatus (string &strContent)
{
	string strID = GetValue (strContent, "id");
	OneGame olg;
	string strStatus  = 
		GLOBAL_GAME->IGameData_GetGameByID (strID, OneGame::FLASH_GAME, olg) ? "run" : "download";

	vector<TAB_ITEM> vTab;
	GLOBAL_TABBARDATA->ITabBar_GetTabBarData (vTab);

	CallJSParam * pParam = new CallJSParam;
	memset (pParam, 0, sizeof (CallJSParam));

	pParam->strFunc = "setdownloadpic";
	pParam->nParam = 2;
	pParam->strParam1 = strID;
	pParam->strParam2 = strStatus;

	CTabPageControl::GetInstance ()->CallJS ((LPVOID)pParam);
	delete pParam;
	return "";
}

string CWebManager::GetValue( string& strContent, string strKey )
{
	string strRes;
	CString str(strContent.c_str());
	CString strOneItem;
	int curPos = 0;
	strOneItem= str.Tokenize(BOX_DELIMITERS,curPos);
	while(strOneItem != "")
	{		
		CString strLeft,strRight;
		_string_help( strOneItem,strLeft,strRight );
		if( strLeft.Compare(strKey.c_str()) == 0 )
		{
			strRes = strRight;
			break;
		}
		strOneItem = str.Tokenize(BOX_DELIMITERS, curPos);
	};
	return strRes;
}

void CWebManager::CallWebFromGBox(CHtmlView* pHTMLView, CallJSParam * pParam/*const string& strCommand*/)
{
	if (pParam == NULL || pParam->nParam > 3)
		return;

	if( pHTMLView)
	{
		m_WebPage.SetDocument( pHTMLView->GetHtmlDocument() );
// 		m_WebPage.CallJScript("setdownloadpic",strCommand.c_str());
		switch (pParam->nParam)
		{
		case 0:
			m_WebPage.CallJScript (pParam->strFunc.c_str ());
			break;
		case 1:
			m_WebPage.CallJScript (pParam->strFunc.c_str (), pParam->strParam1.c_str ());
			break;
		case 2:
			m_WebPage.CallJScript (pParam->strFunc.c_str (), pParam->strParam1.c_str (), pParam->strParam2.c_str ());
			break;
		case 3:
			m_WebPage.CallJScript (pParam->strFunc.c_str (), pParam->strParam1.c_str (), pParam->strParam2.c_str (), pParam->strParam3.c_str ());
			break;
		default:
			break;
		}
	}
}

void CWebManager::_string_help(const CString& strInput,CString& strLeft,CString& strRight)
{
	strLeft = strRight = "";

	int iPosEq = strInput.Find('=');
	if( iPosEq == -1 )
		return;
	strLeft		= strInput.Left(iPosEq);
	strRight	= strInput.Mid(iPosEq+1,strInput.GetLength()-iPosEq-1);
}