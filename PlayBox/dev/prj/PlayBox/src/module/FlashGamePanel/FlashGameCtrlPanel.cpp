#include "stdafx.h"
#include "resource.h"
#include "FlashGameCtrlPanel.h"
#include "..\..\Gui\CommonControl\xSkinButton.h"

IMPLEMENT_DYNAMIC(CFlashGameCtrlPanelWnd, CBasicWnd)

CFlashGameCtrlPanelWnd::CFlashGameCtrlPanelWnd()
{
	m_pBtnRePlay   = new CxSkinButton();
	m_pBtnMute     = new CxSkinButton();
	m_pBtnUnMute   = new CxSkinButton();
	m_pBtnToFullScreen  = new CxSkinButton();
	m_pBtnExitFullScreen = new CxSkinButton();
	m_pBtnPause = new CxSkinButton();
}

CFlashGameCtrlPanelWnd::~CFlashGameCtrlPanelWnd()
{
	delete m_pBtnRePlay;
	delete m_pBtnMute;
	delete m_pBtnUnMute;
	delete m_pBtnToFullScreen;
	delete m_pBtnExitFullScreen;
	delete m_pBtnPause;
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
	m_pBtnExitFullScreen->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_EXIT_FULL_SCREEN);
	m_pBtnMute->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_MUTE);
	m_pBtnUnMute->Create(NULL,WS_VISIBLE,rectNull,this,IDC_BTN_UN_MUTE);
	m_pBtnPause->Create (NULL, WS_VISIBLE, rectNull, this, IDC_BTN_PAUSE);

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
	//std::string strLocalSwfPath;
	//m_olg.GetLocalSwfPath(strLocalSwfPath);
	//PlayMovie(strLocalSwfPath);
	//if(GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen())
	//{
	//	this->m_pWndRight->ShowWindow(SW_HIDE);
	//	this->m_pWndBottom->ShowWindow(SW_HIDE);
	//	CRect rect;
	//	this->GetClientRect(&rect);
	//	rect.top +=40;
	//	this->m_pGameFlash->MoveWindow(rect,false);
	//}
}

void CFlashGameCtrlPanelWnd::OnClickedPause()
{
	//if(GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen())
	//{
	//	this->m_pWndRight->ShowWindow(SW_HIDE);
	//	this->m_pWndBottom->ShowWindow(SW_HIDE);
	//	CRect rect;
	//	this->GetClientRect(&rect);
	//	rect.top +=40;
	//	this->m_pGameFlash->MoveWindow(rect,false);
	//}
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
	//bool bIsMute = CSound::GetInstance()->GetMute( 1 );
	//CSound::GetInstance()->SetMute( 1, !bIsMute );
	//UpdateAllWnd();
	//if(GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen())
	//{
	//	this->m_pWndRight->ShowWindow(SW_HIDE);
	//	this->m_pWndBottom->ShowWindow(SW_HIDE);
	//	CRect rect;
	//	this->GetClientRect(&rect);
	//	rect.top +=40;
	//	this->m_pGameFlash->MoveWindow(rect,false);
	//}
}

void CFlashGameCtrlPanelWnd::OnClickedUnMute()
{
	//bool bIsMute = CSound::GetInstance()->GetMute( 1 );
	//CSound::GetInstance()->SetMute( 1, !bIsMute );
	//UpdateAllWnd();
	//if(GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen())
	//{
	//	this->m_pWndRight->ShowWindow(SW_HIDE);
	//	this->m_pWndBottom->ShowWindow(SW_HIDE);
	//	CRect rect;
	//	this->GetClientRect(&rect);
	//	rect.top +=40;
	//	this->m_pGameFlash->MoveWindow(rect,false);
	//}
}

void CFlashGameCtrlPanelWnd::OnClickedFullScreen()
{
	//if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
	//	GLOBAL_PANELCHANGEDATA->IPanelChange_ExitFullScreen();
	//else
	//{
	//	GLOBAL_PANELCHANGEDATA->IPanelChange_ToFullScreen( this );
	//	this->m_pWndRight->ShowWindow(SW_HIDE);
	//	this->m_pWndBottom->ShowWindow(SW_HIDE);
	//	CRect rect;
	//	this->GetClientRect(&rect);
	//	rect.top +=40;
	//	this->m_pGameFlash->MoveWindow(rect,false);
	//}
}

void CFlashGameCtrlPanelWnd::OnClickedExitFullScreen()
{
	//if( GLOBAL_PANELCHANGEDATA->IPanelChange_IsFullScreen() )
	//	GLOBAL_PANELCHANGEDATA->IPanelChange_ExitFullScreen();
	//else
	//{
	//	GLOBAL_PANELCHANGEDATA->IPanelChange_ToFullScreen( this );
	//	this->m_pWndRight->ShowWindow(SW_SHOW);
	//	this->m_pWndBottom->ShowWindow(SW_SHOW);
	//}
}

