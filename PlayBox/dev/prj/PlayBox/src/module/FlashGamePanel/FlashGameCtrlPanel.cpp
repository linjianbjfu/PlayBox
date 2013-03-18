#include "stdafx.h"
#include "resource.h"
#include "FlashGameCtrlPanel.h"
#include "..\..\Gui\CommonControl\xSkinButton.h"
#include "OneFlashGameControl.h"
#include "MessageID.h"
#include "util\Sound.h"

IMPLEMENT_DYNAMIC(CFlashGameCtrlPanelWnd, CBasicWnd)

CFlashGameCtrlPanelWnd::CFlashGameCtrlPanelWnd(COneFlashGameControl* pCtrl)
{
	m_pCtrl = pCtrl; 
	m_pBtnRePlay   = new CxSkinButton();
	m_pBtnMute     = new CxSkinButton();
	m_pBtnUnMute   = new CxSkinButton();
	m_pBtnToFullScreen  = new CxSkinButton();
	m_pBtnExitFullScreen = new CxSkinButton();
	m_pBtnPause = new CxSkinButton();
	AfxGetMessageManager()->AttachMessage(ID_MESSAGE_MUTE,(IMuteMsgObserver*) this);	
}

CFlashGameCtrlPanelWnd::~CFlashGameCtrlPanelWnd()
{
	delete m_pBtnRePlay;
	delete m_pBtnMute;
	delete m_pBtnUnMute;
	delete m_pBtnToFullScreen;
	delete m_pBtnExitFullScreen;
	delete m_pBtnPause;
	AfxGetMessageManager()->DetachMessage(ID_MESSAGE_MUTE,(IMuteMsgObserver*) this);	
}

BEGIN_MESSAGE_MAP(CFlashGameCtrlPanelWnd, CBasicWnd)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_REPLAY,OnClickedReplay)
	ON_BN_CLICKED(IDC_BTN_FULL_SCREEN,OnClickedFullScreen)
	ON_BN_CLICKED(IDC_BTN_EXIT_FULL_SCREEN,OnClickedExitFullScreen)
	ON_BN_CLICKED(IDC_BTN_MUTE,OnClickedMute)
	ON_BN_CLICKED(IDC_BTN_UN_MUTE,OnClickedUnMute)
	ON_BN_CLICKED(IDC_BTN_PAUSE,OnClickedPause)
END_MESSAGE_MAP()

int CFlashGameCtrlPanelWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectNull(0,0,0,0);
	m_pBtnRePlay->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_REPLAY);
	m_pBtnToFullScreen->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_FULL_SCREEN);
	m_pBtnExitFullScreen->Create(NULL,WS_CHILD,rectNull,this,IDC_BTN_EXIT_FULL_SCREEN);
	m_pBtnMute->Create(NULL,WS_CHILD,rectNull,this,IDC_BTN_MUTE);
	m_pBtnUnMute->Create(NULL,WS_CHILD,rectNull,this,IDC_BTN_UN_MUTE);
	m_pBtnPause->Create (NULL, WS_VISIBLE, rectNull, this, IDC_BTN_PAUSE);
	m_bMute = CSound::GetInstance()->GetMute( 1 );
	UpdateBtnState();

	ILayoutMgr* pLayoutMgr =  AfxGetUIManager()->UIGetLayoutMgr();	
	pLayoutMgr->RegisterCtrl( this, "GameRePlay", m_pBtnRePlay );
	pLayoutMgr->RegisterCtrl( this, "ToFull", m_pBtnToFullScreen );
	pLayoutMgr->RegisterCtrl( this, "ExitFull", m_pBtnExitFullScreen );
	pLayoutMgr->RegisterCtrl( this, "Mute", m_pBtnMute );
	pLayoutMgr->RegisterCtrl( this, "UnMute", m_pBtnUnMute );
	pLayoutMgr->RegisterCtrl (this, "Pause", m_pBtnPause);

	pLayoutMgr->CreateControlPane( this, "flashgamectrlpanel", "normal" );
	pLayoutMgr->CreateBmpPane( this,"flashgamectrlpanel","normal" );

	return 0;
}

void CFlashGameCtrlPanelWnd::OnClickedReplay()
{
	m_pCtrl->RePlay();
}

void CFlashGameCtrlPanelWnd::OnClickedPause()
{
}

void CFlashGameCtrlPanelWnd::OnClickedCut()
{
	//string strDesktopPath;
	//char szTmpPath[MAX_PATH] = {0};
	//SHGetSpecialFolderPath (AfxGetMainWindow()->m_hWnd, szTmpPath, CSIDL_DESKTOP, FALSE);
	//strDesktopPath = szTmpPath;

	//if (!strDesktopPath.length ())
	//	return;

	//time_t now;
	//time (&now);

	//tm * tm_now = localtime (&now);

	//string strNow;
	//YL_StringUtil::Format (strNow, "%02d%02d%02d%02d%02d",
	//	tm_now->tm_mon + 1, tm_now->tm_mday,
	//	tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);

	//string strFileName;
	//YL_StringUtil::Format(strFileName, "%s\\%s.jpg",
	//	strDesktopPath.c_str(), strNow.c_str());

	//CRect rcWindow;
	//m_pGameFlash->GetClientRect(&rcWindow);

	//HDC hdcSrc = m_pGameFlash->GetDC()->GetSafeHdc();
	//int nBitPerPixel = GetDeviceCaps(hdcSrc, BITSPIXEL);
	//CImage image;
	//image.Create (rcWindow.Width (), rcWindow.Height (), nBitPerPixel);
	//BitBlt (image.GetDC (), 0, 0, rcWindow.Width (), rcWindow.Height (), 
	//	hdcSrc, rcWindow.left, rcWindow.top, SRCCOPY);

	//::ReleaseDC(NULL, hdcSrc);
	//image.ReleaseDC();
	//image.Save(strFileName.c_str(), Gdiplus::ImageFormatJPEG);

	//CString strTitle = "ÌבÊ¾";
	//CString strContent = CString(strFileName.c_str());

	//::SendMessage( AfxGetMainWindow()->m_hWnd, MSG_SHOW_TRAYICONBALLOON, WPARAM(&strTitle), LPARAM(&strContent) );
}


void CFlashGameCtrlPanelWnd::OnClickedMute()
{
	m_pCtrl->SetMute(true);
}

void CFlashGameCtrlPanelWnd::OnClickedUnMute()
{
	m_pCtrl->SetMute(false);
}

void CFlashGameCtrlPanelWnd::OnClickedFullScreen()
{
	m_pCtrl->ToFullScreen();
}

void CFlashGameCtrlPanelWnd::OnClickedExitFullScreen()
{
	m_pCtrl->ExitFullScreen();
}

void CFlashGameCtrlPanelWnd::MuteMsg_Change(bool bMute)
{
	m_bMute = bMute;
	UpdateBtnState();	
}

void CFlashGameCtrlPanelWnd::UpdateBtnState()
{
	m_pBtnMute->ShowWindow(m_bMute ? SW_HIDE : SW_SHOW);
	m_pBtnUnMute->ShowWindow(m_bMute ? SW_SHOW : SW_HIDE);
	Invalidate(TRUE);
}