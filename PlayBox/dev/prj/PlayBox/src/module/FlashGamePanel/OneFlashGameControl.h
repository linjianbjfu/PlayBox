#pragma once
#include "..\..\Global\GlobalVariable.h"
#include "..\..\datainterface\IGameData.h"
#include "IHttpDownObserver.h"
#include "IPanelChangeObserver.h"

class MyWebBrowserWnd;
class CFlashGamePlay;
class CFlashGameDownloadWnd;
class ESCFullDlg;
class CShockwaveFlash;
class CFlashGamePanelWnd;
class CFlashGameStagePanelWnd;
class CFlashGameStageLeft;

class COneFlashGameControl : public IHttpDownObserver, public IPanelChangeObserver
{
public:
	COneFlashGameControl();
	~COneFlashGameControl();

	void SetBrowserIntro(MyWebBrowserWnd* pWnd);
	void SetBrowserRecommand(MyWebBrowserWnd* pWnd);
	void SetFlashPlay(CFlashGamePlay* pWnd);
	void SetFlashDownload(CFlashGameDownloadWnd* pWnd);
	void SetFlashCore(CShockwaveFlash* pWnd);
	void SetFlashGamePanel(CFlashGamePanelWnd* pWnd);
	void SetStage(CFlashGameStagePanelWnd* pWnd);
	void SetStageLeft(CFlashGameStageLeft* pWnd);
	void SetTabItem(TAB_ITEM ti);
	void RePlay();
	void ExitFullScreen();
	void ToFullScreen();
	void SetMute(bool mute);
	bool IsIntroShow();
	void ShowIntro(bool bShow);
	bool IsRecommandShow();
	void ShowRecommand(bool bShow);
	bool IsDownloading();
private:
	void NavigateIntro(std::string strID);
	void NavigateRecommand(std::string strID);
	void UpdateAllWnd();
	void PlayMovie(string strPath);
	CString UINT2CString(UINT ui);
	CString GetLeftTime( unsigned int uiSize, unsigned int uiSpeed, unsigned int uiFinished );
	void ShowHideEseFull(bool isShow);
	void IPanelChangeOb_ToFullScreen( CWnd* pWnd );
	void IPanelChangeOb_ExitFullScreen( CWnd* pWnd );
	void HttpDownOb_DownStart( string& strID );
	void HttpDownOb_DownFinish( string& strID, string& strSwfPath );
	void HttpDownOb_DownFailed( string& strID, HTTP_DOWN_FAILED_REASON r );
	void HttpDownOb_DownProgress( string& strID, double dPercent, unsigned int unFileSize, unsigned int unSpeed);
	void SetMainWindow(bool isTopMost);

	MyWebBrowserWnd* m_pBrowserIntro;
	MyWebBrowserWnd* m_pBrowserRecommand;
	CFlashGamePlay* m_pPlay;
	CShockwaveFlash* m_pFlashCore;
	CFlashGameDownloadWnd* m_pDownload;
	CFlashGamePanelWnd* m_pFlashGamePanel;
	CFlashGameStagePanelWnd* m_pStage;
	CFlashGameStageLeft* m_pStageLeft;
	TAB_ITEM m_tabItem;
	OneGame	m_olg;
	bool m_bDown;
	bool m_isMainWindowTopMost;
	bool m_bFullScreen;
	CWnd*				m_pWndParent;
	ESCFullDlg*			m_pEscFullTipDlg;
	CRect				m_rectBeforeFull;
	CRect				m_rectFullScreen;
};