#include "stdafx.h"
#include "LocalGameData.h"
#include "YL_StringUtil.h"
#include "YL_DirInfo.h"
#include "YL_FileInfo.h"
#include "tools.h"
#include "../../util/MarkupArchive.h"
#include "../../DataInterface/IQQItemCountChangeObserver.h"
#include "../PlayedGamePanel/LMCStructs.h"

LocalGameData*	LocalGameData::m_pData = NULL;
#define MAX_TAB_COUNT	3

IData* LocalGameData::GetInstance()
{
	if(m_pData == NULL)
		m_pData = new LocalGameData();
	return m_pData;
}

void LocalGameData::DelInstance()
{
	if(m_pData != NULL)
		delete m_pData;
	m_pData = NULL;
}

void LocalGameData::DataAppStart()
{
	char szHomePath[MAX_PATH];
	CLhcImg::GetHomePath( szHomePath, MAX_PATH );

	YL_StringUtil::Format( m_strDataFilePath, "%s\\Resources\\Profiles\\usergame.xml", szHomePath );
	YL_Log( "LocalGameData.txt", LOG_DEBUG, "DataAppStart",	"m_strDataFilePath:%s", m_strDataFilePath.c_str() );

	LoadGameData();
}

void LocalGameData::DataAppExit()
{
	UnLoadGameData();
}

static void MergeXml (CString strMaster, CString strUpdate)
{
	CMarkupArchive xmlMaster;
	CMarkupArchive xmlUpdate;

	// safe check
	if (!xmlMaster.Open (strMaster.GetBuffer ()))
		return;
	if (!xmlUpdate.Open (strUpdate.GetBuffer ()))
		return;

	xmlMaster.ResetPos();
	xmlMaster.FindElem();
	xmlMaster.IntoElem();

	xmlUpdate.ResetPos ();
	xmlUpdate.FindElem ();
	xmlUpdate.IntoElem ();

	while (xmlUpdate.FindElem ())
	{
		CString strElemName = xmlUpdate.GetTagName ();
		xmlMaster.AddElem (strElemName);

		CString strID    = xmlUpdate.GetAttrib( "id" );
		CString strName  = xmlUpdate.GetAttrib( "name" );
		CString strIntro  = xmlUpdate.GetAttrib( "intro" );
		CString strPicPath = xmlUpdate.GetAttrib( "pic" );
		CString strGamePath = xmlUpdate.GetAttrib( "swf" );
		CString strLastPlayTime = xmlUpdate.GetAttrib( "lastplaytime" );
		CString strPlayCount = xmlUpdate.GetAttrib( "playcount" );
		CString strGameType = xmlUpdate.GetAttrib ("type");
		CString strMD5 = xmlUpdate.GetAttrib ("MD5");

		xmlMaster.SetAttrib ("id", strID);
		xmlMaster.SetAttrib ("name", strName);
		xmlMaster.SetAttrib ("intro", strIntro);
		xmlMaster.SetAttrib ("pic", strPicPath);
		xmlMaster.SetAttrib ("swf", strGamePath);
		xmlMaster.SetAttrib ("lastplaytime", strLastPlayTime);
		xmlMaster.SetAttrib ("playcount", strPlayCount);
		xmlMaster.SetAttrib ("type", strGameType);
		xmlMaster.SetAttrib ("MD5", strMD5);
	}

	xmlMaster.Save ();

	xmlMaster.Close ();
	xmlUpdate.Close ();
}


