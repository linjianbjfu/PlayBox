#include "stdafx.h"
#include "DownSubject.h"
#include "YL_StringUtil.h"
#include "YL_DirInfo.h"
#include "YL_FileInfo.h"
#include "YL_HTTPDownFile.h"
#include "YL_FileZip.h"
#include "./src/module/IrregularBorder/IrregularBorderMgr.h"


static	HWND	s_hMsgWnd = NULL;
static  string	s_strURL  = "";
static	string	s_strDesFile = "";
static	string  s_strSubName ="";
static BOOL s_bDownWebSkin = 0;
static HANDLE s_hWebSkin = NULL;
static YL_CHTTPDownFile* s_httpDownWebSub = NULL;
HANDLE s_Event = NULL;

DWORD WINAPI	DownSub_Thread(void* ppar)
{
	YL_CHTTPDownFile* httpDownFile = new YL_CHTTPDownFile;
	::PostMessage( s_hMsgWnd,MSG_DOWNSUBSTART,(WPARAM)httpDownFile,0);

	int iRetryTime = 0;
	while( iRetryTime < 3 )
	{	
		YL_StringUtil::Format(s_strDesFile,"%s%d",s_strDesFile.c_str(),GetTickCount());
		if( httpDownFile->DownloadFile( s_strURL,s_strDesFile) )
		{
			char szbuffer[512];
			CLhcImg::GetHomePath(szbuffer,512);
			strcat(szbuffer,"\\skin\\");

			vector<string> vec;
			if( YL_CFileZip::UnCompress( s_strDesFile.c_str(), szbuffer,vec,true) )
				::PostMessage( s_hMsgWnd,MSG_DOWNSUBCLOSE,1,0);
			else
				::PostMessage( s_hMsgWnd,MSG_DOWNSUBCLOSE,0,0);
			break;
		}
		iRetryTime++;
		delete httpDownFile;
		httpDownFile = new YL_CHTTPDownFile;
	}

	if( iRetryTime >= 3)
	{
		delete httpDownFile;
		::PostMessage( s_hMsgWnd,MSG_DOWNSUBCLOSE,0,0);
	}

	return 0;
}

DWORD WINAPI	DownWebSub_Thread(void* ppar)
{

	return 0;
}

void	CDownSub::ReqDownSub(const CString& strSub,HWND hMsgWnd )
{
	s_hMsgWnd = hMsgWnd;
	YL_StringUtil::Format( s_strURL,"http://down.kuwo.cn/mbox_data/theme_v267/%s.zip",strSub);	

	YL_DirInfo::GetTempDir( s_strDesFile );
	if( !PathFileExists( s_strDesFile.c_str() ) )
	{
		YL_DirInfo::MakeDir( s_strDesFile );
	}

	s_strDesFile += "\\" + strSub;

	CloseHandle( CreateThread(0,0,DownSub_Thread,0,0,0) );
}

void	CDownSub::ReqDownWebSub(const CString& strSubUrl,const CString& strSubId,HWND hMsgWnd )
{
	DWORD dwstatues = 0;

	s_hMsgWnd = hMsgWnd;
	s_strSubName = strSubId;
	string tempfilestr;
	s_strURL = strSubUrl;
	YL_DirInfo::GetTempDir( s_strDesFile );
	if( !PathFileExists( s_strDesFile.c_str() ) )
	{
		YL_DirInfo::MakeDir( s_strDesFile );
	}
	YL_StringUtil::Format(tempfilestr,"%s\\%s%d.ksf",s_strDesFile.c_str(),strSubId,GetTickCount());
	s_strDesFile = tempfilestr;
	if (s_Event == NULL)
	{
		s_Event = CreateEvent(NULL,true,TRUE,NULL);
	}
	s_hWebSkin = CreateThread(0,0,DownWebSub_Thread,0,0,0);
	
}

void	CDownSub::StopDownWebSubThread()
{
	DWORD dwstatues = 0;
	if(s_bDownWebSkin)//::WaitForSingleObject(s_hWebSkin, 100) ==WAIT_TIMEOUT)//s_bDownWebSkin)//&&
	{
		//等待解压完成，否则中断线程会出现问题
		WaitForSingleObject(s_Event,INFINITE);
		::TerminateThread(s_hWebSkin,dwstatues);
		CloseHandle(s_hWebSkin);
		s_bDownWebSkin = 0;
		s_hWebSkin = NULL;
	}
}

 bool	CDownSub::IsDownWebSub()
 {
	 return s_bDownWebSkin;
 }
