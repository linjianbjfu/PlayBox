#pragma once
#include "SortInfoDef.h"

#include "../../gui/CommonControl/xSkinButton.h"
#include "../../DataInterface/IAppExitObserver.h"

class CDibBitmap;
class PlayedGameWnd;
struct GameVersion;
struct ThreadParameter;

class SortInfoWnd : public CWnd,
					public IAppExitObserver
{
	DECLARE_DYNAMIC(SortInfoWnd)

public:
	static SortInfoWnd * GetInstance ();
	static void			 DelInstance ();

	void SetPlayedGameWnd (PlayedGameWnd * pPlayedWnd);
	void SetPlayedGameCount (int nCnt);

private:
	SortInfoWnd();
	~SortInfoWnd();

	static	SortInfoWnd	* m_pThis;
	char m_strTotalGameCnt[MAX_PATH];

	COLORREF	m_txtColorOrig;
	COLORREF	m_txtColorHilight;

	PlayedGameWnd *m_playedWnd;

	CDibBitmap	* m_pBack;

private:
	CFont	m_font;	
	CFont*	m_oldFont;

	void DrawSortInfo (CDC * pDC);
	void DrawSortTypeText (CDC * pDC);

	CRect	m_rcTotalGameCnt;
	CRect	m_rcSortTitle;
	CRect	m_rcSortTime;
	CRect	m_rcSortGameType;
	CRect	m_rcSortPlayTimes;
	CRect	m_rcScaningText;
	CRect	m_rcAddGameButton;
	CRect	m_rcAddGameTriangle;
	CRect	m_rcCancelAddGame;

	CxSkinButton	* m_pbtnSortByTime;
	CxSkinButton	* m_pbtnSortByType;
	CxSkinButton	* m_pbtnSortByTimes;

	CxSkinButton	* m_pbtnAutoAddLocalGame;
	CxSkinButton	* m_pbtnManualAddLocalGame;
	CxSkinButton	* m_pbtnAddGameTriangle;
	CxSkinButton	* m_pbtnCancelAddGame;

	BOOL	m_bAutoAddLocalGame;
	BOOL	m_bScanning;
	string	m_strScanningText;

	void	ShowMenu ();

	std::vector<GameVersion *>	m_vGameVersion;
	void InitGameVersion ();

	static HANDLE m_hFindThread;
	static DWORD WINAPI FindGame (ThreadParameter *lpParameter);

	void _ManualScan ();
	void _AutoScan ();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int	 OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	afx_msg void OnAutoScan ();
	afx_msg void OnManualScan ();
public:
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void DrawScanningText (string strText);
	GameVersion*  CheckMD5 (string strFilePath);

	void	IAppExit();	
};