void LocalGameData::LoadGameData()
{
	m_vecGame.clear();
	YL_Log( "LocalGameData.txt", LOG_DEBUG, "LoadGameData","m_strDataFilePath:%s", m_strDataFilePath.c_str() );

	CMarkupArchive xml;
	if( !xml.Open(m_strDataFilePath.c_str()) )
		return;

	YL_Log( "LocalGameData.txt", LOG_DEBUG, "LoadGameData","CMarkupArchive::Open success" );

	char szHomePath[MAX_PATH];
	CLhcImg::GetHomePath( szHomePath, MAX_PATH );

	xml.ResetPos();
	xml.FindElem();
	xml.IntoElem();

	while( xml.FindChildElem() )
	{
		OneLocalGame olg;
		olg.strID    = xml.GetAttrib( "Id" );
		olg.strName  = xml.GetAttrib( "Name" );
		olg.strIntro  = xml.GetAttrib( "Intro" );
		//YL_Log( "LocalGameData.txt", LOG_DEBUG, "LoadGameData","id:%s|name:%s|intro:%s",
		//	olg.strID.c_str(), olg.strName.c_str(), olg.strIntro.c_str() );
		olg.strPicPath = xml.GetAttrib( "Img" );
		olg.strGamePath = xml.GetAttrib( "AppPath" );
		olg.strSrvID = xml.GetAttrib("srvid");
		olg.strAddTime = xml.GetAttrib("add_time");
		CString strGameType = xml.GetAttrib( _T("GameType") );
		olg.nGameType = atoi(strGameType.GetBuffer(0) );
		strGameType.ReleaseBuffer();

		if( YL_FileInfo::IsValid( olg.strGamePath ) )
		{
			YL_Log( "LocalGameData.txt", LOG_DEBUG, "LoadGameData","文件存在:%s",
				olg.strGamePath.c_str() );

			m_vecGame.push_back( olg );
		}
		//else if (olg.strGamePath.find("http://", 0))
		else if ( 0 == strnicmp(olg.strGamePath.c_str(), "http://", strlen("http://")) )
		{
			m_vecGame.push_back( olg );
		}
		else
		{
			YL_Log( "LocalGameData.txt", LOG_DEBUG, "LoadGameData","文件不存在:%s",
				olg.strGamePath.c_str() );
		}
	}
	xml.Close();
}

void LocalGameData::UnLoadGameData()
{
	string strTmp = m_strDataFilePath + ".1";
	string strContent = "<?xml version=\"1.0\" encoding=\"GBK\"?><root/>";

	string strFileDir;
	YL_FileInfo::GetFileDirPath( strTmp, strFileDir );
	YL_DirInfo::MakeDir( strFileDir );

	YL_FileInfo::SetFileContent( strTmp, strContent );
	
	CMarkupArchive xml;
	if( !xml.Open(strTmp.c_str()) )
		return;

	xml.ResetPos();
	xml.FindElem();

	for( LocalGameList::iterator it1 = m_vecGame.begin(); it1 != m_vecGame.end(); it1++ )
	{
		xml.AddChildElem( "app" );		
		xml.IntoElem();
		xml.SetAttrib( "Id", it1->strID.c_str() );
		xml.SetAttrib( "Name", it1->strName.c_str() );
		xml.SetAttrib( "Intro", it1->strIntro.c_str() );
		xml.SetAttrib( "Img", it1->strPicPath.c_str() );
		xml.SetAttrib( "AppPath", it1->strGamePath.c_str() );
		xml.SetAttrib( "srvid", it1->strSrvID.c_str());
		xml.SetAttrib( "add_time", it1->strAddTime.c_str());

		CString str;
		str.Format(_T("%d"), it1->nGameType);
		xml.SetAttrib( _T("GameType"), str.GetBuffer(0));
		str.ReleaseBuffer();
		xml.OutOfElem();
	}
	xml.Close();
	xml.Save();

	CopyFile( strTmp.c_str(), m_strDataFilePath.c_str(), FALSE );
}

