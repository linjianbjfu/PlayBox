#include "stdafx.h"
#include "HttpDownCtrl.h"
#include "YL_HTTPDownFile.h"
#include "YL_StringUtil.h"
#include "YL_FileInfo.h"
#include "YL_DirInfo.h"
#include "../../AppConfig/config/ConfigAppDef.h"
#include "../../AppConfig/config/ConfigParaDefault.h"

#define ID_WND_HTTP_DOWN_CTRL	2030
#define ELAPSE_DOWN				500
#define TIMER_CHECK_DOWN_STATE	WM_USER+203
HttpDownCtrl* HttpDownCtrl::m_pCtr = NULL;

BEGIN_MESSAGE_MAP(HttpDownCtrl, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()

HttpDownCtrl::HttpDownCtrl(void)
{
}

HttpDownCtrl::~HttpDownCtrl(void)
{
}

HttpDownCtrl* HttpDownCtrl::GetInstance()
{
	if( m_pCtr == NULL )
	{
		m_pCtr = new HttpDownCtrl;
		m_pCtr->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,
			CRect(0,0,0,0),AfxGetMainWindow(),ID_WND_HTTP_DOWN_CTRL );
		m_pCtr->ShowWindow( SW_HIDE );
	}
	return m_pCtr;
}

void HttpDownCtrl::DelInstance()
{
	if( m_pCtr != NULL )
	{
		m_pCtr->DestroyWindow();
		delete m_pCtr;
		m_pCtr = NULL;
	}
}

void HttpDownCtrl::StartDown( string strID, string strSwfUrl )
{
	//已在下载中
	CSingleLock sl( &m_cs );
	sl.Lock();
	if( m_mapDownThread.find( strID ) != m_mapDownThread.end() )
	{
		SetTimer( TIMER_CHECK_DOWN_STATE, ELAPSE_DOWN, NULL );
		return;
	}
	sl.Unlock();

	NotifyDownStart( strID );
	YL_CHTTPDownFile* pHttpDownFile = new YL_CHTTPDownFile();

	//将swf文件放入存放目录
	string strSwfSavePath;
	AfxGetUserConfig()->GetConfigStringValue( CONF_APP_MODULE_NAME, CONF_APP_SWF_PATH, strSwfSavePath );
	YL_DirInfo::MakeDir( strSwfSavePath ); //创建该目录
	if( !YL_FileInfo::IsDirectory( strSwfSavePath ) )
	{
		strSwfSavePath = ConfigParaDefault::GetDefaultAPP_SWF_PATH();
		YL_DirInfo::MakeDir( strSwfSavePath );
		AfxGetUserConfig()->SetConfigStringValue( CONF_APP_MODULE_NAME, CONF_APP_SWF_PATH, strSwfSavePath );
	}
	//开始下载
	string strFileName = strID + ".swf";
	string strLocalPath = YL_StringUtil::ComposeDirPath( strSwfSavePath, strFileName );
	
	DeleteFile( strLocalPath.c_str() );
	pHttpDownFile->AsyncDownloadFile( strSwfUrl, strLocalPath );
	pHttpDownFile->GetFileSize();

	sl.Lock();
	m_mapDownThread[strID] = pHttpDownFile;
	sl.Unlock();
	SetTimer( TIMER_CHECK_DOWN_STATE, ELAPSE_DOWN, NULL );
}

void HttpDownCtrl::StopDown( string strID )
{
	CSingleLock sl( &m_cs );
	sl.Lock();
	map<string,YL_CHTTPDownFile*>::iterator it1 = m_mapDownThread.find( strID );
	if( it1 != m_mapDownThread.end() )
	{
		delete it1->second;
		m_mapDownThread.erase( it1 );
	}
	sl.Unlock();

	NotifyDownFailed( strID, HTTP_FAILED_CANCELED );
}

void HttpDownCtrl::NotifyDownStart( string& strID )
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_HTTP_DOWN,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IHttpDownObserver* pOb = dynamic_cast<IHttpDownObserver*>(*itOb);
		pOb->HttpDownOb_DownStart( strID );
	}
}

void HttpDownCtrl::NotifyDownFinish( string& strID, string& strSwfPath )
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_HTTP_DOWN,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IHttpDownObserver* pOb = dynamic_cast<IHttpDownObserver*>(*itOb);
		pOb->HttpDownOb_DownFinish( strID, strSwfPath );
	}
}

void HttpDownCtrl::NotifyDownFailed( string& strID, HTTP_DOWN_FAILED_REASON r )
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_HTTP_DOWN,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IHttpDownObserver* pOb = dynamic_cast<IHttpDownObserver*>(*itOb);
		pOb->HttpDownOb_DownFailed( strID, r );
	}
}

void HttpDownCtrl::NotifyDownProgress( string& strID, double dPercent, 
									  unsigned int unFileSize, unsigned int unSpeed)
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_HTTP_DOWN,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IHttpDownObserver* pOb = dynamic_cast<IHttpDownObserver*>(*itOb);
		pOb->HttpDownOb_DownProgress( strID, dPercent, unFileSize, unSpeed );
	}
}

void HttpDownCtrl::OnTimer(UINT nIDEvent)
{
	if( nIDEvent == TIMER_CHECK_DOWN_STATE )
	{
		CSingleLock sl( &m_cs );
		sl.Lock();
		if( m_mapDownThread.size() == 0 )
			KillTimer( TIMER_CHECK_DOWN_STATE ); 

		vector<string> vecGidFinish; //要从任务中删除的gid
		//处理下载失败的
		
		for( map<string,YL_CHTTPDownFile*>::iterator it1 = m_mapDownThread.begin();
			it1 != m_mapDownThread.end(); it1++ )
		{
			string strGid = it1->first;
			YL_CHTTPDownFile* pHttpDownFile = it1->second;
			if( pHttpDownFile->IsDownloadFailed() )
			{
				//获取下载失败的原因
				NotifyDownFailed( strGid, pHttpDownFile->GetFailedReason() );
				vecGidFinish.push_back( strGid );
			}else
			{
				double dDownPercentage = pHttpDownFile->GetCurrentDownloadStatus();
				if( dDownPercentage < 1 )
				{
					NotifyDownProgress( strGid, dDownPercentage,
						pHttpDownFile->GetFileSize(), pHttpDownFile->GetSpeed()*1024 );
				}else
				{
					vecGidFinish.push_back( strGid );
					string strSwfPath = pHttpDownFile->GetParamLocalFile();
					NotifyDownFinish( strGid, strSwfPath );
				}				
			}
		}

		for( vector<string>::iterator it2 = vecGidFinish.begin();
			it2 != vecGidFinish.end(); it2++ )
		{
			map<string,YL_CHTTPDownFile*>::iterator it3 = m_mapDownThread.find( *it2 );
			if( it3 != m_mapDownThread.end() )
			{
				delete it3->second;
				m_mapDownThread.erase( it3 );
			}
		}
		sl.Unlock();
	}
	CWnd::OnTimer(nIDEvent);
}
