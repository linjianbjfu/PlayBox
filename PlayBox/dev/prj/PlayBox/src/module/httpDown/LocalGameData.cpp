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
		
		CString strGameType = xml.GetAttrib( _T("GameType") );
		olg.nGameType = atoi(strGameType.GetBuffer(0) );
		strGameType.ReleaseBuffer();

		if( YL_FileInfo::IsValid( olg.strGamePath ) )
		{
			YL_Log( "LocalGameData.txt", LOG_DEBUG, "LoadGameData","文件存在:%s",
				olg.strGamePath.c_str() );

			m_vecGame.push_back( olg );
		}else
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

bool LocalGameData::ILocalGameData_GetGameByID( string strID, OneLocalGame& og )
{
	LocalGameList::iterator it1 = m_vecGame.begin();
	for( ; it1 != m_vecGame.end(); it1 ++ )
	{
		if( strID == it1->strID)
		{
			break;
		}
	}
	if( it1 != m_vecGame.end() )
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
	}else
	{
		og.clear();
		return false;
	}
}

bool LocalGameData::ILocalGameData_AddGame( string strID, string strName, string strPicPath, string strSwfPath,
										   string strIntro, unsigned int type, int nGameType, string strMD5)
{
	//删除原有的信息
	LocalGameList::iterator it1 = m_vecGame.begin();
	for( ; it1 != m_vecGame.end(); it1 ++ )
	{
		if( strID == it1->strID)
		{
			break;
		}
	}
	if( it1 != m_vecGame.end() )
	{
		return false;
	}
	//加入本地游戏数据
	OneLocalGame olg;
	olg.strID = strID;
	olg.strName = strName;
	olg.strGamePath = strSwfPath;
	olg.strIntro = strIntro;
	olg.nGameType = nGameType;
	YL_FileInfo::GetFileSize( olg.strGamePath.c_str(), &(olg.uiFileSize) );
	olg.strPicPath  = strPicPath;	//获取图片

	m_vecGame.push_back( olg );
	NotifyQQItemCountChange();
	return true;
}

bool LocalGameData::ILocalGameData_DelGame( string strID )
{
	LocalGameList::iterator it1 = m_vecGame.begin();
	for( ; it1 != m_vecGame.end(); it1 ++ )
	{
		if( strID == it1->strID )
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
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_QQ_ITEM_CHANGE,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IQQItemCountChangeObserver* pOb = dynamic_cast<IQQItemCountChangeObserver*>(*itOb);
		pOb->IQQItemChangeOb_CountChange( m_vecGame.size() );
	}
}
