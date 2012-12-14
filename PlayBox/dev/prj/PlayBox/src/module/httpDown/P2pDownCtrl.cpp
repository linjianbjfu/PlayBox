#include "stdafx.h"
#include "P2pDownCtrl.h"
#include "YL_HTTPDownFile.h"
#include "YL_StringUtil.h"
#include "YL_FileInfo.h"
#include "YL_DirInfo.h"
#include "../../AppConfig/config/ConfigAppDef.h"
#include "../../AppConfig/config/ConfigParaDefault.h"

#define ID_WND_HTTP_DOWN_CTRL	2030
#define ELAPSE_DOWN				500
#define TIMER_CHECK_DOWN_STATE	WM_USER+203
P2pDownCtrl* P2pDownCtrl::m_pCtr = NULL;

BEGIN_MESSAGE_MAP(P2pDownCtrl, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()

P2pDownCtrl::P2pDownCtrl(void)
{
}

P2pDownCtrl::~P2pDownCtrl(void)
{
}

P2pDownCtrl* P2pDownCtrl::GetInstance()
{
	if( m_pCtr == NULL )
	{
		m_pCtr = new P2pDownCtrl;
		m_pCtr->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,
			CRect(0,0,0,0),AfxGetMainWindow(),ID_WND_HTTP_DOWN_CTRL );
		m_pCtr->ShowWindow( SW_HIDE );
	}
	return m_pCtr;
}

void P2pDownCtrl::DelInstance()
{
	if( m_pCtr != NULL )
	{
		m_pCtr->DestroyWindow();
		delete m_pCtr;
		m_pCtr = NULL;
	}
}

void P2pDownCtrl::StartDown( string strID, string strSwfUrl )
{
	//����������
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

	//��swf�ļ�������Ŀ¼
	string strSwfSavePath;
	AfxGetUserConfig()->GetConfigStringValue( CONF_APP_MODULE_NAME, CONF_APP_SWF_PATH, strSwfSavePath );
	YL_DirInfo::MakeDir( strSwfSavePath ); //������Ŀ¼
	if( !YL_FileInfo::IsDirectory( strSwfSavePath ) )
	{
		strSwfSavePath = ConfigParaDefault::GetDefaultAPP_SWF_PATH();
		YL_DirInfo::MakeDir( strSwfSavePath );
		AfxGetUserConfig()->SetConfigStringValue( CONF_APP_MODULE_NAME, CONF_APP_SWF_PATH, strSwfSavePath );
	}
	//��ʼ����
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

void P2pDownCtrl::StopDown( string strID )
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

void P2pDownCtrl::NotifyDownStart( string& strID )
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_HTTP_DOWN,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IHttpDownObserver* pOb = dynamic_cast<IHttpDownObserver*>(*itOb);
		pOb->HttpDownOb_DownStart( strID );
	}
}

void P2pDownCtrl::NotifyDownFinish( string& strID, string& strSwfPath )
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_HTTP_DOWN,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IHttpDownObserver* pOb = dynamic_cast<IHttpDownObserver*>(*itOb);
		pOb->HttpDownOb_DownFinish( strID, strSwfPath );
	}
}

void P2pDownCtrl::NotifyDownFailed( string& strID, HTTP_DOWN_FAILED_REASON r )
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_HTTP_DOWN,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IHttpDownObserver* pOb = dynamic_cast<IHttpDownObserver*>(*itOb);
		pOb->HttpDownOb_DownFailed( strID, r );
	}
}

void P2pDownCtrl::NotifyDownProgress( string& strID, double dPercent, 
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

void P2pDownCtrl::OnTimer(UINT nIDEvent)
{
	if( nIDEvent == TIMER_CHECK_DOWN_STATE )
	{
		CSingleLock sl( &m_cs );
		sl.Lock();
		if( m_mapDownThread.size() == 0 )
			KillTimer( TIMER_CHECK_DOWN_STATE ); 

		vector<string> vecGidFinish; //Ҫ��������ɾ����gid
		//��������ʧ�ܵ�
		
		for( map<string,YL_CHTTPDownFile*>::iterator it1 = m_mapDownThread.begin();
			it1 != m_mapDownThread.end(); it1++ )
		{
			string strGid = it1->first;
			YL_CHTTPDownFile* pHttpDownFile = it1->second;
			if( pHttpDownFile->IsDownloadFailed() )
			{
				//��ȡ����ʧ�ܵ�ԭ��
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
					string strSwfPath = pHttpDownFile->GetParamLocalFile();
					NotifyDownFinish( strGid, strSwfPath );
					vecGidFinish.push_back( strGid );
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

void P2pDownCtrl::StartKWMV(  const char* pszKwMVPath, const char* pszAppName )
{}

//�˳�KwMV
void P2pDownCtrl::CloseKWMV()
{}

//ע�����P2P��Ϣ����
void P2pDownCtrl::AttachP2PListener(const P2PObserver* pP2PGObserver)
{}

//��ע����P2P��Ϣ����
void P2pDownCtrl::DetachP2PListener(const P2PObserver* pP2PGObserver)
{}

//����������Դ
bool P2pDownCtrl::StartDown(const string& strID, const P2PObserver* pP2PObserver, bool bContinueDown)
{
	return false;
}

//ֹͣ������Դ
void P2pDownCtrl::StopDown(const string& strID, P2PObserver* pP2PObserver)
{}

//ɾ��������Դ
void P2pDownCtrl::DelRes(const string& strID, P2PObserver* pP2PObserver)
{}

//��ȡ��Դ��Ϣ
void P2pDownCtrl::GetResInfo(const string& strID, unsigned int& uiRate, string& strFilePath)
{
}

//ֹͣ����
void P2pDownCtrl::StopAll(void)
{
}

//ע��ϵͳ�㲥��Ϣ���մ���
void P2pDownCtrl::SetSysMsgWnd(HWND hWnd)
{
}