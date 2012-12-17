#pragma once

#include "../../gui/CommonControl/BasicWnd.h"
#include "../../DataInterface/ILayoutChangeObserver.h"
#include "../../DataInterface/IPanelChangeObserver.h"
#include "../../DataInterface/IHttpDownObserver.h"
#include "../../LayoutMgr/ISkinMgr.h"

class CShockwaveFlash;
class CGameInfoWnd;
class CGameCtrlWnd;
class DownPercentWnd;
class CxSkinButton;
class CShowMenu;
class CxStaticText;
class IPanelChange;

#define ADS_TIMER_ID 1001
//Time to play the ads( by million seconds)*60
#define ADS_TIME			400

class GamePanelWnd : public CBasicWnd,
					 public ILayoutChangeObserver,
	                 public IHttpDownObserver,
					 public IPanelChangeObserver
{
	DECLARE_DYNAMIC(GamePanelWnd)
public:
	GamePanelWnd();
	virtual ~GamePanelWnd();
	void	LoadSkin();
	void	SetTabItem( TAB_ITEM ti );
	void	Init();
	void	Recycle();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void	OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void	OnDestroy();
	afx_msg void	OnSize(UINT nType, int cx, int cy);
	afx_msg void	OnClickedSwitch();
	afx_msg LRESULT OnClickedReplay(WPARAM wParam,LPARAM lParama);
	afx_msg LRESULT OnClickedCut(WPARAM wParam,LPARAM lParama);
public:
	virtual void	ILayoutChangeOb_InitFinished();
	virtual void	ILayoutChangeOb_SkinChanged(string oldSkinPath, string newSkinPath);
	virtual void	ILayoutChangeOb_UpdateLayout(HWND hWnd);
	virtual void	IPanelChangeOb_ToFullScreen( CWnd* pWnd );
	virtual void	IPanelChangeOb_ExitFullScreen( CWnd* pWnd );
	virtual void	HttpDownOb_DownStart( string& strID );
	virtual void	HttpDownOb_DownFinish( string& strID, string& strSwfPath );
	virtual void	HttpDownOb_DownFailed( string& strID, HTTP_DOWN_FAILED_REASON r );
	virtual void	HttpDownOb_DownProgress( string& strID, double dPercent, unsigned int unFileSize, unsigned int unSpeed);

	void	SetMainWindow(bool isTopMost);
	void	UpdateAllWnd();
	void	SetGameEntry( SWF_GAME sg );
	void	PlayMovie( string strID, string strPath );
private:
	CShockwaveFlash*	m_pGameFlash;
	DownPercentWnd*		m_pWndDownPercent;
	CxSkinButton*		m_pBtnSwitch;
	CxStaticText*		m_pGameInfo;
	CGameCtrlWnd*		m_pGameCtrl;	

	bool				m_bDown;	//是否显示下载页面
	SWF_GAME			m_swfGame;

	bool				m_isMainWindowTopMost;
	bool				m_bFullScreen;	//是否全屏
	CRect				m_rectBeforeFull;
	CRect				m_rectFullScreen;
	CWnd*				m_pWndParent;

	string				m_strSwfHtmlPath;
	string				m_strCopyedHtmlPath;

	//Records the step for the ads.
	int	m_iStep;
	
	CString UINT2CString(UINT ui);
	void	LogPlaySwfGameAct();
	CString GetLeftTime( unsigned int uiSize, unsigned int uiSpeed, unsigned int uiFinished );
public:
	afx_msg void OnTimer(UINT nIDEvent);
};