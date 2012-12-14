#include "stdafx.h"
#include "PcGameDownCtrl.h"
#include "YL_HTTPDownFile.h"
#include "YL_StringUtil.h"
#include "YL_FileInfo.h"
#include "YL_DirInfo.h"
#include "../../AppConfig/config/ConfigAppDef.h"
#include "../../AppConfig/config/ConfigParaDefault.h"
#include "../../datainterface/IDownData.h"
#include "../../core/CDataManager.h"
#include <math.h>

#define ID_WND_HTTP_DOWN_CTRL	2030
#define ELAPSE_DOWN				500
#define TIMER_CHECK_DOWN_STATE	WM_USER+203
PcGameDownCtrl* PcGameDownCtrl::m_pCtr = NULL;

struct DecompressThreadParam
{
	string strSrc;
	string strDes;
	PcGameDownCtrl * pDownCtrl;
	HANDLE hEvent;

	DecompressThreadParam (){
		strSrc = string ("");
		strDes = string ("");
		pDownCtrl = NULL;
		hEvent = NULL;
	}
};

BEGIN_MESSAGE_MAP(PcGameDownCtrl, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()

PcGameDownCtrl::PcGameDownCtrl(void)
{
// 	m_curEventIndex = 0;
// 	memset (m_eDecompress, 0, sizeof (HANDLE) * MAX_PATH);
// 
// 	for (int i = 0; i < MAX_PATH; i++)
// 		m_eDecompress[i] = CreateEvent (NULL, TRUE, FALSE, NULL);
}

PcGameDownCtrl::~PcGameDownCtrl(void)
{
// 	for (int i = 0; i < MAX_PATH; i++)
// 	{
// 		CloseHandle (m_eDecompress[i]);
// 		m_eDecompress[i] = NULL;
// 	}
}

PcGameDownCtrl* PcGameDownCtrl::GetInstance()
{
	if( m_pCtr == NULL )
	{
		m_pCtr = new PcGameDownCtrl;
		m_pCtr->Create(NULL,NULL,WS_CHILD|WS_CLIPCHILDREN,
			CRect(0,0,0,0),AfxGetMainWindow(),ID_WND_HTTP_DOWN_CTRL );
		m_pCtr->ShowWindow( SW_HIDE );
	}
	return m_pCtr;
}

void PcGameDownCtrl::DelInstance()
{
	if( m_pCtr != NULL )
	{
		m_pCtr->DestroyWindow();
		delete m_pCtr;
		m_pCtr = NULL;
	}
}

void PcGameDownCtrl::StartDown( string strID)
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

	const ExeDownInfo * _exeInfo = GLOBAL_DOWNDATA->GetDownInfo (strID);

	string strUrl, strLocalPath;
	if (_exeInfo->dsZip1.dnc != DNC_FINISHED)
	{
		strUrl = _exeInfo->gameInfo.strUrlZip1;
		strLocalPath = YL_StringUtil::ComposeDirPath (_exeInfo->gameInfo.strDownPath, "zip1");
	}
	else if (_exeInfo->dsZip2.dnc != DNC_FINISHED)
	{
		strUrl = _exeInfo->gameInfo.strUrlZip2;
		strLocalPath = YL_StringUtil::ComposeDirPath (_exeInfo->gameInfo.strDownPath, "zip2");
	}
	pHttpDownFile->AsyncDownloadFile (strUrl, strLocalPath);
/*
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
*/
// 	pHttpDownFile->AsyncDownloadFile( strSwfUrl, strLocalPath );
	pHttpDownFile->GetFileSize();

	sl.Lock();
	m_mapDownThread[strID] = pHttpDownFile;
	sl.Unlock();
	SetTimer( TIMER_CHECK_DOWN_STATE, ELAPSE_DOWN, NULL );
}

void PcGameDownCtrl::StopDown( string strID )
{
	CSingleLock sl( &m_cs );

	sl.Lock();
	map<string,YL_CHTTPDownFile*>::iterator it1 = m_mapDownThread.find( strID );
	if( it1 != m_mapDownThread.end() )
	{
		YL_CHTTPDownFile * pDownFile = it1->second;
		pDownFile->Disconnect ();
// 		delete it1->second;
		m_mapDownThread.erase( it1 );
	}
	sl.Unlock();

	NotifyDownFailed( strID, HTTP_FAILED_CANCELED );
}

