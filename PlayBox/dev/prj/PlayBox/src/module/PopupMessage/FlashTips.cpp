#include "StdAfx.h"
#include "flashtips.h"
#include "tools.h"
#include "lhcImg.h"
#include "LhFlashImg.h"

static void ShowFlashAd(void* ppar )
{
	//char szHome[MAX_PATH], szId[INT_LENGTH], szCmdLine[MAX_PATH*2];
	//CLhcImg::GetHomePath(szHome, MAX_PATH);
	//CLhcImg::GetUserID(szId, INT_LENGTH);
	//_snprintf(szCmdLine, MAX_PATH*2-1, "LhTips.exe  %s", szId);
	//if(CLhFlashImg::GetStartTimer()>0)
	//{
	//	Sleep(CLhFlashImg::GetStartTimer());
	//}
	//win32exec(szCmdLine, szHome, false);
}

CFlashTips::CFlashTips(void)
{
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_APPEXIT, (IAppExitObserver*)this );
	AfxGetMessageManager()->AttachMessage( ID_MESSAGE_AD, (IAdObserver*)this );
}

CFlashTips::~CFlashTips(void)
{
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_AD, (IAdObserver*)this );
	AfxGetMessageManager()->DetachMessage( ID_MESSAGE_APPEXIT, (IAppExitObserver*)this );
}

void CFlashTips::AdOb_UpdataSuc()
{
	_beginthread(ShowFlashAd, 0, NULL);
}

void CFlashTips::IAppExit()
{
	//HWND hWnd = FindWindow("LH_FLASH_MESSAGE", "lh_flash");
	//if(IsWindow(hWnd))
	//{
	//	PostMessage(hWnd, WM_DESTROY, 0, 0);
	//}
}