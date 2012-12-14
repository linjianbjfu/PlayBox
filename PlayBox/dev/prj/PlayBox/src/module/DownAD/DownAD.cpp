#include "StdAfx.h"
#include "downad.h"
#include "src\module\HTTPControl\HTTPControl.h"
#include "YL_HTTPRequest.h"
#include "YL_FileZip.h"
#include "YL_StringUtil.h"
#include "YL_FileInfo.h"
#include "IAdObserver.h"

CDownAD* CDownAD::m_pDownAD = NULL;

CDownAD::CDownAD(void)
{
	m_iCurAd = 0;
	m_vecAD.clear();
	ReloadAd();
}

CDownAD::~CDownAD(void)
{
}

CDownAD* CDownAD::GetInstance()
{
	if( m_pDownAD == NULL )
	{
		m_pDownAD = new CDownAD();
		m_pDownAD->m_bGetAdData = false;
	}
	return m_pDownAD;
}

static DWORD WINAPI Thread_DownZip(void* ppar )
{
	if( CLhcImg::CheckUpdateADData() )
	{
		YL_Log( "qq.txt",LOG_NOTICE,"CAdData","需要进行ZipRes更新");
		CHTTPControl::HTTPWait_AD();

		if( CDownAD::m_pDownAD != NULL )
		{
			CDownAD::m_pDownAD->m_bGetAdData = TRUE;
			YL_CHTTPRequest http;
			string strSvr;
			if( !CLhcImg::GetAdDataServer( strSvr ) )
			{
				strSvr = "";
			}

			if( http.Request( strSvr.c_str() ,YL_CHTTPRequest::REQUEST_GET,30*1000 ) )
			{
				YL_Log( "log_httpr.txt",LOG_DEBUG,"Ad--HTTP","下载完成");

				long		   lConLen = 0;
				unsigned char* pContent = http.GetContent(lConLen);

				CDownAD::m_pDownAD->DownZipFinish( pContent,lConLen );
			}
 			else
 			{
				CDownAD::m_pDownAD->m_bGetAdData = FALSE;
 				YL_Log( "log_httpr.txt",LOG_NOTICE,"CAdData","获取ZipRes Sign == 失败");
 			}
 			CDownAD::m_pDownAD->ZipResReady( CDownAD::m_pDownAD->m_bGetAdData );
		}
		YL_Log( "log_httpr.txt",LOG_NOTICE,"CAdData","更新广告--结束");
 	}
 	CHTTPControl::HTTPFinish_DownAD();
	return 0;
}

void CDownAD::DataAppStart()
{
	YL_Log( STR_LOG_FILE,LOG_NOTICE,"CDownAD","DataAppStart");
	// 创建下载榜单的线程
	m_hThreadDown = CreateThread(0,0,Thread_DownZip,0,0,0);
	YL_Log( STR_LOG_FILE,LOG_NOTICE,"CDownAD","DataAppStart--OUT");
}

void CDownAD::DataAppExit()
{
	YL_Log( STR_LOG_FILE,LOG_NOTICE,"CDownAD","DataAppExit");
	DWORD dwExitCode = 0;

	if( GetExitCodeThread( m_hThreadDown,&dwExitCode) && dwExitCode == STILL_ACTIVE )
	{
		TerminateThread(m_hThreadDown,1);
		WaitForSingleObject( m_hThreadDown,2000);
	}
	CloseHandle( m_hThreadDown );
	delete this;
	YL_Log( STR_LOG_FILE,LOG_NOTICE,"CDownAD","DataAppExit-- out");
}

void CDownAD::ZipResReady( BOOL bDownSuccess )
{
	if(bDownSuccess)
	{
		NotifyZipSuc();
	}else
	{
		NotifyZipFailed();
	}
}

void CDownAD::NotifyZipFailed()
{
	m_hThreadDown = CreateThread(0,0,Thread_DownZip,0,0,0);
}

void CDownAD::NotifyZipSuc()
{
	ReloadAd();

	list<IMessageObserver*> listOb;
	AfxGetMessageManager()->QueryObservers( ID_MESSAGE_AD,listOb);

	for( list<IMessageObserver*>::iterator itOb = listOb.begin();itOb != listOb.end();itOb++ )
	{
		IAdObserver* pOb = dynamic_cast<IAdObserver*>(*itOb);
		pOb->AdOb_UpdataSuc();
	}
	CLhcImg::SetUpdateADData();
}

void CDownAD::NotifyZipStart()
{
}

void CDownAD::DownZipFinish( void* pVoid,unsigned int unContentLen )
{
	char szBuffer[512];
	if( !CLhcImg::GetAdPath( szBuffer,512 ) )
	{
		YL_Log( "ad.txt",LOG_NOTICE,"DownZipFinish","GetAd Path Failed" );
		return;
	}

	vector<string> vecStrPath;
	if( !YL_CFileZip::UnCompress( pVoid,unContentLen,string(szBuffer),vecStrPath,true) )
	{
		YL_Log( "qq.txt",LOG_NOTICE,"DownZipFinish","res 解压缩失败!!!!!!" );
		return;
	}
}

void CDownAD::ReloadAd()
{
	m_iCurAd = 0;
	m_vecAD.clear();
	//读取配置文件，获取所有广告的信息
	char szBuffer[512];
	memset( szBuffer, 0, 512 );
	if( !CLhcImg::GetAdPath( szBuffer,512 ) )
	{
		return;
	}
	string strAdPath = string( szBuffer );
	strcat( szBuffer, "AdLoadingPane.ini" );

	YL_Ini ini (szBuffer);
	const char * szSection = "AdLoadingPane";
	int nAdNum = ini.GetInt (szSection, "AdLoadingPaneNum", 0);

	char szTmpBuf [1024] = {0};
	DWORD dwTmpRet = 0;
	string strTmpKey;
	for (int i = 0; i < nAdNum; i++)
	{
		AD_STRUCT ad;
		//得到strLinkUrl
		memset (szTmpBuf, 0, sizeof (char) * 1024);
		YL_StringUtil::Format (strTmpKey, "LoadingPaneLink%d", i);
		dwTmpRet = ini.GetString (szSection, strTmpKey.c_str (), szTmpBuf, 1024 - 1);
		if (dwTmpRet == 0 || strlen(szTmpBuf) == 0)
			continue;
		ad.strLinkUrl = szTmpBuf;
		//得到strPicPath
		memset (szTmpBuf, 0, sizeof (char) * 1024);
		YL_StringUtil::Format (strTmpKey, "LoadingPanePic%d", i);
		dwTmpRet = ini.GetString (szSection, strTmpKey.c_str (), szTmpBuf, 1024 - 1);
		if (dwTmpRet == 0 || strlen(szTmpBuf) == 0)
			continue;
		ad.strPicPath = strAdPath + szTmpBuf;

		if( ad.strLinkUrl.length() > 0 && YL_FileInfo::IsValid( ad.strPicPath ) )
		{
			m_vecAD.push_back(ad);
		}
	}
}

bool CDownAD::GetLoadingPicAd( AD_STRUCT& ad )
{
	if( m_vecAD.size() == 0 )
		return false;

	m_iCurAd++;
	if( m_iCurAd >= m_vecAD.size() )
	{
		m_iCurAd = 0;
	} 
	ad = m_vecAD[m_iCurAd];
	return true;
}