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
	YL_Log("KPlugin.txt",LOG_DEBUG,"CallKwMusic","%s",strCommand.c_str() );

	string strHeader;
	string strContent;

	size_t iHeader = strCommand.find("\n\n");
	if( iHeader == string::npos)
	{
		return;
	}
	strHeader	= strCommand.substr(0,iHeader);
	strContent	= strCommand.substr(iHeader+2);
	//header
	CString str(strHeader.c_str());
	CString resToken;
	int curPos = 0;

	resToken= str.Tokenize("\n",curPos);
	while (resToken != "")
	{
		CString strLeft,strRight;
		_string_help( resToken,strLeft,strRight );

		if( strLeft == "method" )
		{
			if( strRight == "flashgame")//点击玩flash游戏
			{
				strRes = _command_playswfgame( strContent );
				return;			
			}else
			if( strRight == "webgame")//点击玩web游戏（也可玩嵌在网页里的flash游戏）
			{
				strRes = _command_playwebgame( strContent );
				return;			
			}else
			if( strRight == "refresh" )
			{
				strRes = _command_refresh( strContent );
				return;
			}else
			if (strRight == "opennewtab")
			{
				strRes = _command_opennewtab (strContent);
				return;
			}else
			if (strRight == "checkdlstatus")
			{
				strRes = _command_checkdlstatus (strContent);
				return;
			}
		}
		resToken = str.Tokenize("\n", curPos);
	};   
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
		return "res=ok\n\n";
	}	
	return "res=err\n\n";
}

string CWebManager::_command_playswfgame(string& strContent)
{	
	string strID   = GetValue( strContent, "id" );
	string strName = GetValue( strContent, "name" );
	string strUrl  = GetValue( strContent, "swfurl" );

	//判断id是否OK
	bool bIDOK = false;
	if( strID.length() > 0 && IsNumber( strID ) ) //ID必须是数字
	{
		bIDOK = true;
	}
	//判断name是否OK
	bool bNameOK = false;
	if( strName.length() != 0 )
	{
		bNameOK = true;
	}
	//判断url是否OK
	bool bUrlOK = false;
	CString cstrUrl = CString( strUrl.c_str() );
	if( cstrUrl.Right(4).MakeLower() == ".swf" ) //url末4位必须是.swf
	{
		bUrlOK = true;
	}	
	
	if( bIDOK && bNameOK && bUrlOK )
	{
		TAB_ITEM tItem;
		tItem.eumType = TAB_GAME;
		tItem.strName = strName;
		tItem.strParam = strContent;
		GLOBAL_TABBARDATA->ITabBar_ChangeTab(tItem);

		string s = "PlaySwfGame:" + strID;
		LogRealMsg("PLAYGAME",s.c_str());
		return "res=ok\n\n";
	}else
	{
		return "res=err\n\n";
	}	
}

string CWebManager::_command_playwebgame(string& strContent)
{	
	string strID   = GetValue( strContent, "id" );
	string strName = GetValue( strContent, "name" );
	string strUrl  = GetValue( strContent, "url" );

	//判断id是否OK
	bool bIDOK = false;
	if( strID.length() > 0 && IsNumber( strID ) ) //ID必须是数字
	{
		bIDOK = true;
	}
	//判断name是否OK
	bool bNameOK = false;
	if( strName.length() != 0 )
	{
		bNameOK = true;
	}
	//判断url是否OK
	bool bUrlOK = false;
	CString cstrUrl = CString( strUrl.c_str() );
	if( cstrUrl.Left(7).MakeLower() == "http://" ) //url必须是http:// 打头
	{
		bUrlOK = true;
	}	
	
	if( bIDOK && bNameOK && bUrlOK )	
	{
		TAB_ITEM tItem;
		tItem.eumType = TAB_WEBGAME;
		tItem.strName = strName;
		tItem.strParam = strContent;
		GLOBAL_TABBARDATA->ITabBar_ChangeTab(tItem);

		string s = "PlayWebGame:" + strID;
		LogRealMsg("PLAYGAME",s.c_str());
		return "res=ok\n\n";
	}	
	return "res=err\n\n";
}


string CWebManager::_command_opennewtab (string & strContent)
{
	string strID = GetValue (strContent, "id");
	string strName = GetValue (strContent, "name");
	string strUrl = GetValue (strContent, "infourl");

	string strParam;
	YL_StringUtil::Format (strParam, "param=%s", strUrl.c_str ());

	TAB_ITEM tItem;
	tItem.eumType = TAB_WEB;
	tItem.strName = strName;
	tItem.strParam = strParam;

	GLOBAL_TABBARDATA->ITabBar_ChangeTab(tItem);

	return "";
}

string CWebManager::_command_checkdlstatus (string &strContent)
{
	string strID = GetValue (strContent, "id");

	OneLocalGame olg;

	string strStatus  = 
		GLOBAL_LOCALGAME->ILocalGameData_GetGameByID (strID, olg) ? "run" : "download";

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
	pParam = NULL;

	return "";
}

string CWebManager::GetValue( string& strContent, string strKey )
{
	string strRes;
	CString str(strContent.c_str());
	CString strOneItem;
	int curPos = 0;
	strOneItem= str.Tokenize("\n",curPos);
	while(strOneItem != "")
	{		
		CString strLeft,strRight;
		_string_help( strOneItem,strLeft,strRight );
		if( strLeft.Compare(strKey.c_str()) == 0 )
		{
			strRes = strRight;
			break;
		}
		strOneItem = str.Tokenize("\n", curPos);
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
	{
		return;
	}
	strLeft		= strInput.Left(iPosEq);
	strRight	= strInput.Mid(iPosEq+1,strInput.GetLength()-iPosEq-1);
}