void PcGameDownCtrl::NotifyDownStart( string& strID )
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_EXE_HTTP_DOWN,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IHttpExeDownObserver* pOb = dynamic_cast<IHttpExeDownObserver*>(*itOb);
		pOb->HttpExeDownOb_DownStart( strID );
	}
}

void PcGameDownCtrl::NotifyDownFinish( string& strID)
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_EXE_HTTP_DOWN,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IHttpExeDownObserver* pOb = dynamic_cast<IHttpExeDownObserver*>(*itOb);
		pOb->HttpExeDownOb_DownFinish( strID);
	}
}

void PcGameDownCtrl::NotifyDownQuickPlay (string &strID)
{
	list <IMessageObserver*> listOb;
	AfxGetMessageManager ()->QueryObservers (ID_MESSAGE_EXE_HTTP_DOWN, listOb);

	for (list<IMessageObserver*>::iterator itOb = listOb.begin (); itOb != listOb.end (); itOb++)
	{
		IHttpExeDownObserver * pOb = dynamic_cast<IHttpExeDownObserver*>(*itOb);
		pOb->HttpExeDownOb_DownQuickPlay (strID);
	}
}

void PcGameDownCtrl::NotifyDownFailed( string& strID, HTTP_DOWN_FAILED_REASON r )
{

	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_EXE_HTTP_DOWN,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IHttpExeDownObserver* pOb = dynamic_cast<IHttpExeDownObserver*>(*itOb);
		pOb->HttpExeDownOb_DownFailed( strID, r );
	}
}

void PcGameDownCtrl::NotifyDownProgress( string& strID, double dPercent, 
									  unsigned int unFileSize, unsigned int unSpeed)
{
	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_EXE_HTTP_DOWN,listOb);

GLOBAL_DOWNDATA->LockDownInfoList ();
	const ExeDownInfo * pInfo = GLOBAL_DOWNDATA->GetDownInfo (strID);
GLOBAL_DOWNDATA->UnlockDownInfoList ();

	UINT uTotalSize = (UINT) atoi (pInfo->gameInfo.strSize.c_str ());

	UINT uFirstSize = 0;
	if (pInfo->gameInfo.uiStartPoint == 0)
		uFirstSize = (UINT) atoi (pInfo->gameInfo.strSize.c_str ());
	else
		uFirstSize = (UINT) atoi (pInfo->gameInfo.strFirstSize.c_str ());

	UINT uSecondSize= uTotalSize - uFirstSize;

	bool bQuickPlay = false;

	if (dPercent < 0)
		dPercent = 0; //= fabs (dPercent);

	if (pInfo->dsZip1.dnc == DNC_FINISHED)
	{
		dPercent = (dPercent * uSecondSize + uFirstSize) / (double) uTotalSize;
		unFileSize += uFirstSize;
		bQuickPlay = true;
	}
	else
	{
		dPercent = (dPercent * uFirstSize) / (double) uTotalSize;
	}

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IHttpExeDownObserver* pOb = dynamic_cast<IHttpExeDownObserver*>(*itOb);
		pOb->HttpExeDownOb_DownProgress( strID, dPercent, unFileSize, unSpeed, bQuickPlay );
	}
}

static DWORD WINAPI Decompress (LPVOID lpParam)
{
	DecompressThreadParam * pParam = (DecompressThreadParam*)lpParam;

	string str7z;
	char szBuffer[512] = {0};
	CLhcImg::GetHomePath (szBuffer,512);
	str7z = szBuffer;
	str7z += "\\7zr.exe";

	string str7zParam;
	str7zParam = "x \"" + pParam->strSrc + "\"" + " -o" + "\"" + pParam->strDes + "\"" + " -r -y";

// 	SetCurrentDirectory (pParam->strDes.c_str ());

	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = str7z.c_str ();
	ShExecInfo.lpParameters = str7zParam.c_str ();
	ShExecInfo.lpDirectory = "";//pParam->strDes.c_str ();
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;

	ShellExecuteEx(&ShExecInfo);

	DWORD dwRet= WaitForSingleObject(ShExecInfo.hProcess, INFINITE);

	if (dwRet == WAIT_OBJECT_0)
	{
		SetEvent (pParam->hEvent);
	}

	delete pParam;
	pParam = NULL;

	return 0;
}

