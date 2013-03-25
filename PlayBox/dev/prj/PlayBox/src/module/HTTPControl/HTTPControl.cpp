#include "stdafx.h"
#include "HTTPControl.h"

//œ¬‘ÿÀ≥–Ú
static  HANDLE	s_EventInit;
static	HANDLE	s_EventTodayRecommend;
static	HANDLE  s_EventRegID;
static	HANDLE	s_EventConfigFile;
static	HANDLE  s_EventBangDan;
static	HANDLE	s_EventUpClientLog;
static	HANDLE	s_EventDownAD;

void CHTTPControl::Init()
{
	s_EventInit				= CreateEvent(NULL,TRUE,TRUE,NULL);
	s_EventTodayRecommend	= CreateEvent(NULL,TRUE,FALSE,NULL);
	s_EventRegID			= CreateEvent(NULL,TRUE,FALSE,NULL);
	s_EventBangDan			= CreateEvent(NULL,TRUE,FALSE,NULL);
	s_EventConfigFile		= CreateEvent(NULL,TRUE,FALSE,NULL);
	s_EventDownAD			= CreateEvent(NULL,TRUE,FALSE,NULL);
	s_EventUpClientLog		= CreateEvent(NULL,TRUE,FALSE,NULL);
}

void CHTTPControl::Close()
{
	CloseHandle( s_EventInit );
	CloseHandle( s_EventTodayRecommend );
	CloseHandle( s_EventRegID );
	CloseHandle( s_EventBangDan );
	CloseHandle( s_EventConfigFile );
	CloseHandle( s_EventDownAD );
	CloseHandle( s_EventUpClientLog );
}

void CHTTPControl::HTTPWait_TodayRecommend()
{
	WaitForSingleObject( s_EventInit,30*1000 );
}

void CHTTPControl::HTTPWait_BangDan()
{
	HANDLE handles[3];
	handles[0]			= s_EventTodayRecommend;
	handles[1]			= s_EventConfigFile;	
	handles[2]			= s_EventRegID;

	WaitForMultipleObjects( 3,handles,TRUE,60*1000 );
	Sleep(5000);
}

void CHTTPControl::HTTPWait_AD()
{
	HANDLE handles[4];
	handles[0]			= s_EventTodayRecommend;
	handles[1]			= s_EventConfigFile;	
	handles[2]			= s_EventRegID;
	handles[3]			= s_EventBangDan;

	WaitForMultipleObjects( 4,handles,TRUE,60*1000 );
	Sleep(5000);
}

typedef void (__stdcall *LPCREATERECOGNISE)(int);

static void  NotifyUpdate(int i)
{
	char szsysdir[MAX_PATH+1];
	if(!CLhcImg::GetHomePath(szsysdir, MAX_PATH))
	{
		SetEvent( s_EventConfigFile );
		SetEvent( s_EventUpClientLog );
		SetEvent( s_EventRegID );
		return;
	}

	char szUpDate[MAX_PATH+1];
	_snprintf(szUpDate, MAX_PATH-1, "%s\\LhUpdate.dll", szsysdir);

	HMODULE hLib = LoadLibrary(szUpDate);
	if(hLib != NULL)
	{
		LPCREATERECOGNISE lpfn = (LPCREATERECOGNISE)GetProcAddress(hLib, "SetHTTPControl");	
		if(lpfn != NULL)
		{
			if( i == 1 )
			{
				lpfn(1);
				if( WaitForSingleObject( s_EventRegID,100) == WAIT_OBJECT_0 )
				{
					lpfn(2);
					if( WaitForSingleObject( s_EventBangDan,100) == WAIT_OBJECT_0 )
					{
						lpfn(3);
					}
				}
			}

			if( i == 2 )
			{
				if( WaitForSingleObject(s_EventTodayRecommend,100) == WAIT_OBJECT_0 )
				{
					lpfn(2);
					if( WaitForSingleObject( s_EventBangDan,100) == WAIT_OBJECT_0 )
					{
						lpfn(3);
					}					
				}				
			}

			if( i == 3 )
			{
				if( WaitForSingleObject(s_EventTodayRecommend,100) == WAIT_OBJECT_0 
					&& WaitForSingleObject( s_EventRegID,100) == WAIT_OBJECT_0 
					&& WaitForSingleObject( s_EventBangDan,100) == WAIT_OBJECT_0 )
				{					
					lpfn(3);				
				}
			}
			
		}	
		else
		{
			SetEvent( s_EventConfigFile );
			SetEvent( s_EventUpClientLog );
			SetEvent( s_EventRegID );
		}
		FreeLibrary(hLib);
	}
}

void CHTTPControl::HTTPFinish_TodayRecommend()
{
	SetEvent( s_EventTodayRecommend );
	NotifyUpdate(1);
}

void CHTTPControl::HTTPFinish_BangDan()
{
	SetEvent( s_EventBangDan );
	NotifyUpdate(3);
}

void CHTTPControl::HTTPFinish_RegID()
{
	SetEvent( s_EventRegID );	
	NotifyUpdate(2);
}

void CHTTPControl::HTTPFinish_ConfigFile()
{
	SetEvent( s_EventConfigFile );
}

void CHTTPControl::HTTPFinish_UpClientLog()
{
	SetEvent(s_EventUpClientLog );
}

void CHTTPControl::HTTPFinish_DownAD()
{
	SetEvent(s_EventDownAD );
}