bool LocalGameData::ILocalGameData_GetGameByID( string strID, int nGameType, OneLocalGame& og )
{
	LocalGameList::iterator it1 = m_vecGame.begin();
	for( ; it1 != m_vecGame.end(); it1 ++ )
	{
		if( strID == it1->strID
			&& nGameType == it1->nGameType)
		{
			break;
		}
	}
	if( it1 != m_vecGame.end() )
	{
		if (it1->nGameType == OneLocalGame::TYPE_WEB_GAME)
		{
			og = *it1;
			return true;
		}
		else if (it1->nGameType == OneLocalGame::TYPE_FLASH_GAME)
		{
			if( YL_FileInfo::IsValid( it1->strGamePath ) )
			{
				og = *it1;
				return true;
			}else
			{			
				m_vecGame.erase( it1 );
				og.clear();
				return false;
			}
		}
	}else
	{
		og.clear();
		return false;
	}
}

bool LocalGameData::ILocalGameData_AddGame( OneLocalGame og, string strMD5)
{
	//删除原有的信息
	LocalGameList::iterator it1 = m_vecGame.begin();
	for( ; it1 != m_vecGame.end(); it1 ++ )
	{
		if( og.strID == it1->strID
			&& og.nGameType == it1->nGameType)
		{
			break;
		}
	}
	if( it1 != m_vecGame.end() )
	{
		return false;
	}
	OneLocalGame olg = og;
	
	// 获取系统日期
	struct tm* tmLocal;
	time_t t = time(NULL);;
	tmLocal = localtime(&t);

	YL_StringUtil::Format(olg.strAddTime, "%d-%d-%d %d:%d",
		tmLocal->tm_year+1900, tmLocal->tm_mon+1, tmLocal->tm_mday, // date
		tmLocal->tm_hour, tmLocal->tm_min);// time
	
	YL_FileInfo::GetFileSize( olg.strGamePath.c_str(), &(olg.uiFileSize) );

	//加入本地游戏数据
	m_vecGame.push_back( olg );
	NotifyQQItemCountChange();
	return true;
}

bool LocalGameData::ILocalGameData_DelGame( string strID, int nGameType )
{
	LocalGameList::iterator it1 = m_vecGame.begin();
	for( ; it1 != m_vecGame.end(); it1 ++ )
	{
		if( strID == it1->strID 
			&& nGameType == it1->nGameType)
		{
			break;
		}
	}
	if( it1 != m_vecGame.end() )
	{
		//删除图片
		::DeleteFile( it1->strPicPath.c_str() );
		//删除swf文件
		::DeleteFile( it1->strGamePath.c_str() );

		m_vecGame.erase( it1 );
		NotifyQQItemCountChange();
		return true;
	}else
	{
		return false;
	}	
}

bool LocalGameData::ILocalGameData_GetAllGame( LocalGameList& lgl )
{
	lgl.clear();
	copy( m_vecGame.begin(), m_vecGame.end(), back_inserter(lgl) );
	return true;
}

unsigned int LocalGameData::ILocalGameData_GetGameCount()
{
	return (unsigned int)(m_vecGame.size());
}

void LocalGameData::NotifyQQItemCountChange()
{
//  	list<IMessageObserver*> listOb;
//  	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_QQ_ITEM_CHANGE,listOb);
//  
//  	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
//  	{
//  		IQQItemCountChangeObserver* pOb = dynamic_cast<IQQItemCountChangeObserver*>(*itOb);
//  		pOb->IQQItemChangeOb_CountChange( m_vecGame.size() );
//  	}
}

int LocalGameData::ILocalGameData_GetWebGame(LocalGameList& lgl)
{
	lgl.clear();
	for (size_t i=0; i<m_vecGame.size(); i++)
	{
		if (m_vecGame[i].nGameType == OneLocalGame::TYPE_WEB_GAME)
		{
			lgl.push_back(m_vecGame[i]);
		}
	}
	return true;
}

int LocalGameData::ILocalGameData_GetFlashGame(LocalGameList& lgl)
{
	lgl.clear();
	for (size_t i=0; i<m_vecGame.size(); i++)
	{
		if (m_vecGame[i].nGameType == OneLocalGame::TYPE_FLASH_GAME)
		{
			lgl.push_back(m_vecGame[i]);
		}
	}
	return true;
}