static void WriteExePath (string & strExePath)
{
	string strIni;
	string strTmp = strExePath;
	strIni = strTmp.substr (0, strTmp.find ("downCache")) + "mcyyouxi.ini";

	YL_Ini ini(strIni.c_str ());
	char szExeName[MAX_PATH] = {0};
	ini.GetString ("main", "filename", szExeName, MAX_PATH);

	strExePath = strTmp.substr (0, strTmp.find ("downCache")) + "" + szExeName;
}

void PcGameDownCtrl::OnTimer(UINT nIDEvent)
{
/*
	DWORD dwRet = WaitForMultipleObjects (MAX_PATH, m_eDecompress, FALSE, 0);
	if (dwRet != WAIT_TIMEOUT && dwRet < MAX_PATH)
	{
		DWORD dwIndex = dwRet - WAIT_OBJECT_0;

		map<DWORD, pair<string, DecompressOperator> >::iterator itDecompress = m_DecompressCache.find (dwIndex);
		if (itDecompress != m_DecompressCache.end ())
		{
			string strGameID = itDecompress->second.first;
			GLOBAL_DOWNDATA->LockDownInfoList ();
			ExeDownInfo * pInfo = const_cast<ExeDownInfo*> (GLOBAL_DOWNDATA->GetDownInfo (strGameID));
			GLOBAL_DOWNDATA->UnlockDownInfoList ();

			switch (itDecompress->second.second)
			{
			case DO_QUICKPLAY:
				// 填充游戏的可执行文件的路径
				if (pInfo != NULL)
				{
					pInfo->gameInfo.strExePath = pInfo->gameInfo.strDownPath;
					WriteExePath (pInfo->gameInfo.strExePath);
				}
				NotifyDownQuickPlay (strGameID);
				break;
			case DO_FINISH:
				// 填充游戏的可执行文件的路径
				if (pInfo != NULL)
				{
					pInfo->gameInfo.strExePath = pInfo->gameInfo.strDownPath;
					WriteExePath (pInfo->gameInfo.strExePath);
				}
				NotifyDownFinish (strGameID);
				CSingleLock sl( &m_cs );
				sl.Lock ();
				map<string,YL_CHTTPDownFile*>::iterator it3 = m_mapDownThread.find (strGameID);
				if( it3 != m_mapDownThread.end() )
				{
					delete it3->second;
					m_mapDownThread.erase( it3 );
				}
				sl.Unlock ();
				break;
			}

			m_DecompressCache.erase (itDecompress);
			ResetEvent (m_eDecompress[dwIndex]);
		}
	}
*/
	if( nIDEvent == TIMER_CHECK_DOWN_STATE )
	{
		CSingleLock sl( &m_cs );
		sl.Lock();
		if( m_mapDownThread.size() == 0 )
			KillTimer( TIMER_CHECK_DOWN_STATE ); 

		vector<string> vecGidFinish; //要从任务中删除的gid
		vector<string> vecStartDownSecPart;

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
						pHttpDownFile->GetFileSize(), pHttpDownFile->GetSpeed());
				}else
				{
GLOBAL_DOWNDATA->LockDownInfoList ();
					ExeDownInfo * pInfo = const_cast<ExeDownInfo*> (GLOBAL_DOWNDATA->GetDownInfo (strGid));
GLOBAL_DOWNDATA->UnlockDownInfoList ();

					if (pInfo == NULL)
						continue;

					if (pInfo->dnc == DNC_DECOMPRESS)
					{
						DWORD dwRet = WaitForSingleObject (pInfo->hEventDecompress, 0);
						if (dwRet == WAIT_OBJECT_0)
						{
							if (pInfo->gameInfo.uiStartPoint == 0)
							{
								pInfo->dnc = DNC_FINISHED;
								pInfo->dsZip1.dnc = DNC_FINISHED;
							}
							else
							{
								if (pInfo->dsZip1.dnc != DNC_FINISHED)
								{
									pInfo->dsZip1.dnc = DNC_FINISHED;
									pInfo->dnc = DNC_DOWNING;

									pInfo->gameInfo.strExePath = pInfo->gameInfo.strDownPath;
									WriteExePath (pInfo->gameInfo.strExePath);
									NotifyDownQuickPlay (strGid);

									vecStartDownSecPart.push_back (strGid);
									continue;
								}
								else
								{
									pInfo->dnc = DNC_FINISHED;
									pInfo->dsZip1.dnc = DNC_FINISHED;
									pInfo->dsZip2.dnc = DNC_FINISHED;
								}
							}
						}
						else
						{
							NotifyDownCompress (strGid);
							continue;
						}
					}

					if (pInfo->dnc == DNC_FINISHED)
					{
						pInfo->gameInfo.strExePath = pInfo->gameInfo.strDownPath;
						WriteExePath (pInfo->gameInfo.strExePath);
						NotifyDownFinish (strGid);

						vecGidFinish.push_back (strGid);
					}
					else
					{
// 						if (pInfo->dsZip1.dnc != DNC_FINISHED)
// 						{
							pInfo->dnc = DNC_DECOMPRESS;

							DWORD   dwThreadID;	
							DecompressThreadParam * param = new DecompressThreadParam ();

							if (pInfo->dsZip1.dnc != DNC_FINISHED)
								param->strSrc = pInfo->gameInfo.strDownPath + "\\zip1";
							else
								param->strSrc = pInfo->gameInfo.strDownPath + "\\zip2";

							string::size_type pos = pInfo->gameInfo.strDownPath.find ("downCache");
							if (pos != string::npos)
							{
								param->strDes = pInfo->gameInfo.strDownPath.substr (0, pos);
								param->pDownCtrl = this;

								pInfo->hEventDecompress = CreateEvent (NULL, TRUE, FALSE, NULL);
								param->hEvent = pInfo->hEventDecompress;

								CloseHandle (CreateThread(NULL, 0,
									(LPTHREAD_START_ROUTINE)Decompress, (LPVOID)param, 0, &dwThreadID));
							}
// 						}
// 						else
// 						{
// 						}
					}
				}
			}
		}
		
		vector<string>::iterator itFinish = vecGidFinish.begin ();
		for (; itFinish != vecGidFinish.end (); itFinish++)
		{
			map<string,YL_CHTTPDownFile*>::iterator it = m_mapDownThread.find (*itFinish);
			if( it != m_mapDownThread.end() )
			{
				delete it->second;
				m_mapDownThread.erase (it);
			}
		}

		sl.Unlock();

		for (vector<string>::iterator itSecPart = vecStartDownSecPart.begin (); 
			itSecPart != vecStartDownSecPart.end (); itSecPart++)
		{
			sl.Lock ();
			map<string,YL_CHTTPDownFile*>::iterator itTmp = m_mapDownThread.find (*itSecPart);
			if( itTmp != m_mapDownThread.end() )
			{
				delete itTmp->second;
				m_mapDownThread.erase( itTmp );
			}
			sl.Unlock ();

			StartDown (*itSecPart);
		}
	}

	CWnd::OnTimer(nIDEvent);
}

void PcGameDownCtrl::NotifyDownCompress (string & strID)
{
	list <IMessageObserver*> listOb;
	AfxGetMessageManager ()->QueryObservers (ID_MESSAGE_EXE_HTTP_DOWN, listOb);

	for (list<IMessageObserver*>::iterator itOb = listOb.begin (); itOb != listOb.end (); itOb++)
	{
		IHttpExeDownObserver * pOb = dynamic_cast<IHttpExeDownObserver*>(*itOb);
		pOb->HttpExeDownOb_DownDecompress (strID);
	}
}
